#pragma once
#include <boost/asio.hpp>
#include "comm/app/Const.h"

namespace cys {
	namespace comm {
		namespace app {
			struct UDPSessionListener {
				virtual ~UDPSessionListener() = default;
				virtual void onUDPSessionBind(std::size_t channel) = 0;
				virtual void onUDPSessionUnBind(std::size_t channel) = 0;
				virtual void onUDPSessionSent(std::size_t channel, const boost::system::error_code& e) = 0;
				virtual void onUDPSessionReceived(std::size_t channel, const boost::system::error_code& e, const std::array<uint8_t, MAX_BUFFER_NUM>& data) = 0;
			};
		}
	}
}