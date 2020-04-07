#pragma once
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include "Const.h"
#include "TCPSessionListener.h"
#include "comm/Context.h"

namespace cys {
namespace comm {
namespace app {
	using tcp = boost::asio::ip::tcp;
	class TCPSession {
	public:
		TCPSession(cys::comm::Context* ctx, std::size_t channel)
			: m_context(ctx)
			, m_channel(0)
			, m_mutex()
			, m_listeners()
			, m_isCreated(false)
			, m_socket()
			, m_buffer()
		{}
		~TCPSession() = default;
		TCPSession(const TCPSession& session) = delete;
		TCPSession(TCPSession&& session) noexcept 
			: m_context(std::move(session.m_context))
			, m_channel(std::move(session.m_channel))
			, m_mutex()
			, m_listeners(std::move(session.m_listeners))
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
				m_channel = std::move(session.m_channel);
				m_listeners = std::move(session.m_listeners);
				m_isCreated = std::move(session.m_isCreated);
				m_socket = std::move(session.m_socket);
				m_buffer = std::move(session.m_buffer);
			}

			return *this;
		}

	public:
		inline bool addListener(TCPSessionListener* listener) {
			if (std::find(m_listeners.begin(), m_listeners.end(), listener) != m_listeners.end()) return false;
			m_listeners.emplace_back(listener);
			return true;
		}

		inline bool deleteListener(TCPSessionListener* listener) {
			if (std::find(m_listeners.begin(), m_listeners.end(), listener) == m_listeners.end()) return false;
			m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
			return true;
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
			std::unique_lock<std::mutex> m_mutex;
			if (!m_isCreated) return false;
			if (!startReceiveAsync()) return false;
			return true;
		}

		bool stop() {
			std::unique_lock<std::mutex> m_mutex;
			return true;
		}

		bool destroy() {
			std::unique_lock<std::mutex> m_mutex;
			if (!m_isCreated) return true;
			closeSocket();

			for (auto& listener : m_listeners)
				if (listener != nullptr) listener->onTCPSessionUnBind(m_channel);

			m_isCreated = false;
			return true;
		}

	public:
		tcp::socket& getSocket() { return *m_socket.get(); }

	private:
		bool startReceiveAsync() {
			if (m_socket.get() != nullptr) {
				m_socket->async_receive(boost::asio::buffer(m_buffer),
					[this](const boost::system::error_code& err, std::size_t transferred) {
						if (!err) {
							for (auto& listener : m_listeners) {
								if (listener != nullptr) 
									listener->onTCPsessionReceived(m_channel, err, m_buffer);
							}
							startReceiveAsync();
						}	
						else {
							closeSocket();
						}
					});
			}

			return true;
		}

		bool closeSocket() {
			boost::system::error_code ec;
			m_socket->shutdown(tcp::socket::shutdown_both, ec);
			m_socket->close();
			m_socket.reset();

			return true;
		}

	private:
		cys::comm::Context* m_context;
		std::size_t m_channel;
		mutable std::mutex m_mutex;
		std::vector<TCPSessionListener*> m_listeners;
		bool m_isCreated;
		std::unique_ptr<tcp::socket> m_socket;
		std::array<uint8_t, MAX_BUFFER_NUM> m_buffer;
	};
}
}
}