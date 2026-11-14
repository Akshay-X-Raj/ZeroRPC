# ZeroRPC
 
A small C++17 project that compares **inter-process communication (IPC) latency** on Linux:
 
| Method | Mechanism |
|---|---|
| **ZeroRPC** | POSIX shared memory (`shm_open` + `mmap`) with a futex-based handshake |
| **Unix Socket** | `AF_UNIX` / `SOCK_STREAM` |
| **TCP** | `AF_INET` loopback (`127.0.0.1:5555`) |
| **Pipe** | Named pipe (FIFO) at `/tmp/zerorpc_pipe` |
 
Latency is measured in CPU cycles with `rdtscp` and converted to microseconds using a calibrated TSC frequency. An [ImGui](https://github.com/ocornut/imgui) + SDL2 dashboard shows p50/p90/p99/max latency and throughput for each method.
 
## Requirements
 
- Linux (uses futex syscalls, `shm_open`, FIFOs)
- x86-64 CPU (uses `rdtscp` / `x86intrin.h`)
- GCC or Clang with C++17
- CMake ≥ 3.10
- SDL2 development package (GUI only)
  - Debian/Ubuntu: `sudo apt install libsdl2-dev`
  - Arch: `sudo pacman -S sdl2`
  - Fedora: `sudo dnf install SDL2-devel`
- Dear ImGui is vendored in `third_party/imgui/` (with SDL2 + SDL_Renderer backends)
## Build
 
```bash
cd zerorpc
cmake -S . -B build
cmake --build build -j
```
 
This produces six executables in `build/`:
 
| Binary | Purpose |
|---|---|
| `zerorpc_server` | Shared-memory (futex) server |
| `zerorpc_unix_socket_server` | Unix domain socket echo server |
| `zerorpc_tcp_server` | TCP echo server on 127.0.0.1:5555 |
| `zerorpc_pipe_server` | FIFO echo server |
| `zerorpc_client` | Command-line client; sends one `"ping"` per method and prints the latency |
| `zerorpc_gui` | ImGui benchmark dashboard |
 
## Run
 
### Everything at once (GUI)
 
```bash
./run_all.sh
```
 
Starts all four servers in the background, waits one second, launches the GUI, and kills everything on exit.
 
### Command-line client
 
Start the servers you want in separate terminals, then run the client:
 
```bash
./build/zerorpc_server &
./build/zerorpc_unix_socket_server &
./build/zerorpc_pipe_server &
./build/zerorpc_client
```
 
Example output format:
 
```
ZeroRPC: ping [ack]  (x.xx µs)
UnixSocket: ping [ack]  (x.xx µs)
Pipe: ping [ack]  (x.xx µs)
```
 
Note that the client connects to the shared-memory, Unix socket and pipe servers all at once, so all three must be running. It does not test TCP (there is no TCP client yet).
 
### GUI controls
 
- Click an IPC method button to run a 10,000-request benchmark for it.
- **Save Logs** writes the log pane to `logs/benchmark_log_<timestamp>.txt`.
- **F11** toggles fullscreen.
## Project layout
 
```
zerorpc/
├── CMakeLists.txt
├── run_all.sh              # launches servers + GUI
├── include/
│   ├── Benchmark.h         # TSC helpers (duplicates RDTSCTimer.h)
│   ├── RDTSCTimer.h        # rdtscp / cycles_to_us / calibrate_tsc_ghz
│   ├── Futex.h             # futex_wait / futex_wake wrappers
│   ├── SharedMemory.h      # RAII wrapper around shm_open/mmap
│   ├── Client.h, Server.h  # shared-memory RPC endpoints
│   ├── SocketClient.h      # Unix socket client
│   └── PipeClient.h        # FIFO client
├── src/
│   ├── main_server.cpp, Server.cpp         # shm server
│   ├── main_client.cpp, Client.cpp         # CLI client + shm client
│   ├── SocketServer.cpp, SocketClient.cpp  # Unix socket
│   ├── TcpServer.cpp                       # TCP server
│   ├── PipeServer.cpp, PipeClient.cpp      # FIFO
│   ├── BenchmarkGUI.cpp                    # ImGui dashboard (has main())
│   ├── SharedMemory.cpp, Futex.cpp, RDTSCTimer.cpp, Benchmark.cpp
│   └── main_gui.cpp.bak                    # unused leftover
└── third_party/imgui/      # vendored Dear ImGui
```
 
## How the shared-memory RPC works
 
Client and server map the same `RpcBuffer` from `/dev/shm/zerorpc_shm`:
 
```cpp
struct RpcBuffer {
    std::atomic<int> flag;   // 0 = idle, 1 = request ready, 2 = response ready
    char message[256];
};
```
 
1. Client copies its message into the buffer, sets `flag = 1`, and wakes the server with `FUTEX_WAKE`.
2. Server sees `flag == 1`, appends `" [ack]"`, sets `flag = 2`, and wakes the client.
3. Client sees `flag == 2`, resets `flag = 0`, and returns the response.
