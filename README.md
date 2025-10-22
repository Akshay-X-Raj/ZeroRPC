# ZeroRPC  
Ultra-lightweight Shared Memory RPC with Futex Synchronization + GUI Benchmarking

## Features
- Zero-copy IPC using POSIX shared memory
- Futex synchronization for low-latency signaling
- Client/Server RPC request-response cycle
- GUI Dashboard (Dear ImGui + SDL2 + OpenGL)  
  - Send custom RPC messages  
  - Plot request latency (µs) with auto-scaling  
  - Logs panel  
  - Clear Logs button  
  - Save Logs to CSV  

## Dependencies (Arch Linux)
```bash
sudo pacman -S cmake gcc sdl2 glew mesa
