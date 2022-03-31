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
    if (argc != 3) {
        cerr << "Invalid number of arguments!" << endl;
        return EXIT_FAILURE;
    }

    player player;

    int num_players;

    int socket_fd = init_client(argv[1], argv[2]);

    recv(socket_fd, &player.id, sizeof(player.id), 0);
    recv(socket_fd, &num_players, sizeof(num_players), 0);

    cout << "Connected as player " << player.id << " out of " << num_players
         << " total players" << endl;

    player.socket_fd = init_server("");

    getport(&(player.port), player.socket_fd);
    send(socket_fd, &player.port, sizeof(player.port), 0);

    memset(player.hostname, 0, sizeof(player.hostname));
    gethostname(player.hostname, sizeof(player.hostname));
    send(socket_fd, &player.hostname, sizeof(player.hostname), 0);

    char left_hostname[100];
    int left_port;

    recv(socket_fd, &left_hostname, sizeof(left_hostname), 0);
    recv(socket_fd, &left_port, sizeof(left_port), 0);

    // connect left neighbour as client
    int left_fd = init_client(left_hostname, to_string(left_port).c_str());

    // connect right neighbour as server
    struct sockaddr_in socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int right_fd = accept(player.socket_fd, (struct sockaddr*)&socket_addr,
                          &socket_addr_len);
    if (right_fd == -1) {
        cerr << "Error: cannot accept connection on socket" << endl;
        exit(EXIT_FAILURE);
    }

    fd_set rset;
    vector<int> fd_list = {left_fd, right_fd, socket_fd};
    Potato potato;
    srand((unsigned int)time(NULL) + player.id);
    // cout << "fd_list: " << player.socket_fd << ' ' <<  left_fd << ' ' <<
    // right_fd << ' ' << socket_fd << endl;

    while (true) {
        FD_ZERO(&rset);
        int fdmax = 0;

        for (int& fd : fd_list) {
            FD_SET(fd, &rset);
            fdmax = max(fdmax, fd);
        }
        select(fdmax + 1, &rset, NULL, NULL, NULL);

        for (int& fd : fd_list) {
            if (FD_ISSET(fd, &rset)) {
                recv(fd, &potato, sizeof(potato), MSG_WAITALL);
                // cout << "recv potato from fd " << fd << " with hop " <<
                // potato.get_hops() << endl;
                break;
            }
        }

        int hops = potato.num_hops;

        if (hops >= 1) {
            potato.trace[potato.stops] = (player.id);
            potato.stops++;
            potato.num_hops--;
            // cout << "now potato hop: " << potato.get_hops() << endl;
            if (potato.num_hops >= 1) {
                int nxt_idx = rand() % 2;  // 0:left, 1:right
                send(fd_list[nxt_idx], &potato, sizeof(potato), 0);
                cout << "Sending potato to "
                     << (player.id + 2 * nxt_idx - 1 + num_players) %
                            num_players
                     << endl;
            } else {
                send(socket_fd, &potato, sizeof(potato), 0);
                // cout << "Sending potato to server " << socket_fd << endl;
                cout << "I'm it" << endl;
            }
        } else {
            break;
        }
    }

    sleep(1);

    close(left_fd);
    close(right_fd);
    close(socket_fd);
    return EXIT_SUCCESS;
    // // player <machine_name> <port_num>
    // if (argc != 3) {
    //     cout << "Wrong number of arguments\n" << endl;
    //     return EXIT_FAILURE;
    // }

    // char* master_hostname = argv[1];
    // char* master_port = argv[2];

    // player Myplayer;

    // // Establish connection to ringmaster
    // int master_socket_fd = init_client(master_hostname, master_port);
    // // Receive player id and number of players from ringmaster
    // int num_players;
    // recv(master_socket_fd, &(Myplayer.id), sizeof(Myplayer.id), 0);
    // recv(master_socket_fd, &(num_players), sizeof(num_players), 0);
    // // cout << "Receiving player id as " << Myplayer.id << " in length "
    // //      << sizeof(Myplayer.id) << endl;

    // cout << "Connected as player " << Myplayer.id << " out of " <<
    // num_players
    //      << " total players" << endl;
    // // Initialize a server for this player
    // Myplayer.socket_fd = init_server("");

    // getport(&(Myplayer.port), Myplayer.socket_fd);
    // send(master_socket_fd, &Myplayer.port, sizeof(Myplayer.port), 0);

    // // Send player's hostname and port number to ringmaster
    // memset(Myplayer.hostname, 0, sizeof(Myplayer.hostname));
    // gethostname(Myplayer.hostname, sizeof(Myplayer.hostname));
    // send(master_socket_fd, &Myplayer.hostname, strlen(Myplayer.hostname) + 1,
    //      0);

    // // Receive information of the left server from ringmaster
    // char left_hostname[100];
    // // memset(left_hostname, 0, sizeof(left_hostname));
    // int left_port;
    // recv(master_socket_fd, &left_hostname, sizeof(left_hostname),
    // MSG_WAITALL); recv(master_socket_fd, &left_port, sizeof(left_port), 0);

    // // cout << "Receiving left hostname as " << left_hostname << " in length
    // "
    // //      << strlen(left_hostname) << endl;
    // // cout << "Receiving left port as " << left_port << " in size "
    // //      << sizeof(left_port) << endl;

    // // Connect with left server
    // int left_fd, right_fd;
    // left_fd = init_client(left_hostname, to_string(left_port).c_str());

    // // Connect with right client
    // struct sockaddr_storage socket_addr;
    // socklen_t socket_addr_len = sizeof(socket_addr);
    // right_fd = accept(Myplayer.socket_fd, (struct sockaddr*)&socket_addr,
    //                   &socket_addr_len);

    // // Send to right
    // // string message = "connection from right!" + to_string(Myplayer.id);
    // // send(left_fd, message.c_str(), message.size() + 1, 0);

    // // // Verify connection from right
    // // char buffer[512];
    // // recv(right_fd, buffer, 24, 0);
    // // buffer[25] = 0;
    // // cout << buffer << endl;

    // // // Send to left
    // // message = "connection from left!" + to_string(Myplayer.id);
    // // send(right_fd, message.c_str(), message.size() + 1, 0);

    // // Verify connection from left
    // // recv(left_fd, buffer, 23, 0);
    // // buffer[25] = 0;
    // // cout << buffer << endl;

    // if (right_fd == -1) {
    //     cerr << "Error: cannot accept connection on socket" << endl;
    //     return EXIT_FAILURE;
    // }

    // // cout << "Left:" << left_fd << " Right:" << right_fd << endl;

    // // Receiving potato
    // Potato potato;
    // fd_set rd_fd;
    // vector<int> fd_list{master_socket_fd, left_fd, right_fd};
    // srand((unsigned int)time(NULL) + Myplayer.id);

    // // cout << "Left:" << left_fd << " Right:" << right_fd << endl;

    // while (true) {
    //     FD_ZERO(&rd_fd);
    //     for (int& fd : fd_list) {
    //         FD_SET(fd, &rd_fd);
    //     }
    //     int maxfd = *max_element(fd_list.begin(), fd_list.end());
    //     select(maxfd + 1, &rd_fd, NULL, NULL, NULL);

    //     for (int& fd : fd_list) {
    //         if (FD_ISSET(fd, &rd_fd)) {
    //             recv(fd, &potato, sizeof(potato), MSG_WAITALL);
    //             // if (fd == left_fd) {
    //             //     cout << "Receiving potato from left " << fd << " with
    //             //     hop"
    //             //          << potato.num_hops << endl;
    //             // } else if (fd == right_fd) {
    //             //     cout << "Receiving potato from right " << fd << " with
    //             //     hop "
    //             //          << potato.num_hops << endl;
    //             // } else {
    //             //     cout << "Receiving potato from master " << fd
    //             //          << " with hop " << potato.num_hops << endl;
    //             // }
    //             break;
    //         }
    //     }

    //     if (potato.num_hops < 1) {  // Shut down the player
    //         break;
    //     } else {
    //         potato.num_hops--;
    //         potato.stops++;
    //         potato.trace[potato.stops] = Myplayer.id;
    //         if (potato.num_hops < 1) {
    //             send(master_socket_fd, &potato, sizeof(potato), 0);
    //             cout << "I’m it" << endl;
    //         } else {
    //             int random = rand() % 2;
    //             if (random == 1) {  // Send to right neighbor
    //                 send(right_fd, &potato, sizeof(potato), 0);
    //                 int right_id =
    //                     (Myplayer.id == num_players - 1) ? 0 : Myplayer.id +
    //                     1;
    //                 cout << "Sending potato to " << right_id << endl;
    //             } else {  // Send to left neighbor
    //                 send(left_fd, &potato, sizeof(potato), 0);
    //                 int left_id =
    //                     (Myplayer.id == 0) ? num_players - 1 : Myplayer.id -
    //                     1;
    //                 cout << "Sending potato to " << left_id << endl;
    //             }
    //         }
    //     }
    // }

    // close(master_socket_fd);
    // close(left_fd);
    // close(right_fd);

    // return EXIT_SUCCESS;
}