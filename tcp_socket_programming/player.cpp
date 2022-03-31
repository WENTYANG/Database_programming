#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "potato.h"

using namespace std;

int main(int argc, char* argv[]) {
    // player <machine_name> <port_num>
    if (argc != 3) {
        cout << "Wrong number of arguments\n" << endl;
        return EXIT_FAILURE;
    }

    char* master_hostname = argv[1];
    char* master_port = argv[2];

    player Myplayer;
    memset(&Myplayer, 0, sizeof(Myplayer));

    // Establish connection to ringmaster
    int master_socket_fd = init_client(master_hostname, master_port);
    // Receive player id and number of players from ringmaster
    int num_players;
    recv(master_socket_fd, &(Myplayer.id), sizeof(Myplayer.id), 0);
    recv(master_socket_fd, &(num_players), sizeof(num_players), 0);
    // cout << "Receiving player id as " << Myplayer.id << " in length "
    //      << sizeof(Myplayer.id) << endl;

    cout << "Connected as player " << Myplayer.id << " out of " << num_players
         << " total players" << endl;
    // Initialize a server for this player
    int server_socket_fd = init_server("");
    Myplayer.socket_fd = server_socket_fd;

    gethostname(Myplayer.hostname, sizeof(Myplayer.hostname));
    getport(&(Myplayer.port), server_socket_fd);

    // Send player's hostname and port number to ringmaster
    send(master_socket_fd, &Myplayer.hostname, strlen(Myplayer.hostname) + 1,
         0);
    send(master_socket_fd, &Myplayer.port, sizeof(Myplayer.port), 0);

    // Receive information of the left server from ringmaster
    char left_hostname[100];
    memset(left_hostname, 0, sizeof(left_hostname));
    int left_port;
    recv(master_socket_fd, &left_port, sizeof(left_port), 0);
    recv(master_socket_fd, &left_hostname, sizeof(left_hostname), 0);

    // cout << "Receiving left hostname as " << left_hostname << " in length "
    //      << strlen(left_hostname) << endl;
    // cout << "Receiving left port as " << left_port << " in size "
    //      << sizeof(left_port) << endl;

    // Connect with left server
    int left_fd, right_fd;
    left_fd = init_client(left_hostname, to_string(left_port).c_str());

    // Connect with right client
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    right_fd = accept(server_socket_fd, (struct sockaddr*)&socket_addr,
                      &socket_addr_len);
    if (right_fd == -1) {
        cerr << "Error: cannot accept connection on socket" << endl;
        return EXIT_FAILURE;
    }

    // Send to right
    // string message = "connection from right!" + to_string(Myplayer.id);
    // send(left_fd, message.c_str(), message.size() + 1, 0);

    // // Verify connection from right
    // char buffer[512];
    // recv(right_fd, buffer, 24, 0);
    // buffer[25] = 0;
    // cout << buffer << endl;

    // // Send to left
    // message = "connection from left!" + to_string(Myplayer.id);
    // send(right_fd, message.c_str(), message.size() + 1, 0);

    // Verify connection from left
    // recv(left_fd, buffer, 23, 0);
    // buffer[25] = 0;
    // cout << buffer << endl;

    // cout << "Left:" << left_fd << " Right:" << right_fd << endl;

    // Receiving potato
    Potato potato;
    fd_set rd_fd;
    vector<int> fd_list{master_socket_fd, left_fd, right_fd};
    int maxfd = *max_element(fd_list.begin(), fd_list.end());
    srand((unsigned int)time(NULL) + Myplayer.id);

    // cout << "Left:" << left_fd << " Right:" << right_fd << endl;

    while (true) {
        FD_ZERO(&rd_fd);
        for (int& fd : fd_list) {
            FD_SET(fd, &rd_fd);
        }
        select(maxfd + 1, &rd_fd, NULL, NULL, NULL);
        int size;
        for (int& fd : fd_list) {
            if (FD_ISSET(fd, &rd_fd)) {
                size = recv(fd, &potato, sizeof(potato), 0);

                // if (fd == left_fd) {
                //     cout << "Receiving potato from left " << fd << " with
                //     hop"
                //          << potato.num_hops << endl;
                // } else if (fd == right_fd) {
                //     cout << "Receiving potato from right " << fd << " with
                //     hop "
                //          << potato.num_hops << endl;
                // } else {
                //     cout << "Receiving potato from master " << fd
                //          << " with hop " << potato.num_hops << endl;
                // }

                break;
            }
        }

        if (size == 0 || potato.num_hops < 1) {  // Shut down the player
            break;
        } else {
            // cout << "now have hops: " << potato.num_hops << endl;
            // cout << "add to trace " << potato.stops << endl;

            potato.num_hops--;
            potato.stops++;
            potato.trace[potato.stops] = Myplayer.id;

            if (potato.num_hops < 1) {
                send(master_socket_fd, &potato, sizeof(potato), 0);
                cout << "I’m it" << endl;

                // cout << "Trace of potato:" << endl;
                // for (int i = 0; i < potato.stops; i++) {
                //     if (i > 0)
                //         cout << ",";
                //     cout << potato.trace[i];
                // }
                // cout << endl;

            } else {
                int random = rand() % 2;
                if (random == 1) {  // Send to right neighbor
                    send(right_fd, &potato, sizeof(potato), 0);
                    int right_id =
                        (Myplayer.id == num_players - 1) ? 0 : Myplayer.id + 1;
                    cout << "Sending potato to " << right_id << endl;
                } else {  // Send to left neighbor
                    send(left_fd, &potato, sizeof(potato), 0);
                    int left_id =
                        (Myplayer.id == 0) ? num_players - 1 : Myplayer.id - 1;
                    cout << "Sending potato to " << left_id << endl;
                }
            }
        }
    }

    // close(server_socket_fd);
    close(left_fd);
    close(right_fd);
    close(master_socket_fd);

    return EXIT_SUCCESS;
}