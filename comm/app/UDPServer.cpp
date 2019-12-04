#include "UDPServer.h"

namespace cys {
namespace comm {
namespace app {
	UDPServer::UDPServer(Context* ctx)
		: m_mutex()
		, m_sckMtx()
		, m_context(ctx)
		, m_socket()
		, m_port(0)
		, m_listeners()
		, m_buffer()
		, m_isBinding(false)
	{
	}

	UDPServer::~UDPServer()
	{
		destroy();
	}

	UDPServer::UDPServer(UDPServer&& server) noexcept
		: m_mutex()
		, m_sckMtx()
	{
		std::unique_lock<std::shared_mutex> lock(server.m_mutex);
		std::unique_lock<std::recursive_mutex> lk(server.m_sckMtx);
		m_context = std::move(server.m_context);
		m_socket = std::move(server.m_socket);
		m_port = std::move(server.m_port);
		m_listeners = std::move(server.m_listeners);
		m_buffer = std::move(server.m_buffer);
		m_isBinding = server.m_isBinding.load(std::memory_order::memory_order_seq_cst);
	}

	UDPServer& UDPServer::operator=(UDPServer&& server) noexcept
	{
		if (this != &server) {
			std::unique_lock<std::shared_mutex> lk1(m_mutex, std::defer_lock);
			std::unique_lock<std::shared_mutex> lk2(server.m_mutex, std::defer_lock);
			std::unique_lock<std::recursive_mutex> lk3(m_sckMtx, std::defer_lock);
			std::unique_lock<std::recursive_mutex> lk4(server.m_sckMtx, std::defer_lock);
			std::lock(lk1, lk2, lk3, lk4);
			m_context = std::move(server.m_context);
			m_socket = std::move(server.m_socket);
			m_port = std::move(server.m_port);
			m_listeners = std::move(server.m_listeners);
			m_buffer = std::move(server.m_buffer);
			m_isBinding = server.m_isBinding.load(std::memory_order::memory_order_seq_cst);
		}

		return *this;
	}

	bool UDPServer::create(uint16_t port) 
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (m_socket.get() != nullptr) return false;
		m_port = port;
		m_socket = std::make_unique<udp::socket>(m_context->getContext(), udp::endpoint(udp::v4(), port));

		return true;
	}

	bool UDPServer::bind()
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (m_socket.get() == nullptr) return false;
		if (m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;

		if (startReceiveAsync()) {
			for (auto& listener : m_listeners) {
				if (listener != nullptr) listener->onUDPServerBinded();
			}
		}
		else {
			for (auto& listener : m_listeners) {
				if (listener != nullptr) listener->onUDPServerUnBinded();
			}
			return false;
		}
				
		m_isBinding.store(true, std::memory_order::memory_order_seq_cst);
		return true;
	}

	bool UDPServer::unBind()
	{	
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (m_socket.get() == nullptr) return false;
		if (!m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;
		
		boost::system::error_code ec;
		m_socket->shutdown(udp::socket::shutdown_both, ec);
		m_socket->close(); 
		
		for (auto& listener : m_listeners) {
			if (listener != nullptr) listener->onUDPServerUnBinded();
		}
		
		return true;
	}

	bool UDPServer::destroy()
	{
		unBind();
		{
			std::unique_lock<std::recursive_mutex> lock(m_sckMtx);
			m_socket.reset(nullptr);
		}

		return true;
	}

	bool UDPServer::startReceiveAsync() 
	{
		std::unique_lock<std::recursive_mutex> lock(m_sckMtx);
		if (m_socket.get() != nullptr) {
			m_socket->async_receive(boost::asio::buffer(m_buffer),
				[this](const boost::system::error_code& err, std::size_t transferred) {
				if (!err && m_isBinding.load(std::memory_order::memory_order_seq_cst)) {
					for (auto& listener : m_listeners) {
						if (listener != nullptr) listener->onUDPServerReceived(err, m_buffer);
					}

					startReceiveAsync();
				}
			});
		}

		return true;
	}
}
}
}
