#pragma once

namespace cys {
namespace comm {
namespace app {
	struct TCPClientListener {
		virtual ~TCPClientListener() = default;
		virtual void onTCPClientConnected() = 0;
		virtual void onTCPClientDisconnected(const boost::system::error_code& e) = 0;
		virtual void onTCPClientSent(const boost::system::error_code& e) = 0;
		virtual void onTCPClientReceived(const boost::system::error_code& e, const std::array<uint8_t, MAX_BUFFER_NUM>& data) = 0;
	};
}
}
}