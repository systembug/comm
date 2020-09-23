#include "TCPServer.h"
#include <boost/bind.hpp>
#include <iostream>

namespace cys {
namespace comm {
namespace app {
	TCPServer::TCPServer(Context* ctx)
		: m_mutex()
		, m_conMtx()
		, m_ssMtx()
		, m_context(ctx)
		, m_acceptor()
		, m_port(0)
		, m_listeners()
		, m_sessions()
		, m_buffer()
		, m_isBinding(false)
	{
	}

	TCPServer::~TCPServer()
	{
		destroy();
	}

	TCPServer::TCPServer(TCPServer&& server) noexcept
		: m_mutex()
		, m_conMtx()
		, m_ssMtx()
	{
		std::unique_lock<std::shared_mutex> lk1(server.m_mutex);
		std::unique_lock<std::recursive_mutex> lk2(server.m_conMtx);
		std::unique_lock<std::shared_mutex> lk3(server.m_ssMtx);
		m_context = std::move(server.m_context);
		m_acceptor = std::move(server.m_acceptor);
		m_port = std::move(server.m_port);
		m_listeners = std::move(server.m_listeners);
		m_sessions = std::move(server.m_sessions);
		m_buffer = std::move(server.m_buffer);
		m_isBinding = server.m_isBinding.load(std::memory_order::memory_order_seq_cst);
	}

	TCPServer& TCPServer::operator=(TCPServer&& server) noexcept
	{
		if (this != &server) {
			std::unique_lock<std::shared_mutex> lk1(m_mutex, std::defer_lock);
			std::unique_lock<std::shared_mutex> lk2(server.m_mutex, std::defer_lock);
			std::unique_lock<std::recursive_mutex> lk3(m_conMtx, std::defer_lock);
			std::unique_lock<std::recursive_mutex> lk4(server.m_conMtx, std::defer_lock);
			std::unique_lock<std::shared_mutex> lk5(m_ssMtx, std::defer_lock);
			std::unique_lock<std::shared_mutex> lk6(server.m_ssMtx, std::defer_lock);
			std::lock(lk1, lk2, lk3, lk4, lk5, lk6);
			m_context = std::move(server.m_context);
			m_acceptor = std::move(server.m_acceptor);
			m_port = std::move(server.m_port);
			m_listeners = std::move(server.m_listeners);
			m_sessions = std::move(server.m_sessions);
			m_buffer = std::move(server.m_buffer);
			m_isBinding = server.m_isBinding.load(std::memory_order::memory_order_seq_cst);
		}

		return *this;
	}

	bool TCPServer::create(uint16_t port)
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (m_acceptor.get() != nullptr) return false;
		m_port = port;
		
		return true;
	}

	bool TCPServer::bind()
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		// if (m_acceptor.get() == nullptr) return false;
		if (m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;

		try {
			m_acceptor = std::make_unique<tcp::acceptor>(m_context->getContext(),
				tcp::endpoint(tcp::v4(), m_port));
			m_acceptor->set_option(tcp::acceptor::reuse_address(true));
		}
		catch (boost::system::system_error & e) {
			std::cout << "Socket Create Error: " << e.what() << std::endl;
			// for (auto& listener : m_listeners) {
			//	if (listener != nullptr) listener->onTCPServerError(e.code());
			// }
			return false;
		}
		
		// std::cout << "bind" << std::endl;
		
		if (!m_acceptor->is_open()) {
			std::cout << "Acceptor is reopen!" << std::endl;
			tcp::endpoint ep(tcp::v4(), m_port);
			m_acceptor->open(ep.protocol());
			m_acceptor->set_option(tcp::acceptor::reuse_address(true));
		}

		startAcceptAsync();
		m_isBinding.store(true, std::memory_order::memory_order_seq_cst);
		return true;
	}

