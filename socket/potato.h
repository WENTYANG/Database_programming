#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

typedef struct player_t {
    char hostname[100];
    int port;
    int socket_fd;
    int id;
} player;

class Potato {
   public:
    int trace[512];
    int num_hops;
    int stops;

    Potato() : num_hops(0), stops(0) { memset(trace, 0, sizeof(trace)); }
};
int init_server(const char* port) {
    const char* hostname = NULL;
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo* host_info_list;

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags =
        AI_PASSIVE;  // AI_PASSIVE && nodename==null: the returned socket addr
                     // can be used in bind()

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    if (strlen(port) == 0) {
        struct sockaddr_in* addr_in =
            (struct sockaddr_in*)(host_info_list->ai_addr);
        addr_in->sin_port = 0;
    }

    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                        sizeof(int));  //设置端口可复用
    status =
        bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot bind socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    status = listen(socket_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    // cout << "Waiting for connection on port " << port << endl;

    freeaddrinfo(host_info_list);
    return socket_fd;
}

int init_client(const char* hostname, const char* port) {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo* host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    // cout << "Connecting to " << hostname << " on port " << port << "..."
    //      << endl;

    status =
        connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(host_info_list);
    return socket_fd;
}

void getport(int* port_ptr, int socket_fd) {
    // struct sockaddr_in addr;
    // socklen_t socketlen;

    // addr.sin_family = AF_INET;
    // addr.sin_port = 0;  // 绑定随机端口
    // addr.sin_addr.s_addr = INADDR_ANY;
    // socketlen = sizeof(addr);

    // // 获取系统随机分配的端口号
    // getsockname(socket_fd, (sockaddr*)&addr, &socketlen);
    // // printf("%u\n", ntohs(addr.sin_port));
    // int port = ntohs(addr.sin_port);
    // // _itoa(port, port_ptr, 10);
    // // port_ptr = (char*)(to_string(port)).c_str();
    // *port_ptr = port;
    struct sockaddr_in socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int status = getsockname(socket_fd, (struct sockaddr*)&socket_addr,
                             &socket_addr_len);
    if (status == -1) {
        cerr << "Error: cannot get socket name" << endl;
        exit(EXIT_FAILURE);
    }
    *port_ptr = ntohs(socket_addr.sin_port);
}