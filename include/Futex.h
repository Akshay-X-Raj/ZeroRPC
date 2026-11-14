#pragma once
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>

inline int futex_wait(std::atomic<int>* addr, int expected) {
    return syscall(SYS_futex, addr, FUTEX_WAIT, expected, nullptr, nullptr, 0);
}

inline int futex_wake(std::atomic<int>* addr, int count) {
    return syscall(SYS_futex, addr, FUTEX_WAKE, count, nullptr, nullptr, 0);
}
