#pragma once
#include <boost/asio.hpp>

namespace cys {
namespace comm {
namespace app {
	struct UDPServerListener {
		virtual ~UDPServerListener() = default;
		virtual void onUDPServerBinded() = 0;
		virtual void onUDPServerUnBinded() = 0;
		virtual void onUDPServerSent(const boost::system::error_code& e) = 0;
		virtual void onUDPServerSentTo(const boost::system::error_code& e) = 0;
		virtual void onUDPServerReceived(const boost::system::error_code& e, const std::array<uint8_t, MAX_BUFFER_NUM>& data) = 0;
	};
}
}
}