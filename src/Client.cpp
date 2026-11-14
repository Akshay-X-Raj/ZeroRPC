#include "Client.h"
#include "SharedMemory.h"
#include "Futex.h"
#include <cstring>
#include <atomic>

struct RpcBuffer {
    std::atomic<int> flag;
    char message[256];
};

Client::Client(const std::string& shm_name) {
    SharedMemory shm(shm_name, sizeof(RpcBuffer), false);
    shm_ptr = shm.data();
}

std::string Client::call(const std::string& msg) {
    RpcBuffer* buf = (RpcBuffer*)shm_ptr;
    std::strncpy(buf->message, msg.c_str(), sizeof(buf->message));
    buf->flag.store(1);
    futex_wake(&buf->flag, 1);
    while (buf->flag.load() != 2)
        futex_wait(&buf->flag, 1);
    buf->flag.store(0);
    return std::string(buf->message);
}
