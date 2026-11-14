#include "RDTSCTimer.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono;

// Convert cycle delta to microseconds
double cycles_to_us(uint64_t start, uint64_t end, double ghz) {
    return static_cast<double>(end - start) / (ghz * 1000.0);
}

// Measure TSC frequency in GHz
double calibrate_tsc_ghz() {
    uint64_t t0 = rdtscp();  // ✅ uses inline definition from RDTSCTimer.h
    auto start = steady_clock::now();
    std::this_thread::sleep_for(milliseconds(100));
    uint64_t t1 = rdtscp();
    auto end = steady_clock::now();

    double elapsed_s = duration<double>(end - start).count();
    uint64_t delta = t1 - t0;
    double ghz = (delta / elapsed_s) / 1e9;

    std::cout << "[Calibration] TSC frequency ≈ " << ghz << " GHz\n";
    return ghz;
}
