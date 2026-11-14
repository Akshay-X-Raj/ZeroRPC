#pragma once
#include <cstdint>
#include <x86intrin.h>
#include <chrono>
#include <thread>   // ✅ Fix: required for std::this_thread::sleep_for

// Simple TSC-based timing utilities
inline double calibrate_tsc_ghz() {
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t t0 = __rdtsc();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    uint64_t t1 = __rdtsc();
    auto end = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(end - start).count();
    return (t1 - t0) / (elapsed * 1e9);
}

inline uint64_t rdtscp() {
    unsigned aux;
    return __rdtscp(&aux);
}

inline double cycles_to_us(uint64_t start, uint64_t end, double ghz) {
    return static_cast<double>(end - start) / (ghz * 1000.0);
}
