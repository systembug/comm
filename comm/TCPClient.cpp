//
// Created by ubuntu on 20. 6. 16..
//

#include "TCPClient.h"
#include "app/TCPClient.h"
#include "app/TCPClientListener.h"

namespace cys::comm::wrapper {
    TCPClient::TCPClient(Ctx* ctx)
        : m_client(new cys::comm::app::TCPClient(ctx->getContext()))
    {}

    TCPClient::~TCPClient()
    {
        if (m_client != nullptr) {
            delete m_client;
            m_client = nullptr;
        }
    }

    TCPClient::TCPClient(const TCPClient& client)
        : m_client(client.m_client)
    {}

    TCPClient::TCPClient(TCPClient&& client) noexcept
        : m_client(std::move(client.m_client))
    {}

    TCPClient& TCPClient::operator=(const TCPClient& client)
    {
        if (this != &client) {
            m_client = client.m_client;
        }

        return *this;
    }

    TCPClient& TCPClient::operator=(TCPClient&& client) noexcept
    {
        if (this != &client) {
            m_client = client.m_client;
        }

        return *this;
    }

    bool TCPClient::addListener(app::TCPClientListener* listener)
    {
        return m_client->addListener(listener);
    }

    bool TCPClient::deleteListener(app::TCPClientListener* listener)
    {
        return m_client->deleteListener(listener);
    }

    bool TCPClient::create()
    {
        return m_client->create();
    }

    bool TCPClient::connect(const char* address, uint16_t port)
    {
        return m_client->connect(address, port);
    }

    bool TCPClient::connect(const std::string& address, uint16_t port)
    {
        return m_client->connect(address, port);
    }

    bool TCPClient::connectAsync(const char* address, uint16_t port)
    {
        return m_client->connectAsync(address, port);
    }

    bool TCPClient::connectAsync(const std::string& address, uint16_t port)
    {
        return m_client->connectAsync(address, port);
    }

    bool TCPClient::send(const std::string& data)
    {
        return m_client->send(data);
    }

    bool TCPClient::sendAsync(const std::string& data)
    {
        return m_client->sendAsync(data);
    }

    bool TCPClient::receive(std::string& data)
    {
        return m_client->receive(data);
    }

    bool TCPClient::receiveAsync()
    {
        return m_client->receiveAsync();
    }

    bool TCPClient::disconnect()
    {
        return m_client->disconnect();
    }

    bool TCPClient::destroy()
    {
        return m_client->destroy();
    }

    bool TCPClient::isConnecting()
    {
        return m_client->isConnecting();
    }
}