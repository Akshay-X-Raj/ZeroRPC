#include "SharedMemory.h"

SharedMemory::SharedMemory(const std::string& n, size_t s, bool create)
: size(s), name(n) {
    fd = shm_open(name.c_str(), create ? (O_CREAT | O_RDWR) : O_RDWR, 0666);
    if (fd < 0) throw std::runtime_error("shm_open failed");
    ftruncate(fd, size);
    ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) throw std::runtime_error("mmap failed");
}

SharedMemory::~SharedMemory() {
    munmap(ptr, size);
    close(fd);
}

void* SharedMemory::data() { return ptr; }
