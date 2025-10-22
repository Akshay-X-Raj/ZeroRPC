#include "Server.h"

int main() {
    Server server("/zerorpc_shm");
    server.run();
    return 0;
}
