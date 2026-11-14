#pragma once
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

class SharedMemory {
public:
    SharedMemory(const std::string& name, size_t size, bool create);
    ~SharedMemory();
    void* data();
private:
    int fd;
    void* ptr;
    size_t size;
    std::string name;
};
