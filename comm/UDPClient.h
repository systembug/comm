#pragma once
#include <cstdint>
#include <string>
#include "Ctx.h"

namespace cys {
namespace comm {
	namespace app {
		class UDPClient;
		class UDPClientListener;
	}
namespace wrapper {
	class EndPoint {
	public:
		EndPoint(const std::string& address, uint16_t port)
			: m_address(address), m_port(port)
		{}
		~EndPoint() = default;
		EndPoint(const EndPoint& ep)
			: m_address(ep.m_address), m_port(ep.m_port)
		{}
		EndPoint(EndPoint&& ep) noexcept
			: m_address(std::move(ep.m_address)), m_port(std::move(ep.m_port))
		{}
		EndPoint& operator=(const EndPoint& ep)
		{
			if (this != &ep) {
				m_address = ep.m_address;
				m_port = ep.m_port;
			}

			return *this;
		}	
		EndPoint& operator=(EndPoint&& ep) noexcept
		{
			if (this != &ep) {
				m_address = std::move(ep.m_address);
				m_port = std::move(ep.m_port);
			}

			return *this;
		}

	private:
		std::string m_address;
		uint16_t m_port;
	};

	class UDPClient {
	public:
		UDPClient(Ctx* ctx);
		~UDPClient();
		UDPClient(const UDPClient& client);
		UDPClient(UDPClient&& client) noexcept;
		UDPClient& operator=(const UDPClient& client);
		UDPClient& operator=(UDPClient&& client) noexcept;

	private:
		app::UDPClient* m_client;

	public:
		bool addListener(app::UDPClientListener* listener);
		bool deleteListener(app::UDPClientListener* listener);

	public:
		bool create();
		bool create(uint16_t port);
		bool connect(const char* address, uint16_t port);
		bool connect(const std::string& address, uint16_t port) ;
		bool connectAsync(const char* address, uint16_t port);
		bool connectAsync(const std::string& address, uint16_t port);
		bool send(const std::string& data);
		bool sendAsync(const std::string& data);
		bool sendToBroadCast(uint16_t port, const std::string& data);
		bool sendTo(const std::string& address, uint16_t port, const std::string& data);
		bool sendToAsync(const std::string& address, uint16_t port, const std::string& data);
		bool receive(std::string& data);
		EndPoint receiveFrom(uint16_t port, std::string& data);
		bool receiveAsync();		
		bool disconnect();
		bool destroy();
		bool isConnecting();
	};
}
}
}