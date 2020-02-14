//
// Created by ubuntu on 20. 2. 7..
//

#ifndef COMM_UDPSERVER_H
#define COMM_UDPSERVER_H


#pragma once
#include <cstdint>
#include <string>
#include "Ctx.h"

namespace cys {
    namespace comm {
        namespace app {
            class UDPServer;
            class UDPServerListener;
        }
        namespace wrapper {
            class UDPServer {
            public:
                UDPServer(Ctx* ctx);
                ~UDPServer();
                UDPServer(const UDPServer& server);
                UDPServer(UDPServer&& server) noexcept;
                UDPServer& operator=(const UDPServer& server);
                UDPServer& operator=(UDPServer&& server) noexcept;

            private:
                app::UDPServer* m_server;

            public:
                bool addListener(app::UDPServerListener* listener);
                bool deleteListener(app::UDPServerListener* listener);

            public:
                bool create(uint16_t port);
                bool bind();
                bool send(const std::string& data);
                bool sendAsync(const std::string& data);
                bool sendToBroadCast(uint16_t port, const std::string& data);
                bool sendTo(const std::string& address, uint16_t port, const std::string& data);
                bool sendToAsync(const std::string& address, uint16_t port, const std::string& data);
                bool unBind();
                bool destroy();
            };
        }
    }
}


#endif //COMM_UDPSERVER_H
