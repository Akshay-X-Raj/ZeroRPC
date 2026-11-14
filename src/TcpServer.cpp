#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) throw std::runtime_error("TCP socket creation failed");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5555);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("TCP bind failed");
    listen(server_fd, 1);
    std::cout << "[TCP Server] Listening on 127.0.0.1:5555" << std::endl;

    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) throw std::runtime_error("TCP accept failed");

    char buf[256];
    while (true) {
        ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        std::string response = std::string(buf) + " [ack]";
        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
