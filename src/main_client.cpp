#include "Client.h"
#include "SocketClient.h"
#include "PipeClient.h"
#include "RDTSCTimer.h"
#include <iostream>

int main() {
    try {
        double ghz = calibrate_tsc_ghz();

        Client shmClient("/zerorpc_shm");
        SocketClient unixClient("/tmp/zerorpc_sock");
        PipeClient pipeClient("/tmp/zerorpc_pipe");

        for (auto& method : {"ZeroRPC", "UnixSocket", "Pipe"}) {
            uint64_t start = rdtscp();
            std::string response;
            if (std::string(method) == "ZeroRPC") response = shmClient.call("ping");
            else if (std::string(method) == "UnixSocket") response = unixClient.call("ping");
            else response = pipeClient.call("ping");
            uint64_t end = rdtscp();
            double us = cycles_to_us(start, end, ghz);
            std::cout << method << ": " << response << "  (" << us << " µs)" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
