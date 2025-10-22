#pragma once
#include <atomic>
#include <cstddef>

constexpr size_t BUFFER_SIZE = 256;

// Single-slot RPC shared region
// state: 0=idle, 1=request ready, 2=response ready
struct RpcSharedData {
    std::atomic<int> state;
    char buffer[BUFFER_SIZE];
};
