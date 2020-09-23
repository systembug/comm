#pragma once
#include <boost/asio.hpp>
#include <shared_mutex>
#include <memory>
#include <type_traits>
#include "Const.h"
#include "comm/Context.h"
#include "UDPClientListener.h"

namespace cys {
namespace comm {
namespace app {
	using udp = boost::asio::ip::udp;
	class UDPClient {		
	public:
		UDPClient(Context* ctx);
		~UDPClient();
		UDPClient(const UDPClient& client) = delete;
		UDPClient(UDPClient&& client) noexcept;
		UDPClient& operator=(const UDPClient& client) = delete;
		UDPClient& operator=(UDPClient&& client) noexcept;

	public:
		inline bool addListener(UDPClientListener* listener) {
			if (std::find(m_listeners.cbegin(), m_listeners.cend(), listener) != m_listeners.cend()) return false;
			m_listeners.emplace_back(listener);
			return true;
		}

		inline bool deleteListener(UDPClientListener* listener) {
			auto exist = std::find(m_listeners.begin(), m_listeners.end(), listener);
			if (exist == m_listeners.cend()) return false;
			*exist = std::move(m_listeners.back());
			m_listeners.pop_back();
			return true;
		}

	public:
		bool create();
		bool connect(const char* address, uint16_t port);
		inline bool connect(const std::string& address, uint16_t port) {
			return connect(address.c_str(), port);
		}
		bool connectAsync(const char* address, uint16_t port);
		inline bool connectAsync(const std::string& address, uint16_t port) {
			return connectAsync(address.c_str(), port);
		}

		template <class Data, typename std::enable_if<!std::is_base_of_v<std::streambuf,
			std::decay_t<Data>>>::type* = nullptr>
		bool send(Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;
			try {
				m_socket->send(boost::asio::buffer(std::forward<Data>(data)));
			}
			catch (boost::system::system_error& e) {
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onUDPClientError(e.code());
				}
				return false;
			}

			return true;
		}

		template <class StreamBuf, typename std::enable_if<std::is_base_of_v<std::streambuf, 
			std::decay_t<StreamBuf>>>::type* = nullptr>
		bool send(StreamBuf&& buf) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;
			try {
				auto n = m_socket->send(buf.data());
				buf.consume(n);
			}
			catch (boost::system::system_error& e) {
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onUDPClientError(e.code());
				}
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
					if (listener != nullptr) listener->onUDPClientSent(e);
				}
			});
			return true;
		}

		template <class Data>
		inline bool sendToBroadCast(uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_isConnecting.load()) {
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onUDPClientError(boost::system::error_code());
				}
				return false;
			}
			auto socket = udp::socket(m_context->getContext());
			socket.open(udp::v4());
			socket.set_option(boost::asio::socket_base::broadcast(true));
			socket.send_to(boost::asio::buffer(std::forward<Data>(data)),
				udp::endpoint(boost::asio::ip::address_v4::broadcast(), port));

			return true;
		}

		template <class Address, class Data>
		bool sendTo(Address&& address, uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = udp::socket(m_context->getContext());
			socket.open(udp::v4());
			socket.send_to(boost::asio::buffer(std::forward<Data>(data)), 
				udp::endpoint(boost::asio::ip::address::from_string(std::forward<Address>(address)), port));

			return true;
		}

		template <class Address, class Data>
		bool sendToAsync(Address&& address, uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = udp::socket(m_context->getContext());
			socket.open(udp::v4());
			socket.async_send_to(boost::asio::buffer(std::forward<Data>(data)),
				udp::endpoint(boost::asio::ip::address::from_string(std::forward<Address>(address)), port),
				[this](const boost::system::error_code& e, std::size_t transferred) {
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onUDPClientSentTo(e);
				}
			});
			return true;
		}

		template <class Data, typename std::enable_if<!std::is_base_of_v<std::streambuf, 
			std::decay_t<Data>>>::type* = nullptr>
		bool receive(Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;

			auto size = m_socket->receive(boost::asio::buffer(std::forward<Data>(data)));
			return true;
		}

		template <class StreamBuf, typename std::enable_if<std::is_base_of_v<std::streambuf,
			std::decay_t<StreamBuf>>>::type* = nullptr>
			bool receive(StreamBuf&& buf) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;

			auto size = m_socket->receive(buf);
			buf.commit(size);
			return true;
		}

		template <class Data, typename std::enable_if<!std::is_base_of_v<std::streambuf,
			std::decay_t<Data>>>::type* = nullptr>
		udp::endpoint receiveFrom(uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = udp::socket(m_context->getContext(), udp::endpoint(udp::v4(), port));
			udp::endpoint ep;
			socket.receive_from(boost::asio::buffer(data), ep);
			return ep;
		}

		template <class StreamBuf, typename std::enable_if<std::is_base_of_v<std::streambuf,
			std::decay_t<StreamBuf>>>::type* = nullptr>
		udp::endpoint receiveFrom(uint16_t port, StreamBuf&& streamBuf) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = udp::socket(m_context->getContext(), udp::endpoint(udp::v4(), port));
			udp::endpoint ep;
			auto buf = streamBuf.prepare(MAX_BUFFER_NUM);
			auto size = socket.receive_from(buf, ep);
			streamBuf.commit(size);
			return ep;
		}

		bool receiveAsync();		
		bool disconnect();
		bool destroy();
		inline bool isConnecting() const { return m_isConnecting.load(std::memory_order::memory_order_seq_cst); }
		inline uint16_t getPort() const { return m_port;  }

	private:
		void startReceiveAsync();

	private:
		mutable std::shared_mutex m_mutex;
		mutable std::recursive_mutex m_sckMtx;
		cys::comm::Context* m_context;

	private:
		uint16_t m_port;
		std::vector<UDPClientListener*> m_listeners;
		std::unique_ptr<udp::socket> m_socket;
		std::array<uint8_t, MAX_BUFFER_NUM> m_buffer;

	private:
		std::atomic<bool> m_isConnecting;
	};
}
}
}