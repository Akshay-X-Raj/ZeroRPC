# NanoRPC 🚀
Ultra-Lightweight Shared Memory RPC with Futex Synchronization and GUI Benchmarking

## Overview
NanoRPC is a high-performance **inter-process communication (IPC) system** built for Linux.  
It uses:
- **POSIX shared memory** for true zero-copy data transfer  
- **Linux futexes** for ultra-fast synchronization  

This design enables **microsecond-level latency**, significantly outperforming traditional IPC mechanisms like Unix domain sockets.  

A **Python GUI dashboard** (PyQt + Matplotlib) is included to visualize benchmark results and compare NanoRPC with Unix domain sockets.

---

## Features
- 🚀 Ultra-low latency request/response communication  
- ⚡ Zero-copy IPC using shared memory  
- 🔒 Futex-based synchronization (minimal kernel transitions)  
- 📊 Benchmarking tool to measure RTT and throughput  
- 🎨 Python GUI for real-time visualization of IPC performance  

---

## Getting Started

### Prerequisites
- Linux (Kernel 5.x+, tested on Arch Linux)  
- g++ (C++17 or later)  
- Python 3.9+ with `PyQt5` and `matplotlib`  

Install Python dependencies:
```bash
pip install pyqt5 matplotlib

Build

Compile the server, client, and benchmark tools:
'''bash
make

Run

Start the server:
'''bash
./server

Run the client:
'''bash
./client

Benchmark (Shared Memory vs. Unix Domain Sockets):

'''bash
./benchmark

Launch the GUI dashboard:

'''bash
python3 gui/main.py


##Project Structure

.
├── src/
│   ├── server.cpp
│   ├── client.cpp
│   ├── benchmark.cpp
│   ├── utils.cpp
│   └── ultra_rpc.hpp
├── gui/
│   └── main.py
├── Makefile
├── README.md
├── .gitignore
└── LICENSE


##Roadmap
- Multi-client support
- Asynchronous RPC handling
- Shared memory pool for larger payloads
- Python bindings for direct use in applications


###License

This project is licensed under the MIT License – see the LICENSE

###Author

-Akshay Raj @Akshay-X-Raj


###Acknowledgments

-Linux man pages: futex(2),shm_open(3)
-Ulrich Drepper – “Futexes Are Tricky”
-PyQt and Matplotlib documentation
