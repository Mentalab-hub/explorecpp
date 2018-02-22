#pragma once

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "base.h"

#include <thread>
#include <functional>
#include <memory>
#include <vector>
#include <array>

namespace explore {
    namespace detail {
	    // serial implementation

		template<typename _Socket>
		class socket_base {
		protected:
			_Socket ser_;
		public:
			explicit socket_base(boost::asio::io_service &io) :ser_(io) {}

			bool is_open() { return ser_.is_open(); }

			void close(boost::system::error_code &err) { ser_.close(err); }

			void cancel(boost::system::error_code &err) { ser_.cancel(); }

			_Socket &socket() { return ser_; }
			_Socket &operator()() { return socket(); }
			
		};

		class serial_base : public socket_base<boost::asio::serial_port>{
		public:
			explicit serial_base(boost::asio::io_service &io) :socket_base(io) {}

			void open(const std::string &device, boost::system::error_code &err) { ser_.open(device,err); }
		};

		class tcp_base : public socket_base<boost::asio::ip::tcp::socket>{
		public:
			explicit tcp_base(boost::asio::io_service &io) : socket_base(io) {}

			void open(const std::string &device, boost::system::error_code &err) {
				std::vector<std::string> _res(2);
				boost::split(_res, device, boost::is_any_of(":"));
				ser_.connect(boost::asio::ip::tcp::endpoint(
					boost::asio::ip::address::from_string(_res[0]), 
					boost::lexical_cast<uint16_t>(_res[1])),err);
			}
		};

	    template<class _Target, class _Parser, size_t _Headersize, size_t _Offset = -1, typename _OffsetType = uint8_t, size_t _Maxsize = 0xffff>
	    class io_base : private boost::noncopyable {
		private:
			boost::asio::io_service io_;
			std::unique_ptr<_Target> ser_ = nullptr;
			boost::asio::deadline_timer timer_{ io_ };

			_Parser &p_;

			std::string device_="";

			boost::asio::streambuf buf_;

			std::atomic<int32_t> read_timeout_{ 5 };
			std::atomic<int32_t> write_timeout_{ 5 };
			std::atomic<bool> brun{ false };

			std::future<void> fut_;
			std::mutex mut;

			bool bhead_;

			size_t amount_;
			size_t amount()const { return bhead_ || _Offset == -1 ? _Headersize : amount_; }

			bool connect() {
				boost::system::error_code err;
				std::lock_guard<std::mutex> lock(mut);
				{
					if (!ser_ || !ser_->is_open()) {
						ser_ = std::make_unique<_Target>(io_);
						ser_->open(device_,err);
					}
				}
				if(err){
					p_.forward_error(err.message());
					return false;
				}
				return ser_->is_open();
			}

			// stop & cancel everything, discard serial connection
			void close() {
				{
					std::lock_guard<std::mutex> lock(mut);
					if(ser_){
						boost::system::error_code err;
						try { ser_->cancel(err); }
						catch (std::runtime_error&) {}
						ser_->close(err);
						ser_.reset();
					}
				}
				io_.stop();
				io_.reset();

				bhead_ = true;
				buf_.consume(buf_.size());

				timer_.cancel(); // cancel timer
				timer_.expires_from_now(boost::posix_time::seconds(0));
			}

			// deadline timer for timeouts
			void deadline(const boost::system::error_code &error) {
				if (timer_.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
					if(!error && is_running()){
						close();
						p_.forward_error("timeout");
					}
					else p_.forward_error("stopped");
				}
				else timer_.async_wait([this](const boost::system::error_code& error)->void {this->deadline(error); });
			}

			void read_data() {
				if(!ser_)return;
				std::lock_guard<std::mutex> lock(mut);
				timer_.expires_from_now(boost::posix_time::seconds(read_timeout()));
				boost::asio::async_read(ser_->socket(), buf_, boost::asio::transfer_exactly(amount()),
				    [this](const boost::system::error_code &err, const size_t bt)->void {this->read_data_cb(err, bt); });
			}

