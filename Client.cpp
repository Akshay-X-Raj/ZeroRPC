#include "Client.h"
#include "Futex.h"
#include <iostream>
#include <cstring>

Client::Client(const std::string& shmName)
    : shm(shmName, sizeof(RpcSharedData), /*create*/false)
{
    data = reinterpret_cast<RpcSharedData*>(shm.get());
}

std::string Client::call(const std::string& msg) {
    // Write request
    std::strncpy(data->buffer, msg.c_str(), BUFFER_SIZE - 1);
    data->buffer[BUFFER_SIZE - 1] = '\0';

    data->state.store(1, std::memory_order_release);
    Futex::wake(&data->state);

    // Wait for response
    while (data->state.load(std::memory_order_acquire) != 2) {
        // Sleep only if state matches 1
        Futex::wait(&data->state, 1);
    }

    std::string response(data->buffer);

    // Reset to idle
    data->state.store(0, std::memory_order_release);
    Futex::wake(&data->state);

    return response;
}

void Client::demo_send(const std::string& msg) {
    std::string resp = call(msg);
    std::cout << "[Client] Response: " << resp << std::endl;
}
