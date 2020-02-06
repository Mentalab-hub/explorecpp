#pragma once

#include "data_structs.h"
#include "parsed_structs.h"
#include "base.h"

namespace explore {

	template<class _Forward>
	class base_parser final : private boost::noncopyable {
		_Forward &forward_;
	public:
		typedef _Forward forward_type;

		explicit base_parser(_Forward &f) :forward_(f) {}
		base_parser(_Forward &&f) = delete;

		_Forward &forward() { return forward_; }

		void forward_error(std::string &&ex) { forward_.on_error(std::forward<std::string>(ex)); }

		void forward_connect(bool &&state) { forward_.on_connect(std::move(state)); }

		bool parse_packet(buffer_pointer &&buf) {
			const eeg_packet *ptr = reinterpret_cast<const eeg_packet*>(buf->data());
			const size_t datasize = buf->size() - sizeof(eeg_header) - sizeof(eeg_packet::timestamp) - 4;

			float vref = 4.5f;

			switch (ptr->head.id)
			{
			//env packet
			case 0x13:
			{
				const size_t nsamples = datasize / sizeof(raw_sens_sample);
				if (sens_packet::size() != nsamples) { forward_error("packet size mismatch"); break; }
				sens_packet sens;
				sens.timestamp = ptr->timestamp;
				for (size_t n = 0; n < sens.size(); ++n)
					sens.data[n] = sens_sample{
						(ptr->_data.sens_samples[n].temperature)*1.0,
						(ptr->_data.sens_samples[n].light)*(1000. / 4095.),
						(ptr->_data.sens_samples[n].battery)*((16.8 / 6.8) * (1.8 / 2457.))
				};
				forward_.on_sens(std::move(sens));
				break;
			}
			//orn packet
			case 0x0d:
			{
				const size_t nsamples = datasize / sizeof(raw_acc_sample);
				if (acc_packet::size() != nsamples) { forward_error("packet size mismatch"); break; }
				acc_packet acc;
				acc.timestamp = ptr->timestamp;
				for (size_t n = 0; n < acc.size(); ++n) {
					acc_sample &s = acc.data[n];
					for (size_t i = 0; i < s.values.size(); ++i)
						s.values[i] = ptr->_data.acc_samples[n].data[i];
				}
				forward_.on_acc(std::move(acc));
				break;
			}
			//eeg 94
			case 0x90:
			//eeg 94R
			case 0xD0:
				vref = 2.4f;
			//???
			case 0x1e:
			{
				const size_t nsamples = datasize / sizeof(raw_eeg_sample4);
				if (eeg4_packet::size() != nsamples) { forward_error("packet size mismatch"); break; }
				eeg4_packet eeg;
				eeg.vref = vref;
				eeg.timestamp = ptr->timestamp;
				parse_eeg_packet(eeg, ptr->_data.eeg_samples4);
				forward_.on_eeg4(std::move(eeg));
				break;
			}
			//eeg 98
			case 0x92:
			//eeg98R
			case 0xD2:
				vref = 2.4f;
			//eeg99
			case 0x3e:
			{
				const size_t nsamples = datasize / sizeof(raw_eeg_sample8);
				if (eeg8_packet::size() != nsamples) { forward_error("packet size mismatch"); break; }
				eeg8_packet eeg;
				eeg.vref = vref;
				eeg.timestamp = ptr->timestamp;
				parse_eeg_packet(eeg, ptr->_data.eeg_samples8);
				forward_.on_eeg8(std::move(eeg));
				break;
			}
			//ID_DISCONNECT 
			case 0x6f:
				forward_error("shutdown");
				break;
			//ID_DEV_INFO 
			case 0x63:
				devinfo_packet device_info;
				device_info.version = ptr->_data.dev_info->version;
				device_info.datarate = ptr->_data.dev_info->datarate;
				device_info.mask = ptr->_data.dev_info->mask;
				forward_.on_info(std::move(device_info));
				break;
			//ID_MARKER
			case 0xc2:
			{
				marker_packet marker;
				//marker.timestamp = ptr->timestamp;
				//marker.data[0] = marker_sample{ ptr->_data.marker_counter[0]};
				marker.counter = ptr->_data.marker_info->counter;
				forward_.on_marker(std::move(marker));
				break;
			}

			//ID_CALIB_INFO
			case 0xc3:
			{
				calib_info_packet calibration_info;
				calibration_info.timestamp = ptr->timestamp;
				calibration_info.data[0] = calib_info{
					ptr->_data.calib_infos->offset,
					ptr->_data.calib_infos->slope,
				};
				forward_.on_calib_info(std::move(calibration_info));
				break;
			}
			default:
				forward_error("unknown packet id ");
				break;
			}
			return true;
		}

	private:
		template<typename _Packet, typename _Datatype>
		void parse_eeg_packet(_Packet &eeg, const _Datatype &d) {
			for (size_t n = 0; n < eeg.size(); ++n) {
				auto &smp = eeg.data[n];
				for (size_t i = 0; i < eeg.channels(); ++i)
					smp.data[i] = d[n].data[i]* 4.5 / ((2^23) - 1) / 6.;
				smp.status = d[n].status;
			}
		}
	};
}
