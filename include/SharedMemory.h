#pragma once
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <string>

class SharedMemory {
public:
    SharedMemory(const std::string& name, size_t size, bool create = false);
    ~SharedMemory();

    void* get() const { return mem; }
    size_t length() const { return size; }

private:
    int fd{-1};
    size_t size{0};
    void* mem{nullptr};
    std::string name;
};
