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
			, m_mutex()
			, m_isCreated(false)
			, m_socket()
			, m_buffer()
		{}
		~TCPSession() = default;
		TCPSession(const TCPSession& session) = delete;
		TCPSession(TCPSession&& session) noexcept 
			: m_context(std::move(session.m_context))
			, m_mutex()
			, m_isCreated(std::move(session.m_isCreated))
			, m_socket(std::move(session.m_socket))
			, m_buffer(std::move(session.m_buffer))
		{}
		TCPSession& operator=(const TCPSession& session) = delete;
		TCPSession& operator=(TCPSession&& session) noexcept
		{
			if (this != &session) {
				std::unique_lock<std::mutex> lhsLock(m_mutex, std::defer_lock);
				std::unique_lock<std::mutex> rhsLock(session.m_mutex, std::defer_lock);
				std::lock(lhsLock, rhsLock);
				m_context = std::move(session.m_context);
				m_isCreated = std::move(session.m_isCreated);
				m_socket = std::move(session.m_socket);
				m_buffer = std::move(session.m_buffer);
			}

			return *this;
		}

	public:
		bool create() {
			std::unique_lock<std::mutex> m_mutex;
			if (m_isCreated) return false;
			m_socket = std::make_unique<tcp::socket>(m_context->getContext());
			m_isCreated = true;
			return true;
		}

		bool start() {
			return true;
		}

		bool stop() {
			return true;
		}

		bool destroy() {
			std::unique_lock<std::mutex> m_mutex;
			if (!m_isCreated) return true;
			boost::system::error_code ec;
			m_socket->shutdown(tcp::socket::shutdown_both, ec);
			m_socket->close();
			m_socket.reset();
			m_isCreated = false;
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
		mutable std::mutex m_mutex;
		bool m_isCreated;
		std::unique_ptr<tcp::socket> m_socket;
		std::array<uint8_t, MAX_BUFFER_NUM> m_buffer;
	};
}
}
}