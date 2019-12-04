#pragma once
#include <boost/asio.hpp>
#include "comm/app/Const.h"

namespace cys {
namespace comm {
namespace app {
	struct TCPServerListener {
		virtual ~TCPServerListener() = default;
		virtual void onTCPServerBinded() = 0;
		virtual void onTCPServerUnBinded() = 0;
		virtual void onTCPServerSent(const boost::system::error_code& e) = 0;
		virtual void onTCPServerReceived(const boost::system::error_code& e, const std::array<uint8_t, MAX_BUFFER_NUM>& data) = 0;
	};
}
}
}