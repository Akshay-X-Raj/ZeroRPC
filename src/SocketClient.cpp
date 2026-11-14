#include "SocketClient.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

SocketClient::SocketClient(const std::string& path) : path_(path) {
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
        throw std::runtime_error("SocketClient: Failed to create socket");

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path_.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("SocketClient: Failed to connect to server");
}

SocketClient::~SocketClient() {
    if (sockfd >= 0)
        close(sockfd);
}

std::string SocketClient::call(const std::string& msg) {
    if (write(sockfd, msg.c_str(), msg.size()) < 0)
        throw std::runtime_error("SocketClient: Write failed");

    char buf[1024];
    ssize_t n = read(sockfd, buf, sizeof(buf) - 1);
    if (n < 0)
        throw std::runtime_error("SocketClient: Read failed");

    buf[n] = '\0';
    return std::string(buf);
}
