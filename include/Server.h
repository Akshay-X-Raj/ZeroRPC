#pragma once
#include <string>

class Server {
public:
    Server(const std::string& shm_name);
    void run();
private:
    void* shm_ptr;
};