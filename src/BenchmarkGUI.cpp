#include "Client.h"
#include "RDTSCTimer.h"
#include <SDL2/SDL.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <thread>
#include <vector>
#include <numeric>
#include <algorithm>
#include <atomic>
#include <random>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

// ------------------------------
// Struct: IPC Benchmark Results
// ------------------------------
struct IPCResult {
    std::string name;
    std::vector<double> latencies;
    double throughput = 0.0;
    double p50 = 0, p90 = 0, p99 = 0, p999 = 0, max = 0;
};

// ------------------------------
// Utility: Percentile Computation
// ------------------------------
static double percentile(std::vector<double>& v, double p) {
    if (v.empty()) return 0;
    size_t idx = static_cast<size_t>(p * v.size());
    return v[std::min(idx, v.size() - 1)];
}

static void compute_stats(IPCResult& r) {
    if (r.latencies.empty()) return;
    std::sort(r.latencies.begin(), r.latencies.end());
    r.p50 = percentile(r.latencies, 0.50);
    r.p90 = percentile(r.latencies, 0.90);
    r.p99 = percentile(r.latencies, 0.99);
    r.p999 = percentile(r.latencies, 0.999);
    r.max = r.latencies.back();
}

// ------------------------------
// Logging Utilities
// ------------------------------
static ImGuiTextBuffer logBuffer;
static bool autoScroll = true;

static void log_line(const std::string& msg) {
    logBuffer.appendf("%s\n", msg.c_str());
    std::cout << msg << std::endl;
}

// ------------------------------
// Benchmark Logic (Simulated)
// ------------------------------
static void benchmark_ipc(IPCResult& r, int num_requests = 10000) {
    double tsc_ghz = calibrate_tsc_ghz();
    r.latencies.clear();
    log_line("[Start] Benchmark: " + r.name);

    for (int i = 0; i < num_requests; ++i) {
        uint64_t start = rdtscp();

        if (r.name == "ZeroRPC") {
            // Simulate ultra-fast in-memory loop
            for (volatile int j = 0; j < 200; ++j) {}
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds(
                (rand() % 10) + (r.name == "Unix Socket" ? 40 :
                                (r.name == "TCP" ? 65 : 100))));
        }

        uint64_t end = rdtscp();
        r.latencies.push_back(cycles_to_us(start, end, tsc_ghz));

        if (i % 2000 == 0 && i > 0) {
            std::ostringstream oss;
            oss << "  · Progress: " << i << "/" << num_requests << " requests (" << r.name << ")";
            log_line(oss.str());
        }
    }

    compute_stats(r);
    double total = std::accumulate(r.latencies.begin(), r.latencies.end(), 0.0);
    r.throughput = (num_requests / (total / 1e6));

    std::ostringstream summary;
    summary << "[Done] " << r.name << " Results -> "
            << "p50=" << r.p50 << " µs, "
            << "p90=" << r.p90 << " µs, "
            << "p99=" << r.p99 << " µs, "
            << "max=" << r.max << " µs, "
            << "throughput=" << r.throughput << " req/s";
    log_line(summary.str());
    log_line("-----------------------------------------------------------");
}

// ------------------------------
// Save Logs to File
// ------------------------------
static void save_logs_to_file() {
    std::filesystem::create_directory("logs");

    std::time_t now = std::time(nullptr);
    std::tm* tm_info = std::localtime(&now);
    std::ostringstream fname;
    fname << "logs/benchmark_log_" << std::put_time(tm_info, "%Y%m%d_%H%M%S") << ".txt";

    std::ofstream out(fname.str());
    out << logBuffer.begin();
    out.close();

    log_line("[Save] Logs saved to: " + fname.str());
}

// ------------------------------
// Main GUI Application
// ------------------------------
int main(int, char**) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL2\n";
        return -1;
    }

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    int screenWidth = dm.w;
    int screenHeight = dm.h;

    SDL_Window* window = SDL_CreateWindow("ZeroRPC Benchmark Dashboard",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    const char* font_path = "/usr/share/fonts/TTF/DejaVuSans.ttf";
    if (std::filesystem::exists(font_path))
        io.Fonts->AddFontFromFileTTF(font_path, 18.0f);
    io.Fonts->Build();

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 6.0f;
    style.WindowRounding = 10.0f;
    style.GrabRounding = 5.0f;
    style.FramePadding = ImVec2(6, 4);

    bool running = true;
    SDL_Event e;
    std::vector<IPCResult> results = {
        {"ZeroRPC", {}}, {"Unix Socket", {}}, {"TCP", {}}, {"Pipe", {}}
    };

    int selected_ipc = -1;
    std::atomic<bool> benchmarking(false);

    while (running) {
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) running = false;

            // Fullscreen toggle
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11) {
                Uint32 flags = SDL_GetWindowFlags(window);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                    SDL_SetWindowFullscreen(window, 0);
                else
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)screenWidth, (float)screenHeight));
        ImGui::Begin("ZeroRPC Performance Dashboard", nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

        ImGui::TextWrapped("[Dashboard] Compare IPC Mechanisms (ZeroRPC vs Sockets vs TCP vs Pipes)");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Select IPC Method:");
        ImGui::Spacing();

        for (int i = 0; i < (int)results.size(); ++i) {
            ImGui::PushStyleColor(ImGuiCol_Button,
                (selected_ipc == i) ? ImVec4(0.2f, 0.5f, 0.9f, 1.0f)
                                    : ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
            if (ImGui::Button(results[i].name.c_str(), ImVec2(220, 40))) {
                selected_ipc = i;
                benchmarking = true;
                std::thread([&, i]() {
                    benchmark_ipc(results[i]);
                    benchmarking = false;
                }).detach();
            }
            ImGui::PopStyleColor();
            if (i < (int)results.size() - 1)
                ImGui::SameLine();
        }

        ImGui::Spacing();
        ImGui::Separator();

        if (benchmarking)
            ImGui::TextColored(ImVec4(1, 0.7, 0, 1), "[Running] Benchmarking... please wait");
        else if (selected_ipc >= 0) {
            IPCResult& r = results[selected_ipc];
            ImGui::Text("Results for: %s", r.name.c_str());
            ImGui::Text("Throughput: %.1f req/s", r.throughput);
            ImGui::Text("p50: %.2f µs | p90: %.2f µs | p99: %.2f µs | max: %.2f µs",
                        r.p50, r.p90, r.p99, r.max);
        }

        ImGui::Separator();
        ImGui::Text("Latency Comparison (p99 µs):");

        std::vector<float> p99_vals, throughput_vals;
        for (auto& r : results) {
            p99_vals.push_back(r.p99);
            throughput_vals.push_back(r.throughput / 1000.0f);
        }

        if (ImGui::BeginTable("metrics", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("IPC Method");
            ImGui::TableSetupColumn("p99 Latency (µs)");
            ImGui::TableHeadersRow();
            for (auto& r : results) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", r.name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f", r.p99);
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::PlotHistogram("p99 Latency (µs)", p99_vals.data(), p99_vals.size(), 0, nullptr, 0, 250, ImVec2(0, 130));
        ImGui::PlotHistogram("Throughput (K req/s)", throughput_vals.data(), throughput_vals.size(), 0, nullptr, 0, 1000, ImVec2(0, 130));

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("[Logs] Benchmark Logs");
        if (ImGui::Button("[Save] Save Logs", ImVec2(140, 35))) {
            save_logs_to_file();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &autoScroll);

        ImGui::BeginChild("LogRegion", ImVec2(0, 220), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(logBuffer.begin());
        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();

        ImGui::End();

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
