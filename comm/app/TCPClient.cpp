#include "TCPClient.h"

namespace cys {
namespace comm {
namespace app {
	TCPClient::TCPClient(Context* ctx)
		: m_mutex()
		, m_sckMtx()
		, m_context(ctx)
		, m_port(0)
		, m_listeners()
		, m_socket(nullptr)
		, m_buffer()
		, m_isConnecting(false)
	{}

	TCPClient::~TCPClient()
	{
		destroy();
	}

	TCPClient::TCPClient(TCPClient&& client) noexcept
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

	TCPClient& TCPClient::operator=(TCPClient&& client) noexcept
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

	bool TCPClient::create(uint16_t port)
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);

		m_port = port;
		try {
			m_socket = std::make_unique<tcp::socket>(m_context->getContext(), tcp::endpoint(tcp::v4(), m_port));
		}
		catch (boost::system::system_error& e) {
			for (auto& listener : m_listeners) {
				if (listener != nullptr) listener->onTCPClientError(e.code());
			}
		}
		return true;
	}

	bool TCPClient::connect(const char* address, uint16_t port)
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (m_socket.get() == nullptr) return false;
		if (m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;
		if (port <= 0) return false;
		try {
			m_socket->connect(tcp::endpoint(boost::asio::ip::address::from_string(address), port));
		}
		catch (boost::system::system_error& e) {
			for (auto& listener : m_listeners) {
				if (listener != nullptr) listener->onTCPClientError(e.code());
			}
			return false;
		}
		m_isConnecting.store(true, std::memory_order::memory_order_seq_cst);

		for (auto& listener : m_listeners) {
			if (listener != nullptr) listener->onTCPClientConnected();
		}

		return true;
	}

	bool TCPClient::connectAsync(const char* address, uint16_t port)
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (m_socket.get() == nullptr) return false;
		if (m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;
		if (port <= 0) return false;
		m_socket->async_connect(tcp::endpoint(boost::asio::ip::address::from_string(address), port),
			[this](const boost::system::error_code& e) {
			if (!e) {
				m_isConnecting.store(true, std::memory_order::memory_order_seq_cst);
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onTCPClientConnected();
				}
			}
			else {
				for (auto& listener : m_listeners) {
					if (listener != nullptr) listener->onTCPClientDisconnected(e);
				}
			}
		});

		return true;
	}

	bool TCPClient::receiveAsync()
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (m_socket.get() == nullptr) return false;
		if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return false;

		startReceiveAsync();
		return true;
	}

	bool TCPClient::disconnect()
	{
		std::shared_lock<std::shared_mutex> lock(m_mutex);
		if (m_socket.get() == nullptr) return true;
		if (!m_isConnecting.load(std::memory_order::memory_order_seq_cst)) return true;

		m_isConnecting.store(false, std::memory_order::memory_order_seq_cst);

		boost::system::error_code ec;
		m_socket->shutdown(tcp::socket::shutdown_both, ec);
		if (m_socket->is_open()) m_socket->close();

		for (auto& listener : m_listeners) {
			if (listener != nullptr) listener->onTCPClientDisconnected(boost::system::error_code());
		}

		return true;
	}

	bool TCPClient::destroy()
	{
		disconnect();
		{
			std::unique_lock<std::recursive_mutex> lock(m_sckMtx);
			m_socket.reset(nullptr);
		}
		return true;
	}

	void TCPClient::startReceiveAsync()
	{
		std::unique_lock<std::recursive_mutex> lock(m_sckMtx);
		if (m_socket.get() != nullptr) {
			m_socket->async_receive(boost::asio::buffer(m_buffer),
				[this](const boost::system::error_code& err, std::size_t transferred) {
				if (!err && m_isConnecting.load(std::memory_order::memory_order_seq_cst)) {
					for (auto& listener : m_listeners) {
						if (listener != nullptr) listener->onTCPClientReceived(err, m_buffer);
					}
					startReceiveAsync();
				}
			});
		}
	}
}
}
}