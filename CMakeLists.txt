cmake_minimum_required(VERSION 3.16)
project(zerorpc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Paths
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)

# Includes
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Find SDL2
find_package(SDL2 REQUIRED)
message(STATUS "Found SDL2: ${SDL2_LIBRARIES}")

# ImGui sources (core + SDL2 + SDL2Renderer backends)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

# Common sources
set(ZERORPC_COMMON
    src/SharedMemory.cpp
    src/Futex.cpp
)

# Server
add_executable(zerorpc_server
    src/main_server.cpp
    src/Server.cpp
    ${ZERORPC_COMMON}
)

# CLI Client
add_executable(zerorpc_client
    src/main_client.cpp
    src/Client.cpp
    ${ZERORPC_COMMON}
)

# GUI Client
add_executable(zerorpc_gui
    src/main_gui.cpp
    src/BenchmarkGUI.cpp
    src/Client.cpp
    ${ZERORPC_COMMON}
    ${IMGUI_SOURCES}
)

# Link SDL2
if (TARGET SDL2::SDL2)
    target_link_libraries(zerorpc_server PRIVATE SDL2::SDL2)
    target_link_libraries(zerorpc_client PRIVATE SDL2::SDL2)
    target_link_libraries(zerorpc_gui PRIVATE SDL2::SDL2)
else()
    target_include_directories(zerorpc_server PRIVATE ${SDL2_INCLUDE_DIRS})
    target_include_directories(zerorpc_client PRIVATE ${SDL2_INCLUDE_DIRS})
    target_include_directories(zerorpc_gui PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(zerorpc_server PRIVATE ${SDL2_LIBRARIES})
    target_link_libraries(zerorpc_client PRIVATE ${SDL2_LIBRARIES})
    target_link_libraries(zerorpc_gui PRIVATE ${SDL2_LIBRARIES})
endif()
