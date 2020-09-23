#pragma once
#include <boost/asio.hpp>
#include "comm/app/Const.h"

namespace cys {
namespace comm {
namespace app {
	struct TCPSessionListener {
		virtual ~TCPSessionListener() = default;
		virtual void onTCPSessionBind(std::size_t channel) = 0;
		virtual void onTCPSessionUnBind(std::size_t channel) = 0;
		virtual void onTCPSessionSent(std::size_t channel, const boost::system::error_code& e) = 0;
		virtual void onTCPSessionReceived(std::size_t channel, const boost::system::error_code& e, const std::array<uint8_t, MAX_BUFFER_NUM>& data) = 0;
	};
}
}
}