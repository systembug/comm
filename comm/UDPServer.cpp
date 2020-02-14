//
// Created by ubuntu on 20. 2. 7..
//

#include "UDPServer.h"
#include "app/UDPServer.h"
#include "app/UDPServerListener.h"

namespace cys {
namespace comm {
namespace wrapper {
    UDPServer::UDPServer(Ctx* ctx)
        : m_server(new cys::comm::app::UDPServer(ctx->getContext()))
    {}
    UDPServer::~UDPServer()
    {
        if (m_server != nullptr) {
            delete m_server;
            m_server = nullptr;
        }
    }
    UDPServer::UDPServer(const UDPServer& server)
            : m_server(server.m_server)
    {}
    UDPServer::UDPServer(UDPServer&& server) noexcept
            : m_server(std::move(server.m_server))
    {}
    UDPServer& UDPServer::operator=(const UDPServer& server)
    {
        if (this != &server) {
            m_server = server.m_server;
        }

        return *this;
    }
    UDPServer& UDPServer::operator=(UDPServer&& server) noexcept
    {
        if (this != &server) {
            m_server = std::move(server.m_server);
        }

        return *this;
    }

    bool UDPServer::addListener(app::UDPServerListener* listener) {
        return m_server->addListener(listener);
    }

    bool UDPServer::deleteListener(app::UDPServerListener* listener) {
        return m_server->deleteListener(listener);
    }

    bool UDPServer::create(uint16_t port) {
        return m_server->create(port);
    }

    bool UDPServer::bind() {
        return m_server->bind();
    }

    bool UDPServer::send(const std::string& data) {
        return m_server->send(data);
    }

    bool UDPServer::sendAsync(const std::string& data) {
        return m_server->sendAsync(data);
    }

    bool UDPServer::sendToBroadCast(uint16_t port, const std::string& data) {
        return m_server->sendToBroadCast(port, data);
    }

    bool UDPServer::sendTo(const std::string& address, uint16_t port, const std::string& data) {
        return m_server->sendTo(address, port, data);
    }

    bool UDPServer::sendToAsync(const std::string& address, uint16_t port, const std::string& data) {
        return m_server->sendToAsync(address, port, data);
    }

    bool UDPServer::unBind() {
        return m_server->unBind();
    }

    bool UDPServer::destroy() {
        return m_server->destroy();
    }
}
}
}