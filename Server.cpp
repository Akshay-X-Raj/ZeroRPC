#include "Server.h"
#include "Futex.h"
#include <iostream>
#include <string>
#include <cstring>

Server::Server(const std::string& shmName)
    : shm(shmName, sizeof(RpcSharedData), /*create*/true)
{
    data = reinterpret_cast<RpcSharedData*>(shm.get());
    data->state.store(0, std::memory_order_relaxed);
    std::memset(data->buffer, 0, BUFFER_SIZE);
    std::cout << "[Server] Ready. Shared memory: " << shmName << std::endl;
}

void Server::run() {
    while (true) {
        // Wait for client request
        while (data->state.load(std::memory_order_acquire) != 1) {
            // Sleep only if state matches 0
            Futex::wait(&data->state, 0);
        }

        // Read request
        std::string request(data->buffer);
        std::cout << "[Server] Received: " << request << std::endl;

        // Echo back exactly what was sent
        std::strncpy(data->buffer, request.c_str(), BUFFER_SIZE - 1);
        data->buffer[BUFFER_SIZE - 1] = '\0';

        // Publish response
        data->state.store(2, std::memory_order_release);
        Futex::wake(&data->state);
    }
}
