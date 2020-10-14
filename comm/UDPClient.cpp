#include "UDPClient.h"
#include "app/UDPClient.h"
#include "app/UDPClientListener.h"
#include "Context.h"

namespace cys {
namespace comm {
namespace wrapper {
	UDPClient::UDPClient(Ctx* ctx)
		: m_client(new cys::comm::app::UDPClient(ctx->getContext()))
	{}
	UDPClient::~UDPClient()
	{
		if (m_client != nullptr) {
			delete m_client;
			m_client = nullptr;
		}
	}	
	UDPClient::UDPClient(const UDPClient& client)
		: m_client(client.m_client)
	{}
	UDPClient::UDPClient(UDPClient&& client) noexcept
		: m_client(std::move(client.m_client))
	{}
	UDPClient& UDPClient::operator=(const UDPClient& client)
	{
		if (this != &client) {
			m_client = client.m_client;
		}

		return *this;
	}
	UDPClient& UDPClient::operator=(UDPClient&& client) noexcept
	{
		if (this != &client) {
			m_client = std::move(client.m_client);
		}

		return *this;
	}

	bool UDPClient::addListener(app::UDPClientListener* listener) {
		return m_client->addListener(listener);
	}

	bool UDPClient::deleteListener(app::UDPClientListener* listener) {
		return m_client->deleteListener(listener);
	}	

	bool UDPClient::create() {
		return m_client->create();
	}

	bool UDPClient::connect(const char* address, uint16_t port) {
		return m_client->connect(address, port);
	}

	bool UDPClient::connect(const std::string& address, uint16_t port) {
		return m_client->connect(address, port);
	}

	bool UDPClient::connectAsync(const char* address, uint16_t port) {
		return m_client->connectAsync(address, port);
	}

	bool UDPClient::connectAsync(const std::string& address, uint16_t port) {
		return m_client->connectAsync(address, port);
	}

	bool UDPClient::send(const std::string& data) {
		return m_client->send(data);
	}

	bool UDPClient::sendAsync(const std::string& data) {
		return m_client->sendAsync(data);
	}

	bool UDPClient::sendToBroadCast(uint16_t port, const std::string& data) {
		return m_client->sendToBroadCast(port, data);
	}

	bool UDPClient::sendTo(const std::string& address, uint16_t port, const std::string& data) {
		return m_client->sendTo(address, port, data);
	}

	bool UDPClient::sendToAsync(const std::string& address, uint16_t port, const std::string& data) {
		return m_client->sendToAsync(address, port, data);
	}

	bool UDPClient::receive(std::string& data) {
		return m_client->receive(data);
	}

	EndPoint UDPClient::receiveFrom(uint16_t port, std::string& data) {
		auto rev = m_client->receiveFrom(port, data);
		return EndPoint(rev.address().to_string(), rev.port());
	}

	bool UDPClient::receiveAsync() {
		return m_client->receiveAsync();
	}		

	bool UDPClient::disconnect() {
		return m_client->disconnect();
	}

	bool UDPClient::destroy() {
		return m_client->destroy();
	}

	bool UDPClient::isConnecting() {
		return m_client->isConnecting();
	}
}
}
}