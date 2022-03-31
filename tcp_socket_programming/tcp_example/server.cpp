#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo* host_info_list;
    const char* hostname = NULL;
    const char* port = "4444";

    memset(&host_info, 0, sizeof(host_info));

    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags =
        AI_PASSIVE;  // AI_PASSIVE && nodename==null: the returned socket addr
                     // can be used in bind()

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    // hostname
    // can be either a domain name, such as "example.com", an address string,
    // such as "127.0.0.1", or NULL in which case the address 0.0.0.0 or
    // 127.0.0.1 is assigned depending on the hints flags.
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }  // if

    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }  // if

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status =
        bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot bind socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }  // if

    status = listen(socket_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        return -1;
    }  // if

    cout << "Waiting for connection on port " << port << endl;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    client_connection_fd =
        accept(socket_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
    cout << "1" << endl;
    if (client_connection_fd == -1) {
        cerr << "Error: cannot accept connection on socket" << endl;
        return -1;
    }  // if

    char buffer[512];
    recv(client_connection_fd, buffer, 9, 0);  // 9 --> store 9 bytes into
                                               // buffer
    buffer[9] = 0;

    // const char *message = "hi there!";
    // send(client_connection_fd, message, strlen(message), 0);

    cout << "Server received: " << buffer << endl;

    freeaddrinfo(host_info_list);
    close(socket_fd);

    return 0;
}
