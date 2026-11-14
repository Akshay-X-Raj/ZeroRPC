#include "Server.h"
#include "SharedMemory.h"
#include "Futex.h"
#include <iostream>
#include <atomic>
#include <cstring>

struct RpcBuffer {
    std::atomic<int> flag;
    char message[256];
};

Server::Server(const std::string& shm_name) {
    SharedMemory shm(shm_name, sizeof(RpcBuffer), true);
    shm_ptr = shm.data();
    RpcBuffer* buf = (RpcBuffer*)shm_ptr;
    buf->flag.store(0);
}

void Server::run() {
    RpcBuffer* buf = (RpcBuffer*)shm_ptr;
    while (true) {
        if (buf->flag.load() == 1) {
            std::cout << "[Server] Received: " << buf->message << std::endl;
            strcat(buf->message, " [ack]");
            buf->flag.store(2);
            futex_wake(&buf->flag, 1);
        } else {
            futex_wait(&buf->flag, 0);
        }
    }
}
