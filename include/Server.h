#pragma once
#include <string>
#include "SharedMemory.h"
#include "ZeroRPC.h"

class Server {
public:
    explicit Server(const std::string& shmName = "/zerorpc_shm");
    void run(); // infinite loop

private:
    SharedMemory shm;
    RpcSharedData* data{nullptr};
};
