#include "Server.h"
#include <iostream>

int main() {
    try {
        Server server("/zerorpc_shm");
        std::cout << "[ZeroRPC Server] Running... Ctrl+C to stop." << std::endl;
        server.run();
    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
