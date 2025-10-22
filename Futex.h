#pragma once
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>

// Minimal futex wrapper for Linux
// state values used in ZeroRPC:
//   0 = idle
//   1 = request ready
//   2 = response ready
class Futex {
public:
    // Put thread to sleep if *addr == expected
    static int wait(std::atomic<int>* addr, int expected) {
        int* p = reinterpret_cast<int*>(addr);
        return syscall(SYS_futex, p,
                       FUTEX_WAIT, expected,
                       nullptr, nullptr, 0);
    }

    // Wake up up to 'count' threads waiting on addr
    static int wake(std::atomic<int>* addr, int count = 1) {
        int* p = reinterpret_cast<int*>(addr);
        return syscall(SYS_futex, p,
                       FUTEX_WAKE, count,
                       nullptr, nullptr, 0);
    }
};
