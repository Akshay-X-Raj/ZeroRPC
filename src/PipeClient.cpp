#include "PipeClient.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

PipeClient::PipeClient(const std::string& path) : path_(path) {}

PipeClient::~PipeClient() = default;

std::string PipeClient::call(const std::string& msg) {
    // Write
    int fd_w = open(path_.c_str(), O_WRONLY);
    if (fd_w < 0)
        throw std::runtime_error("PipeClient: Failed to open FIFO for write");

    if (write(fd_w, msg.c_str(), msg.size()) < 0) {
        close(fd_w);
        throw std::runtime_error("PipeClient: Write failed");
    }
    close(fd_w);

    // Read
    int fd_r = open(path_.c_str(), O_RDONLY);
    if (fd_r < 0)
        throw std::runtime_error("PipeClient: Failed to open FIFO for read");

    char buf[1024];
    ssize_t n = read(fd_r, buf, sizeof(buf) - 1);
    if (n < 0) {
        close(fd_r);
        throw std::runtime_error("PipeClient: Read failed");
    }

    buf[n] = '\0';
    close(fd_r);
    return std::string(buf);
}
