#pragma once
#include <cstdint>

// Read Time Stamp Counter + Processor ID (fast, precise)
inline uint64_t rdtscp() {
    unsigned int aux;
    return __builtin_ia32_rdtscp(&aux);
}

// Declarations only — implemented in RDTSCTimer.cpp
double cycles_to_us(uint64_t start, uint64_t end, double ghz);
double calibrate_tsc_ghz();
