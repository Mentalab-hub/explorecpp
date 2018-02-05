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
		explicit base_parser(_Forward &&f) = delete;

		_Forward &forward(){ return forward_; }

		void forward_error(std::string &&ex) { forward_.on_error(std::forward<std::string>(ex)); }

		void parse_packet(buffer_pointer &&buf) {
			const eeg_packet *ptr = (eeg_packet*)&(*buf)[0];
			const size_t datasize = buf->size()-sizeof(eeg_header)-sizeof(eeg_packet::timestamp)-4;
			switch (ptr->head.id)
			{
			case 19:
			{
				const size_t nsamples = datasize / sizeof(raw_sens_sample);
				sens_packet sens;
				sens.data.reserve(nsamples);
				sens.timestamp = ptr->timestamp;
				for (size_t n = 0; n < nsamples; ++n) {
					sens.data.push_back(sens_sample{
						ptr->_data.sens_samples[0].temperature,
						ptr->_data.sens_samples[0].light,
						ptr->_data.sens_samples[0].battery
					});
				}
				forward_.on_sens(std::move(sens));
				break;
			}
			case 13:
			{
				const size_t nsamples = datasize / sizeof(raw_acc_sample);
				acc_packet acc;
				acc.data.reserve(nsamples);
				acc.timestamp = ptr->timestamp;
				for (size_t n = 0; n < nsamples; ++n)
					acc.data.push_back(acc_sample{ 
						ptr->_data.acc_samples[n].data[0],
						ptr->_data.acc_samples[n].data[1],
						ptr->_data.acc_samples[n].data[2],
						ptr->_data.acc_samples[n].data[3],
						ptr->_data.acc_samples[n].data[4],
						ptr->_data.acc_samples[n].data[5],
						ptr->_data.acc_samples[n].data[6],
						ptr->_data.acc_samples[n].data[7],
						ptr->_data.acc_samples[n].data[8] 
					});
				forward_.on_acc(std::move(acc));
				break;
			}
			case 142:
			{
				const size_t nsamples = datasize / sizeof(raw_eeg_sample4);
				eeg4_packet eeg;
				eeg.data.reserve(nsamples);
				eeg.timestamp = ptr->timestamp;
				for (size_t n = 0; n < nsamples; ++n) {
					eeg_sample<4> smp;
					for (size_t i = 0; i < smp.size(); ++i)
						smp.data[i] = ptr->_data.eeg_samples4[n].data[i];
					smp.status = ptr->_data.eeg_samples4[n].status;
					eeg.data.push_back(smp);
				}
				forward_.on_eeg4(std::move(eeg));
				break;
			}
			case 146:
			{
				const size_t nsamples = datasize / sizeof(raw_eeg_sample8);
				eeg8_packet eeg;
				eeg.data.reserve(nsamples);
				eeg.timestamp = ptr->timestamp;
				for (size_t n = 0; n < nsamples; ++n) {
					eeg_sample<8> smp;
					for (size_t i = 0; i < smp.size(); ++i)
						smp.data[i] = ptr->_data.eeg_samples8[n].data[i];
					smp.status = ptr->_data.eeg_samples8[n].status;
					eeg.data.push_back(smp);
				}
				forward_.on_eeg8(std::move(eeg));
				break;
			}
			case 111:
				forward_.on_error("shutdown");
				break;
			default:
				forward_error("unknown packet id ");
				break;
			}
		}
	};
}
