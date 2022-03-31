#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

class player {
   public:
    char hostname[100];
    int port;
    int socket_fd;
    int id;
};

class Potato {
   public:
    int trace[512];
    int num_hops;
    int stops;

    Potato() : num_hops(0), stops(0) { memset(trace, 0, sizeof(trace)); }
    Potato(int hops) : num_hops(hops), stops(0) {
        memset(trace, 0, sizeof(trace));
    }
};

int init_server(const char* port);
int init_client(const char* hostname, const char* port);
void getport(int* port_ptr, int socket_fd);