#pragma once
#include <boost/system/error_code.hpp>
#include <array>
#include "Const.h"

namespace cys {
namespace comm {
namespace app {
	struct UDPClientListener {
		virtual ~UDPClientListener() = default;
		virtual void onUDPClientConnected() = 0;
		virtual void onUDPClientDisconnected(const boost::system::error_code& e) = 0;
		virtual void onUDPClientSent(const boost::system::error_code& e) = 0;
		virtual void onUDPClientSentTo(const boost::system::error_code& e) = 0;
		virtual void onUDPClientReceived(const boost::system::error_code& e, const std::array<uint8_t, MAX_BUFFER_NUM>& data) = 0;
		virtual void onUDPClientError(const boost::system::error_code& e) = 0;
	};
}
}
}