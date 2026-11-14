cmake_minimum_required(VERSION 3.10)
project(ZeroRPC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

# ImGui + SDL2 dependencies
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Include paths
include_directories(
    include
    third_party/imgui
    third_party/imgui/backends
)

# Source files shared across components
set(SHARED_SOURCES
    src/Futex.cpp
    src/SharedMemory.cpp
    src/Benchmark.cpp
    src/RDTSCTimer.cpp
)

# ========== ZeroRPC Server ==========
add_executable(zerorpc_server
    src/main_server.cpp
    src/Server.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(zerorpc_server pthread)

# ========== ZeroRPC Client ==========
add_executable(zerorpc_client
    src/main_client.cpp
    src/Client.cpp
    src/SocketClient.cpp
    src/PipeClient.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(zerorpc_client pthread)

# ========== Unix Domain Socket Server ==========
add_executable(zerorpc_unix_socket_server
    src/SocketServer.cpp
)
target_link_libraries(zerorpc_unix_socket_server pthread)

# ========== TCP Server ==========
add_executable(zerorpc_tcp_server
    src/TcpServer.cpp
)
target_link_libraries(zerorpc_tcp_server pthread)

# ========== Pipe Server ==========
add_executable(zerorpc_pipe_server
    src/PipeServer.cpp
)
target_link_libraries(zerorpc_pipe_server pthread)

# ========== GUI Benchmark ==========
add_executable(zerorpc_gui
    src/BenchmarkGUI.cpp   # has main()
    src/Client.cpp
    src/Futex.cpp
    src/SharedMemory.cpp
    src/Benchmark.cpp
    src/RDTSCTimer.cpp
    src/SocketClient.cpp
    src/PipeClient.cpp
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/backends/imgui_impl_sdl2.cpp
    third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_link_libraries(zerorpc_gui
    ${SDL2_LIBRARIES}
    SDL2::SDL2
    SDL2::SDL2main
    pthread
)
