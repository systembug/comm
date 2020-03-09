#pragma once
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include "Const.h"
#include "comm/Context.h"

namespace cys {
namespace comm {
namespace app {
	using tcp = boost::asio::ip::tcp;
	class TCPSession {
	public:
		TCPSession(cys::comm::Context* ctx)
			: m_context(ctx)
			, m_socket()
			, m_buffer()
		{}
		~TCPSession() = default;
		TCPSession(const TCPSession& session) = delete;
		TCPSession(TCPSession&& session) noexcept 
			: m_context(std::move(session.m_context))
			, m_socket(std::move(session.m_socket))
			, m_buffer(std::move(session.m_buffer))
		{}
		TCPSession& operator=(const TCPSession& session) = delete;
		TCPSession& operator=(TCPSession&& session) noexcept
		{
			if (this != &session) {
				m_context = std::move(session.m_context);
				m_socket = std::move(session.m_socket);
				m_buffer = std::move(session.m_buffer);
			}

			return *this;
		}

	public:
		bool create() {
			return true;
		}

		bool start() {
			return true;
		}

		bool stop() {
			return true;
		}

		bool destroy() {
			return true;
		}

	public:
		tcp::socket& getSocket() { return *m_socket.get(); }

	private:
		bool startReceiveAsync() {
			return true;
		}

	private:
		cys::comm::Context* m_context;
		std::unique_ptr<tcp::socket> m_socket;
		std::array<uint8_t, MAX_BUFFER_NUM> m_buffer;
	};
}
}
}