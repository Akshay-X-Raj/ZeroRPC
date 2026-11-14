#!/bin/bash
set -e

# Navigate to build directory
cd "$(dirname "$0")/build"

# Start all servers in the background
echo "[+] Starting servers..."
./zerorpc_server & SERVER_PID=$!
./zerorpc_tcp_server & TCP_PID=$!
./zerorpc_unix_socket_server & SOCKET_PID=$!
./zerorpc_pipe_server & PIPE_PID=$!

# Wait a second for them to initialize
sleep 1

# Launch GUI
echo "[+] Launching GUI..."
./zerorpc_gui & GUI_PID=$!

# Cleanup when user exits (Ctrl+C)
trap 'echo "[*] Cleaning up..."; kill $SERVER_PID $TCP_PID $SOCKET_PID $PIPE_PID $GUI_PID 2>/dev/null' EXIT

wait
