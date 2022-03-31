#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <vector>

#include "potato.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Invalid number of arguments!" << endl;
        return EXIT_FAILURE;
    }

    int num_players, num_hops;

    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << num_players << endl;
    cout << "Hops = " << num_hops << endl;

    int socket_fd = init_server(argv[1]);

    vector<player> players(num_players);

    for (int i = 0; i < num_players; ++i) {
        players[i].id = i;

        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_connection_fd;
        client_connection_fd =
            accept(socket_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            exit(EXIT_FAILURE);
        }

        players[i].socket_fd = client_connection_fd;

        send(players[i].socket_fd, &i, sizeof(i), 0);
        send(players[i].socket_fd, &num_players, sizeof(num_players), 0);

        cout << "Player " << i << " is ready to play" << endl;

        memset(players[i].hostname, 0, sizeof(players[i].hostname));
        recv(players[i].socket_fd, &players[i].port, sizeof(players[i].port),
             MSG_WAITALL);
        recv(players[i].socket_fd, &players[i].hostname,
             sizeof(players[i].hostname), MSG_WAITALL);

        // cout << i << ' ' << players[i].socket_fd << ' ' <<
        // players[i].hostname << ' ' << players[i].port << endl;
    }

    for (int i = 0; i < num_players; ++i) {
        int left_idx = (i + num_players - 1) % num_players;
        send(players[i].socket_fd, &players[left_idx].hostname,
             sizeof(players[left_idx].hostname), 0);
        send(players[i].socket_fd, &players[left_idx].port,
             sizeof(players[left_idx].port), 0);
    }

    Potato potato;
    potato.num_hops = num_hops;

    if (num_hops) {
        srand((unsigned int)time(NULL) + num_players);
        int first_player = rand() % num_players;

        send(players[first_player].socket_fd, &potato, sizeof(potato), 0);

        cout << "Ready to start the game, sending potato to player "
             << first_player << endl;

        fd_set rset;
        FD_ZERO(&rset);
        int fdmax = 0;
        for (int i = 0; i < num_players; ++i) {
            FD_SET(players[i].socket_fd, &rset);
            fdmax = max(fdmax, players[i].socket_fd);
        }
        select(fdmax + 1, &rset, NULL, NULL, NULL);

        for (int i = 0; i < num_players; ++i) {
            if (FD_ISSET(players[i].socket_fd, &rset)) {
                recv(players[i].socket_fd, &potato, sizeof(potato),
                     MSG_WAITALL);
                // cout << "recv potato from " << players[i].player_id << endl;
                break;
            }
        }
    }

    cout << "Trace of potato:" << endl;
    for (int i = 0; i < potato.stops; i++) {
        if (i > 0)
            cout << ",";
        cout << potato.trace[i];
    }
    cout << endl;

    for (int i = 0; i < num_players; ++i) {
        send(players[i].socket_fd, &potato, sizeof(potato), 0);
    }

    for (int i = 0; i < num_players; ++i) {
        close(players[i].socket_fd);
    }
    close(socket_fd);
    return EXIT_SUCCESS;

    // // ringmaster <port_num> <num_players> <num_hops>
    // if (argc != 4) {
    //     cerr << "Wrong number of arguments\n" << endl;
    //     return EXIT_FAILURE;
    // }

    // char* port = argv[1];
    // int num_players = atoi(argv[2]);
    // int num_hops = atoi(argv[3]);

    // int master_socket_fd = init_server(port);

    // cout << "Potato Ringmaster" << endl;
    // cout << "Players = " << num_players << endl;
    // cout << "Hops = " << num_hops << endl;

    // vector<player> players(num_players);
    // for (int i = 0; i < num_players; i++) {
    //     players[i].id = i;

    //     struct sockaddr_storage socket_addr;
    //     socklen_t socket_addr_len = sizeof(socket_addr);
    //     int player_connection_fd;
    //     player_connection_fd = accept(
    //         master_socket_fd, (struct sockaddr*)&socket_addr,
    //         &socket_addr_len);
    //     if (player_connection_fd == -1) {
    //         cerr << "Error: cannot accept connection on socket" << endl;
    //         return EXIT_FAILURE;
    //     }
    //     players[i].socket_fd = player_connection_fd;

    //     cout << "Player " << i << " is ready to play" << endl;

    //     // Send player id and number of players to each player
    //     send(players[i].socket_fd, &i, sizeof(i), 0);
    //     send(players[i].socket_fd, &num_players, sizeof(num_players), 0);

    //     // Receive player's hostname and port number
    //     memset(players[i].hostname, 0, sizeof(players[i].hostname));
    //     recv(players[i].socket_fd, &(players[i].port),
    //     sizeof(players[i].port),
    //          MSG_WAITALL);
    //     recv(players[i].socket_fd, &(players[i].hostname),
    //          sizeof(players[i].hostname), MSG_WAITALL);

    //     // cout << "Hostname is " << players[i].hostname << endl;
    //     // cout << "Port is " << players[i].port << endl;
    // }

    // // Establish connections between neighbors, left is server, right is
    // client for (int i = 0; i < num_players; i++) {
    //     int left_id = i == 0 ? (num_players - 1) : (i - 1);
    //     int right_id = i == num_players - 1 ? 0 : (i + 1);

    //     // The left neighbor is a server, send its hostname and port number
    //     send(players[i].socket_fd, &players[left_id].hostname,
    //          sizeof(players[left_id].hostname), 0);

    //     send(players[i].socket_fd, &players[left_id].port,
    //          sizeof(players[left_id].port), 0);

    //     // cout << "Master sending left hostname of " << left_id << " as "
    //     //      << left.hostname << " in length " << strlen(left.hostname) <<
    //     //      endl;
    //     // cout << "Master sending left port as " << left.port << " in size "
    //     //      << sizeof(left.port) << endl;

    //     // The right neighbor is a client, don't need to send anything
    // }

    // Potato potato;
    // potato.num_hops = num_hops;

    // if (potato.num_hops != 0) {
    //     // Send first potato
    //     srand((unsigned int)time(NULL) + num_players);
    //     int first_player = rand() % num_players;

    //     send(players[first_player].socket_fd, &potato, sizeof(potato), 0);
    //     cout << "Ready to start the game, sending potato to player "
    //          << first_player << endl;

    //     // Waiting to receive the final potato
    //     fd_set rd_fd;
    //     FD_ZERO(&rd_fd);
    //     int maxfd = 0;
    //     for (int i = 0; i < num_players; ++i) {
    //         FD_SET(players[i].socket_fd, &rd_fd);
    //         if (players[i].socket_fd > maxfd)
    //             maxfd = players[i].socket_fd;
    //     }
    //     select(maxfd + 1, &rd_fd, NULL, NULL, NULL);

    //     bool end = false;
    //     for (int i = 0; i < num_players; ++i) {
    //         if (FD_ISSET(players[i].socket_fd, &rd_fd)) {
    //             recv(players[i].socket_fd, &potato, sizeof(potato),
    //                  MSG_WAITALL);
    //             end = true;
    //             break;
    //         }
    //     }
    // }

    // // Shut down the players
    // for (int i = 0; i < num_players; ++i) {
    //     send(players[i].socket_fd, &potato, sizeof(potato), 0);
    // }

    // cout << "Trace of potato:" << endl;
    // for (int i = 0; i < potato.stops; i++) {
    //     if (i > 0)
    //         cout << ",";
    //     cout << potato.trace[i];
    // }
    // cout << endl;

    // for (int i = 0; i < num_players; i++) {
    //     close(players[i].socket_fd);
    // }
    // close(master_socket_fd);
    // return EXIT_SUCCESS;
}