	bool TCPServer::unBind()
	{
		std::unique_lock<std::shared_mutex> lock(m_mutex);
		if (m_acceptor == nullptr) return false;
		if (!m_isBinding.load(std::memory_order::memory_order_seq_cst)) return false;
		
		// std::cout << "unBind" << std::endl;
		
		clearSession();
		m_isBinding.store(false, std::memory_order::memory_order_seq_cst);
		{
			std::unique_lock<std::recursive_mutex> lock(m_conMtx);
			boost::system::error_code ec;
			m_acceptor->cancel(ec);
			// std::cout << ec.message() << std::endl;
			if (m_acceptor->is_open()) m_acceptor->close();
		}
		
		m_acceptor.reset(nullptr);

		return true;
	}

	bool TCPServer::destroy()
	{
		unBind();
		{
			// std::unique_lock<std::recursive_mutex> lock(m_conMtx);
			// m_acceptor.reset(nullptr);
		}

		return true;
	}

	void TCPServer::onTCPSessionBind(std::size_t channel)
	{
		for (auto& listener : m_listeners)
			if (listener != nullptr) listener->onTCPServerBinded(channel);
	}

	void TCPServer::onTCPSessionUnBind(std::size_t channel)
	{
		for (auto& listener : m_listeners)
			if (listener != nullptr) listener->onTCPServerUnBinded(channel);
	}

	void TCPServer::onTCPSessionSent(std::size_t channel, const boost::system::error_code& e)
	{
		for (auto& listener : m_listeners)
			if (listener != nullptr) listener->onTCPServerSent(channel, e);
	}

	void TCPServer::onTCPSessionReceived(std::size_t channel, const boost::system::error_code& e, const std::array<uint8_t, MAX_BUFFER_NUM>& data)
	{
		for (auto& listener : m_listeners)
			if (listener != nullptr) listener->onTCPServerReceived(channel, e, data);
	}

	std::shared_ptr<TCPSession> TCPServer::createSession() {
		auto size = m_sessions.size();
		return std::make_shared<TCPSession>(m_context, size);
		// session->addListener(this);
		// m_sessions.emplace(size, std::move(session));
		// return size;
	}

	std::shared_ptr<TCPSession>& TCPServer::getSession(std::size_t channel) {
		// std::shared_lock<std::shared_mutex> lock(m_ssMtx);
		return m_sessions[channel];
	}

	bool TCPServer::deleteSession(std::size_t channel) {
		if (m_sessions.find(channel) == m_sessions.end()) return false;
		else {
			m_sessions[channel]->stop();
			m_sessions[channel]->destroy();
			m_sessions[channel]->deleteListener(this);
			m_sessions.erase(channel);
			return true;
		}
	}

	bool TCPServer::findSession(std::size_t channel) {
		// std::shared_lock<std::shared_mutex> lock(m_ssMtx);
		if (m_sessions.find(channel) != m_sessions.end()) return true;
		else return false;
	}

	void TCPServer::clearSession() {
		// std::unique_lock<std::shared_mutex> lock(m_ssMtx);
		for (auto& session : m_sessions) {
			session.second->stop();
			session.second->destroy();
		}
		m_sessions.clear();
	}

	void TCPServer::startAcceptAsync()
	{
		// std::unique_lock<std::recursive_mutex> lock(m_conMtx);
		if (m_acceptor != nullptr) {
			auto session = createSession();
			session->create();
			m_acceptor->async_accept(session->getSocket(), 
				boost::bind(&TCPServer::handleAcceptAsync, this, session,
				boost::asio::placeholders::error));
		}
	}

	void TCPServer::handleAcceptAsync(std::shared_ptr<TCPSession> session,
		const boost::system::error_code& error)
	{
		if (!error) {
			session->addListener(this);
			auto channel = m_sessions.size();
			m_sessions.emplace(m_sessions.size(), std::move(session));

			for (auto& listener : m_listeners) {
				if (listener != nullptr) listener->onTCPServerBinded(channel);
			}

			getSession(channel)->start();
			startAcceptAsync();
		}
		else {
			std::cout << error.message() << std::endl;
			std::unique_lock<std::shared_mutex> lock(m_ssMtx);
			session->destroy();
			session->deleteListener(this);
			/*
			if (findSession(channel)) {
				getSession(channel)->stop();
				getSession(channel)->destroy();
				deleteSession(channel);
			}
			*/
		}

		// if (m_isBinding.load(std::memory_order_seq_cst)) startAcceptAsync();
	}
}
}
}
