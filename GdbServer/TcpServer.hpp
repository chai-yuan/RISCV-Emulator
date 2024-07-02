#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../Common/DebugUtils.hpp"

class TcpServer {
  public:
    TcpServer() : socket_fd(0), client_fd(0) {}

    ~TcpServer() {
        if (socket_fd != 0) {
            close(socket_fd);
        }
        if (client_fd != 0) {
            close(client_fd);
        }
    }

    bool DataAvailable() {
        int count;
        ioctl(client_fd, FIONREAD, &count);
        return (bool)count;
    }

    void AcceptClient() {
        socklen_t cli_addr_size = sizeof(client_addr);
        client_fd = ::accept(socket_fd, (struct sockaddr *)&client_addr, &cli_addr_size);
        if (client_fd == -1) {
            ERROR("Accept failed");
        }
    }

    void Start(int port) {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            ERROR("socket creation failed");
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            close(socket_fd);
            ERROR("bind failed");
        }

        if (listen(socket_fd, 1) == -1) {
            close(socket_fd);
            ERROR("listen failed");
        }
    }

    void SendMsg(const char *msg) {
        if (send(client_fd, msg, strlen(msg), 0) == -1) {
            ERROR("send failed");
        }
    }

    std::string RecvBlocking(uint32_t length) {
        char buffer[MaxPacketSize];
        ssize_t bytes_received = recv(client_fd, buffer, std::min(length, MaxPacketSize), 0);
        if (bytes_received == -1) {
            ERROR("recv failed");
        }
        return std::string(buffer, bytes_received);
    }

  private:
    const uint32_t MaxPacketSize = 4096;
    int socket_fd;
    int client_fd;
    sockaddr_in server_addr;
    sockaddr_in client_addr;
};

#endif // TCPSERVER_H
