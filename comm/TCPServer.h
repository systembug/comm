#pragma once
#include "Ctx.h"
#include "EndPoint.h"

namespace cys {
    namespace comm {
        namespace app {
            class TCPServer;
            class TCPServerListener;
        }
        namespace wrapper {
            class TCPServer {
            public:
                TCPServer(Ctx* ctx);
                ~TCPServer();
                TCPServer(const TCPServer& server);
                TCPServer(TCPServer&& server) noexcept;
                TCPServer& operator=(const TCPServer& server);
                TCPServer& operator=(TCPServer&& server) noexcept;

            private:
                app::TCPServer* m_server;

            public:
                bool addListener(app::TCPServerListener* listener);
                bool deleteListener(app::TCPServerListener* listener);

            public:
                bool create(uint16_t port);
                bool bind();
                bool send(std::size_t channel, const std::string& data);
                bool sendAsync(std::size_t channel, const std::string& data);
                bool unBind();
                bool destroy();
            };
        }
    }
}
