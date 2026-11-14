#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

int main() {
    const char* pipe_path = "/tmp/zerorpc_pipe";
    unlink(pipe_path);
    mkfifo(pipe_path, 0666);

    std::cout << "[Pipe Server] Waiting for clients on " << pipe_path << std::endl;
    char buf[256];

    while (true) {
        int read_fd = open(pipe_path, O_RDONLY);
        if (read_fd < 0) throw std::runtime_error("Pipe open read failed");

        ssize_t n = read(read_fd, buf, sizeof(buf) - 1);
        close(read_fd);
        if (n <= 0) continue;

        buf[n] = '\0';
        std::string response = std::string(buf) + " [ack]";

        int write_fd = open(pipe_path, O_WRONLY);
        if (write_fd >= 0) {
            write(write_fd, response.c_str(), response.size());
            close(write_fd);
        }
    }

    unlink(pipe_path);
    return 0;
}
