#pragma once
#include <shared_mutex>
#include <unordered_map>
#include "TCPSession.hpp"
#include "comm/Context.h"
#include "comm/app/TCPServerListener.h"
#include "TCPSessionListener.h"

namespace cys {
namespace comm {
namespace app {
	using tcp = boost::asio::ip::tcp;
	class TCPServer : public TCPSessionListener {
	public:
		TCPServer(Context* ctx);
		virtual ~TCPServer();
		TCPServer(const TCPServer& server) = delete;
		TCPServer(TCPServer&& server) noexcept;
		TCPServer& operator=(const TCPServer& server) = delete;
		TCPServer& operator=(TCPServer&& server) noexcept;

	public:
		inline bool addListener(TCPServerListener* listener) {
			if (std::find(m_listeners.cbegin(), m_listeners.cend(), listener) != m_listeners.cend()) return false;
			m_listeners.emplace_back(listener);
			return true;
		}

		inline bool deleteListener(TCPServerListener* listener) {
			auto exist = std::find(m_listeners.begin(), m_listeners.end(), listener);
			if (exist == m_listeners.cend()) return false;
			*exist = std::move(m_listeners.back());
			m_listeners.pop_back();
			return true;
		}

	public:
		bool create(uint16_t port);
		bool bind();

		template <class Data>
		bool send(std::size_t channel, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (!m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;
			if (m_sessions.find(channel) == m_sessions.end()) return false;
			m_sessions[channel]->getSocket().send(boost::asio::buffer(std::forward<Data>(data)));
			return true;
		}

		template <class Data>
		bool sendAsync(std::size_t channel, Data&& data) {
			return true;
		}

		template <class Data>
		bool sendAll(Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (!m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;
			if (m_sessions.empty()) return false;
			for (auto& session : m_sessions) {
				auto& socket = session.second->getSocket();
				socket.send(boost::asio::buffer(std::forward<Data>(data)));
			}

			return true;
		}

		template <class Data>
		bool sendAllAsync(Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (!m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;
			if (m_sessions.empty()) return false;
			for (auto& session : m_sessions) {
				auto& socket = session.second->getSocket();
				socket.async_send(boost::asio::buffer(std::forward<Data>(data)),
					[this](const boost::system::error_code& e, std::size_t transferred) {
						for (auto& listener : m_listeners) {
							// if (listener != nullptr) listener->onTCPServerSent(e);
						}
					});
			}

			return true;

		}

		bool unBind();
		bool destroy();

	public:
		void onTCPSessionBind(std::size_t channel) override;
		void onTCPSessionUnBind(std::size_t channel) override;
		void onTCPSessionSent(std::size_t channel, const boost::system::error_code& e) override;
		void onTCPSessionReceived(std::size_t channel, const boost::system::error_code& e, const std::array<uint8_t, MAX_BUFFER_NUM>& data) override;

	private:
		std::shared_ptr<TCPSession> createSession();
		std::shared_ptr<TCPSession>& getSession(std::size_t channel);
		bool deleteSession(std::size_t channel);
		bool findSession(std::size_t channel);
		void clearSession();

	private:
		void startAcceptAsync();
		void handleAcceptAsync(std::shared_ptr<TCPSession> session,
			const boost::system::error_code& error);

	private:
		mutable std::shared_mutex m_mutex;
		mutable std::recursive_mutex m_conMtx;
		mutable std::shared_mutex m_ssMtx;
		cys::comm::Context* m_context;
		std::unique_ptr<tcp::acceptor> m_acceptor;

	private:
		uint16_t m_port;
		std::vector<TCPServerListener*> m_listeners;
		std::unordered_map<std::size_t, std::shared_ptr<TCPSession>> m_sessions;
		std::array<uint8_t, MAX_BUFFER_NUM> m_buffer;
		std::atomic<bool> m_isBinding;
	};
}
}
}