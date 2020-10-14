//
// Created by ubuntu on 20. 6. 16..
//

#include "TCPServer.h"
#include "app/TCPServer.h"
#include "app/TCPServerListener.h"

namespace cys::comm::wrapper {
    TCPServer::TCPServer(Ctx *ctx)
        : m_server(new cys::comm::app::TCPServer(ctx->getContext()))
    {}

    TCPServer::~TCPServer()
    {
        if (m_server != nullptr) {
            delete m_server;
            m_server = nullptr;
        }
    }

    TCPServer::TCPServer(const TCPServer &server)
        : m_server(server.m_server)
    {}

    TCPServer::TCPServer(TCPServer &&server) noexcept
        : m_server(std::move(server.m_server))
    {}

    TCPServer& TCPServer::operator=(const TCPServer& server)
    {
        if (this != &server) {
            m_server = server.m_server;
        }

        return *this;
    }

    TCPServer& TCPServer::operator=(TCPServer&& server) noexcept
    {
        if (this != &server) {
            m_server = std::move(server.m_server);
        }

        return *this;
    }

    bool TCPServer::addListener(app::TCPServerListener *listener)
    {
        return m_server->addListener(listener);
    }

    bool TCPServer::deleteListener(app::TCPServerListener *listener)
    {
        return m_server->deleteListener(listener);
    }

    bool TCPServer::create(uint16_t port)
    {
        return m_server->create(port);
    }

    bool TCPServer::bind()
    {
        return m_server->bind();
    }

    bool TCPServer::send(std::size_t channel, const std::string &data)
    {
        return m_server->send(channel, data);
    }

    bool TCPServer::sendAsync(std::size_t channel, const std::string &data)
    {
        return m_server->sendAsync(channel, data);
    }

    bool TCPServer::unBind()
    {
        return m_server->unBind();
    }

    bool TCPServer::destroy()
    {
        return m_server->destroy();
    }
}
