#include "SharedMemory.h"

SharedMemory::SharedMemory(const std::string& name, size_t size, bool create)
    : size(size), name(name)
{
    int flags = create ? (O_CREAT | O_RDWR) : O_RDWR;
    fd = shm_open(name.c_str(), flags, 0666);
    if (fd == -1) throw std::runtime_error("shm_open failed");

    if (create) {
        if (ftruncate(fd, size) == -1) {
            close(fd);
            throw std::runtime_error("ftruncate failed");
        }
    }

    mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("mmap failed");
    }
}

SharedMemory::~SharedMemory() {
    if (mem && mem != MAP_FAILED) munmap(mem, size);
    if (fd != -1) close(fd);
}
