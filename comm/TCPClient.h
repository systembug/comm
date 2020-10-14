#pragma once
#include "Ctx.h"
#include "EndPoint.h"

namespace cys {
    namespace comm {
        namespace app {
            class TCPClient;
            class TCPClientListener;
        }
        namespace wrapper {
            class TCPClient {
            public:
                TCPClient(Ctx* ctx);
                ~TCPClient();
                TCPClient(const TCPClient& client);
                TCPClient(TCPClient&& client) noexcept;
                TCPClient& operator=(const TCPClient& client);
                TCPClient& operator=(TCPClient&& client) noexcept;

            private:
                app::TCPClient* m_client;

            public:
                bool addListener(app::TCPClientListener* listener);
                bool deleteListener(app::TCPClientListener* listener);

            public:
                bool create();
                bool connect(const char* address, uint16_t port);
                bool connect(const std::string& address, uint16_t port) ;
                bool connectAsync(const char* address, uint16_t port);
                bool connectAsync(const std::string& address, uint16_t port);
                bool send(const std::string& data);
                bool sendAsync(const std::string& data);
                bool receive(std::string& data);
                bool receiveAsync();
                bool disconnect();
                bool destroy();
                bool isConnecting();
            };
        }
    }
}
