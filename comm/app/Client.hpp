#pragma once
#include <type_traits>

namespace cys {
namespace comm {
namespace app {
	template <class Transport>
	class Client {
	public:
		Client(Context* ctx)
			: m_mutex()
			, m_sckMtx()
			, m_context(ctx)
			, m_port(0)
			, m_listeners()
			, m_socket(nullptr)
			, m_buffer()
			, m_isConnecting(false)
		{}
		~Client()
		{
			destroy();
		}
		Client(const Client& client) = delete;
		Client(Client&& client) noexcept
			: m_mutex()
			, m_sckMtx()
		{
			std::unique_lock<std::shared_mutex> lock(client.m_mutex);
			std::unique_lock<std::recursive_mutex> lk(client.m_sckMtx);
			m_context = std::move(client.m_context);
			m_port = std::move(client.m_port);
			m_listeners = std::move(client.m_listeners);
			m_socket = std::move(client.m_socket);
			m_buffer = std::move(client.m_buffer);
			m_isConnecting = client.m_isConnecting.load(std::memory_order::memory_order_seq_cst);
		}
		Client& operator=(const Client& client) = delete;
		Client& operator=(Client&& client) noexcept
		{
			if (this != &client) {
				std::unique_lock<std::shared_mutex> lk1(client.m_mutex, std::defer_lock);
				std::unique_lock<std::shared_mutex> lk2(m_mutex, std::defer_lock);
				std::unique_lock<std::recursive_mutex> lk3(client.m_sckMtx, std::defer_lock);
				std::unique_lock<std::recursive_mutex> lk4(m_sckMtx, std::defer_lock);
				std::lock(lk1, lk2, lk3, lk4);
				m_context = std::move(client.m_context);
				m_port = std::move(client.m_port);
				m_listeners = std::move(client.m_listeners);
				m_socket = std::move(client.m_socket);
				m_buffer = std::move(client.m_buffer);
				m_isConnecting = client.m_isConnecting.load(std::memory_order::memory_order_seq_cst);
			}

			return *this;
		}

	public:
		inline bool addListener(Transport::Listener* listener) {
			if (std::find(m_listeners.cbegin(), m_listeners.cend(), listener) != m_listeners.cend()) return false;
			m_listeners.emplace_back(listener);
			return true;
		}

		inline bool deleteListener(Transport::Listener* listener) {
			auto exist = std::find(m_listeners.begin(), m_listeners.end(), listener);
			if (exist == m_listeners.cend()) return false;
			*exist = std::move(m_listeners.back());
			m_listeners.pop_back();
			return true;
		}

