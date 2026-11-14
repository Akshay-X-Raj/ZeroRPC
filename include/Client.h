#pragma once
#include <string>

class Client {
public:
    Client(const std::string& shm_name);
    std::string call(const std::string& msg);
private:
    void* shm_ptr;
};
