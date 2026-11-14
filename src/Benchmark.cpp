#include "Benchmark.h"
#include <vector>
#include <string>
#include <fstream>
#include <numeric>
#include <stdexcept>
#include <algorithm>

// Placeholder functions so the file compiles cleanly.
// If you don’t use these from anywhere, you can leave them minimal.

struct RawRecord {
    double latency;
    double throughput;
};

double percentile_ms(std::vector<double>& v, double p) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    size_t idx = static_cast<size_t>(p * v.size());
    return v[std::min(idx, v.size() - 1)];
}

void write_raw_csv(const std::string& path, const std::vector<RawRecord>& rows) {
    std::ofstream out(path);
    if (!out.is_open()) throw std::runtime_error("Failed to open file: " + path);
    out << "latency_ms,throughput\n";
    for (const auto& r : rows)
        out << r.latency << "," << r.throughput << "\n";
}
