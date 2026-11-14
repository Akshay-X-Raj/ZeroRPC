#pragma once
// SocketClient.h
// Simple Unix Domain Socket client wrapper.
// Throws std::runtime_error on connection/send/recv errors.

#include <string>

class SocketClient {
public:
    // path: filesystem path for AF_UNIX socket (e.g. "/tmp/zerorpc_sock")
    explicit SocketClient(const std::string& path = "/tmp/zerorpc_sock");
    ~SocketClient();

    // Blocking RPC call — returns server response string.
    // Throws std::runtime_error on failure.
    std::string call(const std::string& msg);

    // non-copyable
    SocketClient(const SocketClient&) = delete;
    SocketClient& operator=(const SocketClient&) = delete;

private:
    int sockfd{-1};
    std::string path_;
};
