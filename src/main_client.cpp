#include "Client.h"

int main() {
    Client client("/zerorpc_shm");
    client.demo_send("ZeroRPC World!");
    return 0;
}
