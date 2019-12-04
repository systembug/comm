#pragma once
#include <shared_mutex>
#include "comm/Context.h"
#include "comm/app/TCPServerListener.h"

namespace cys {
namespace comm {
namespace app {
	using tcp = boost::asio::ip::tcp;
	class TCPServer {
	public:
		TCPServer(Context* ctx);
		~TCPServer();
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
		bool send(Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;
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
			if (!m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;
			m_socket->async_send(boost::asio::buffer(std::forward<Data>(data)),
				[](const boost::system::error_code& e, std::size_t transferred) {
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onReceivedSent(e);
				}
			});
			return true;
		}

		template <class Data>
		inline bool sendToBroadCast(uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = udp::socket(m_context->getContext());
			socket.open(udp::v4());
			socket.set_option(boost::asio::socket_base::broadcast(true));
			socket.send_to(boost::asio::buffer(std::forward<Data>(data)),
				udp::endpoint(boost::asio::ip::address_v4::broadcast(), port));

			return true;
		}

		bool unBind();
		bool destroy();

	private:
		bool startReceiveAsync();

	private:
		mutable std::shared_mutex m_mutex;
		mutable std::recursive_mutex m_sckMtx;
		cys::comm::Context* m_context;
		std::unique_ptr<tcp::socket> m_socket;

	private:
		uint16_t m_port;
		std::vector<TCPServerListener*> m_listeners;
		std::array<uint8_t, MAX_BUFFER_NUM> m_buffer;
		std::atomic<bool> m_isBinding;
	};
}
}
}