			void read_data_cb(const boost::system::error_code &err, const size_t bt) {
				if (err || bt != amount()) {
					bhead_ = true;
					p_.forward_error(err.message());
					return;
				}
				if (bhead_ && _Offset >= 0) {
					bhead_ = false;
					const uint8_t *ptr = boost::asio::buffer_cast<const uint8_t*>(buf_.data());
					const int32_t end = *((_OffsetType*)(ptr + _Offset));
					if (end < 0 || end > _Maxsize)
						throw std::runtime_error{ "maxsize limit exceeded" };
					amount_ = end;
				}
				else {
					std::istream ibuf(&buf_);
					buffer_pointer _buf = std::make_unique<byte_buffer>(std::istreambuf_iterator<char>(ibuf), std::istreambuf_iterator<char>{});
					buf_.consume(bt);
					bhead_ = true;
					p_.parse_packet(std::move(_buf));
				}
				this->read_data();
			}

			template<typename _Buffer>
			void write_data(const _Buffer &b) {
				boost::asio::streambuf obuf(b.size());
				std::ostream os(&obuf);
				os << b;
				io_.post([this,obuf]()->void {
					this->timer_.expires_from_now(boost::posix_time::seconds(this->write_timeout()));
					boost::asio::async_write(this->ser_->socket(), obuf);
				});
			}

		public:
			explicit io_base(std::string device, _Parser &p) : device_(std::move(device)), p_(p), amount_(_Headersize),bhead_(true) {
				buf_.prepare(_Maxsize);
			}
			io_base(std::string device, _Parser &&p) = delete;

			~io_base(){ io_.stop(); }

			_Parser &parser() { return p_; }

			const std::string &device()const noexcept { return device_; }

			void device(const std::string &dev) {
				if (is_running()) {
					stop();
					device_ = dev;
				}
				else
					device_ = dev;
			}

			bool is_running()const noexcept { return brun; }

			int32_t read_timeout()const noexcept { return read_timeout_; }
			void read_timeout(const int32_t timeout) noexcept { read_timeout_ = timeout; }

			void start() {
				if(is_running())return;
				if(fut_.valid())
					fut_.wait(); // wait for current thread to exit
				brun = true;
				fut_ = std::async(std::launch::async, [this]() {
					while(brun){ // reconnect loop
						try {
							if (connect()) {
								timer_.expires_at(boost::posix_time::pos_infin);
								timer_.async_wait([this](const boost::system::error_code& error)->void {this->deadline(error); }); // start timer
								io_.post([this]()->void {read_data(); });
								io_.run();
							}
							else
								throw std::runtime_error("connection failed");
						}
						catch (std::runtime_error&) {
							close(); // close connection, sleep and retry
							if (brun) std::this_thread::sleep_for(std::chrono::milliseconds(500));
						}
					}
				});
			}

			void stop() {
				brun=false;
				close();
			}
		};
	}

	template<class _Parser, size_t _Headersize, size_t _Offset, typename _OffsetType, size_t _Maxsize = 0xffff>
	class serial_client : private boost::noncopyable {
	private:
		typedef detail::io_base<detail::serial_base, _Parser, _Headersize, _Offset, _OffsetType, _Maxsize> impl_type;
		impl_type _impl;
	public:
		typedef _Parser parser_type;

		// create client, caller must manage _Parser lifetime 
		explicit serial_client(const std::string &device, _Parser &p) : _impl(device, p) { }
		serial_client(const std::string &device, _Parser &&p) = delete;

		~serial_client() { stop(); }

		// get parser
		_Parser &parser() noexcept { return _impl.parser(); }

		// get and set device string
		const std::string &device()const noexcept { return  _impl.device(); }
		void device(const std::string &dev) { _impl.device(dev); }

		// read timeout
		int32_t read_timeout()const noexcept { return _impl.read_timeout(); }
		void read_timeout(const int32_t timeout) noexcept { _impl.read_timeout(timeout); }

		void start() { _impl.start(); }
		void stop() { _impl.stop(); }

		bool is_running()const noexcept { return _impl.is_running(); }
	};
}
