#include "BenchmarkGUI.h"

// ImGui headers (SDL2 Renderer backend)
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

// SDL2
#include <SDL.h>

// std
#include <fstream>
#include <vector>

BenchmarkGUI::BenchmarkGUI()
    : client("/zerorpc_shm") {}

void BenchmarkGUI::sendRpc(const std::string& msg) {
    auto start = std::chrono::steady_clock::now();
    std::string response = client.call(msg);
    auto end = std::chrono::steady_clock::now();

    double latency = std::chrono::duration<double, std::micro>(end - start).count();
    last_latency = latency;
    requests_sent++;

    results.push_back({latency, end, msg, response});
}

void BenchmarkGUI::render() {
    ImGui::Begin("🚀 ZeroRPC Benchmark Dashboard");

    ImGui::Text("Requests Sent: %d", requests_sent);
    ImGui::Text("Last Latency: %.2f µs", last_latency);

    ImGui::InputText("RPC Request", inputBuffer, sizeof(inputBuffer));

    if (ImGui::Button("Send RPC")) {
        sendRpc(std::string(inputBuffer));
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear Logs")) {
        results.clear();
        requests_sent = 0;
        last_latency = 0.0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Logs to CSV")) {
        std::ofstream file("zerorpc_logs.csv");
        file << "Request,Response,Latency(us),Timestamp(ms)\n";
        for (auto& r : results) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                r.timestamp.time_since_epoch()).count();
            // naive CSV (no escaping for commas)
            file << r.request << "," << r.response << "," << r.latency_us << "," << ms << "\n";
        }
        file.close();
    }

    ImGui::Separator();

    // Plot latency with auto-scaling capped at 2000 µs
    if (!results.empty()) {
        std::vector<float> lat;
        float maxLatency = 0.0f;
        lat.reserve(results.size());
        for (auto& r : results) {
            float v = static_cast<float>(r.latency_us);
            lat.push_back(v);
            if (v > maxLatency) maxLatency = v;
        }
        float yMax = (maxLatency < 2000.0f) ? maxLatency * 1.2f : 2000.0f;
        if (yMax < 10.0f) yMax = 10.0f;

        ImGui::PlotLines("Latency (µs)", lat.data(), static_cast<int>(lat.size()),
                         0, nullptr, 0.0f, yMax, ImVec2(0, 140));
    }

    ImGui::Separator();
    ImGui::Text("Recent Logs:");
    int count = 0;
    for (auto it = results.rbegin(); it != results.rend() && count < 5; ++it, ++count) {
        ImGui::Text("[%d] Req: %s | Resp: %s | Lat: %.2f µs",
                    count, it->request.c_str(), it->response.c_str(), it->latency_us);
    }

    ImGui::End();
}

void BenchmarkGUI::run() {
    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return;
    }

    // Window + Renderer
    SDL_Window* window = SDL_CreateWindow(
        "ZeroRPC GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        900, 700, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // ImGui context + style
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Nice rounding and spacing
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 8.0f;
    style.WindowRounding = 12.0f;
    style.GrabRounding = 6.0f;
    style.ScrollbarRounding = 9.0f;
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(12, 8);

    // Backends: SDL2 + SDLRenderer
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        render();

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 26, 26, 31, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
    }

    // Shutdown
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
