#pragma once
// PipeClient.h
// Minimal named pipe (FIFO) client wrapper.
// The server must create/open the FIFO and echo back.
// This client uses the write-then-read pattern (open O_WRONLY, write, close; open O_RDONLY, read, close).

#include <string>

class PipeClient {
public:
    // path: FIFO path (e.g. "/tmp/zerorpc_pipe")
    explicit PipeClient(const std::string& path = "/tmp/zerorpc_pipe");
    ~PipeClient();

    // Blocking RPC call — returns server response string.
    // Throws std::runtime_error on error.
    std::string call(const std::string& msg);

    // non-copyable
    PipeClient(const PipeClient&) = delete;
    PipeClient& operator=(const PipeClient&) = delete;

private:
    std::string path_;
};