	public:
		inline bool create() { return create(0); }
		bool create(uint16_t port) {
			std::unique_lock<std::shared_mutex> lock(m_mutex);

			m_port = port;
			m_socket = std::make_unique<Transport::socket>(m_context->getContext(), Transport::endpoint(Transport::v4(), m_port));
			return true;
		}
		bool connect(const char* address, uint16_t port)
		{
			std::unique_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;
			if (port <= 0) return false;
			m_socket->connect(Transport::endpoint(boost::asio::ip::address::from_string(address), port));
			m_isConnecting.store(true, std::memory_order::memory_order_seq_cst);

			for (auto& listener : m_listeners) {
				if (listener != nullptr) listener->onClientConnected();
			}

			return true;
		}
		inline bool connect(const std::string& address, uint16_t port) {
			return connect(address.c_str(), port);
		}
		bool connectAsync(const char* address, uint16_t port) {
			std::unique_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;
			if (port <= 0) return false;
			m_socket->async_connect(Transport::endpoint(boost::asio::ip::address::from_string(address), port),
				[this](const boost::system::error_code& e) {
				if (!e) {
					m_isConnecting.store(true, std::memory_order::memory_order_seq_cst);
					for (auto& listener : m_listeners) {
						if (listener != nullptr) listener->onClientConnected();
					}
				}
				else {
					for (auto& listener : m_listeners) {
						if (listener != nullptr) listener->onClientDisconnected(e);
					}
				}
			});

			return true;
		}
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
					if (listener != nullptr) listener->onClientSent(e);
				}
			});
			return true;
		}

		template <class Data, template std::enable_if<std::same_v<Transport, cys::comm::transport::UDP>>::type* = nullptr>
		inline bool sendToBroadCast(uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = Transport::socket(m_context->getContext());
			socket.open(Transport::v4());
			socket.set_option(boost::asio::socket_base::broadcast(true));
			socket.send_to(boost::asio::buffer(std::forward<Data>(data)),
				Transport::endpoint(boost::asio::ip::address_v4::broadcast(), port));

			return true;
		}

		template <class Address, class Data, 
			template std::enable_if<std::same_v<Transport, cys::comm::transport::UDP>>::type* = nullptr>
		bool sendTo(Address&& address, uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = Transport::socket(m_context->getContext());
			socket.open(Transport::v4());
			socket.send_to(boost::asio::buffer(std::forward<Data>(data)),
				Transport::endpoint(boost::asio::ip::address::from_string(std::forward<Address>(address)), port));

			return true;
		}

		template <class Address, class Data, 
			template std::enable_if<std::same_v<Transport, cys::comm::transport::UDP>>::type* = nullptr>
		bool sendToAsync(Address&& address, uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = Transport::socket(m_context->getContext());
			socket.open(Transport::v4());
			socket.async_send_to(boost::asio::buffer(std::forward<Data>(data)),
				Transport::endpoint(boost::asio::ip::address::from_string(std::forward<Address>(address)), port),
				[this](const boost::system::error_code& e, std::size_t transferred) {
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onClientSentTo(e);
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

		template <class Data, template std::enable_if<std::same_v<Transport, cys::comm::transport::UDP>>::type* = nullptr>
		Transport::endpoint receiveFrom(uint16_t port, Data&& data) {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			auto socket = Transport::socket(m_context->getContext(), Transport::endpoint(Transport::v4(), port));
			Transport::endpoint ep;
			socket.receive_from(boost::asio::buffer(data), ep);
			return ep;
		}

		bool receiveAsync() {
			std::unique_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return false;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;

			startReceiveAsync();
			return true;
		}

		bool disconnect() {
			std::shared_lock<std::shared_mutex> lock(m_mutex);
			if (m_socket.get() == nullptr) return true;
			if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return true;

			m_isConnecting.store(false, std::memory_order::memory_order_seq_cst);

			boost::system::error_code ec;
			m_socket->shutdown(Transport::socket::shutdown_both, ec);
			if (m_socket->is_open()) m_socket->close();

			for (auto& listener : m_listeners) {
				if (listener != nullptr) listener->onClientDisconnected(boost::system::error_code());
			}

			return true;
		}

		bool destroy() {
			disconnect();
			{
				std::unique_lock<std::recursive_mutex> lock(m_sckMtx);
				m_socket.reset(nullptr);
			}
			return true;
		}

		inline bool isConnecting() { return m_isConnecting.load(std::memory_order::memory_order_seq_cst); }

	private:
		void startReceiveAsync() {
			std::unique_lock<std::recursive_mutex> lock(m_sckMtx);
			if (m_socket.get() != nullptr) {
				m_socket->async_receive(boost::asio::buffer(m_buffer),
					[this](const boost::system::error_code& err, std::size_t transferred) {
					if (!err && m_isConnecting.load(std::memory_order::memory_order_seq_cst)) {
						for (auto& listener : m_listeners) {
							if (listener != nullptr) listener->onClientReceived(err, m_buffer);
						}
						startReceiveAsync();
					}
					for (auto& listener : m_listeners) {
						if (listener != nullptr) listener->onClientError(err);
					}
				});
			}
		}

	private:
		mutable std::shared_mutex m_mutex;
		mutable std::recursive_mutex m_sckMtx;
		cys::comm::Context* m_context;

	private:
		uint16_t m_port;

		std::vector<Transport::Listener*> m_listeners;
		std::unique_ptr<Transport::socket> m_socket;
		std::array<uint8_t, MAX_BUFFER_NUM> m_buffer;

	private:
		std::atomic<bool> m_isConnecting;
	};
}
}
}