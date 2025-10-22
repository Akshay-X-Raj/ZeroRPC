#pragma once
#include <vector>
#include <string>
#include <chrono>
#include "Client.h"

struct BenchmarkResult {
    double latency_us;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    std::string request;
    std::string response;
};

class BenchmarkGUI {
public:
    BenchmarkGUI();
    void run();

private:
    void render();
    void sendRpc(const std::string& msg);

    Client client;
    std::vector<BenchmarkResult> results;
    double last_latency = 0.0;
    int requests_sent = 0;
    char inputBuffer[256] = "ping";
};
