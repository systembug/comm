#pragma once
#include <shared_mutex>
#include <boost/asio.hpp>
#include "comm/Context.h"
#include "comm/app/Const.h"
#include "comm/app/TCPClientListener.h"

namespace cys {
namespace comm {
namespace app {
	using tcp = boost::asio::ip::tcp;
	class TCPClient {
	public:
		TCPClient(Context* ctx);
		~TCPClient();
		TCPClient(const TCPClient& client) = delete;
		TCPClient(TCPClient&& client) noexcept;
		TCPClient& operator=(const TCPClient& client) = delete;
		TCPClient& operator=(TCPClient&& client) noexcept;

	public:
		inline bool addListener(TCPClientListener* listener) {
			if (std::find(m_listeners.cbegin(), m_listeners.cend(), listener) != m_listeners.cend()) return false;
			m_listeners.emplace_back(listener);
			return true;
		}

		inline bool deleteListener(TCPClientListener* listener) {
			auto exist = std::find(m_listeners.begin(), m_listeners.end(), listener);
			if (exist == m_listeners.cend()) return false;
			*exist = std::move(m_listeners.back());
			m_listeners.pop_back();
			return true;
		}

	public:
		bool create();
		bool create(uint16_t port);
		bool connect(const char* address, uint16_t port);
		inline bool connect(const std::string& address, uint16_t port) {
			return connect(address.c_str(), port);
		}
		bool connectAsync(const char* address, uint16_t port);
		inline bool connectAsync(const std::string& address, uint16_t port) {
			return connectAsync(address.c_str(), port);
		}

		template <class Data>
		bool send(Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;
			try {
				m_socket->send(boost::asio::buffer(std::forward<Data>(data)));
			}
			catch (boost::system::system_error& e) {
				e.code();
				return false;
			}

			return true;
		}

		template <class Data>
		bool sendAsync(Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;
			m_socket->async_send(boost::asio::buffer(std::forward<Data>(data)),
				[this](const boost::system::error_code& e, std::size_t transferred) {
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onTCPClientSent(e);
				}
			});
			return true;
		}

		template <class Data>
		bool receive(Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;

			auto size = m_socket->receive(boost::asio::buffer(std::forward<Data>(data)));
			return true;
		}

		bool receiveAsync();
		bool disconnect();
		bool destroy();
		inline bool isConnecting() { return m_isConnecting.load(std::memory_order::memory_order_seq_cst); }

	private:
		void startReceiveAsync();
		void resetSocket();

	private:
		mutable std::shared_mutex m_mutex;
		mutable std::recursive_mutex m_sckMtx;
		cys::comm::Context* m_context;

	private:
		std::vector<TCPClientListener*> m_listeners;
		std::unique_ptr<tcp::socket> m_socket;
		std::array<uint8_t, MAX_BUFFER_NUM> m_buffer;

	private:
		std::atomic<bool> m_isConnecting;
	};
}
}
}