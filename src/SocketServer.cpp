#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

int main() {
    const char* sock_path = "/tmp/zerorpc_sock";
    unlink(sock_path);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) throw std::runtime_error("Unix socket creation failed");

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Unix socket bind failed");
    listen(server_fd, 1);
    std::cout << "[Unix Socket Server] Listening on " << sock_path << std::endl;

    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) throw std::runtime_error("Unix socket accept failed");

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
    unlink(sock_path);
    return 0;
}
