#pragma once
#include <string>
#include "SharedMemory.h"
#include "ZeroRPC.h"

class Client {
public:
    explicit Client(const std::string& shmName = "/zerorpc_shm");
    std::string call(const std::string& msg);   // blocking call
    void demo_send(const std::string& msg);     // prints to stdout

private:
    SharedMemory shm;
    RpcSharedData* data{nullptr};
};
