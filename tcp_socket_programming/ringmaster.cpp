#include <bits/stdc++.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <vector>

#include "potato.h"

using namespace std;

int main(int argc, char* argv[]) {
    // ringmaster <port_num> <num_players> <num_hops>
    if (argc != 4) {
        cerr << "Wrong number of arguments\n" << endl;
        return EXIT_FAILURE;
    }

    const char* port = argv[1];
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);

    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << num_players << endl;
    cout << "Hops = " << num_hops << endl;

    int master_socket_fd = init_server(port);

    vector<player> players(num_players);
    // memset(&players, 0, sizeof(players));
    for (int i = 0; i < num_players; i++) {
        players[i].id = i;

        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        players[i].socket_fd = accept(
            master_socket_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
        if (players[i].socket_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            return EXIT_FAILURE;
        }

        cout << "Player " << i << " is ready to play" << endl;

        // cout << "Player " << i << " fd: " << players[i].socket_fd << endl;

        // Send player id and number of players to each player
        send(players[i].socket_fd, &i, sizeof(i), 0);
        send(players[i].socket_fd, &num_players, sizeof(num_players), 0);

        // Receive player's hostname and port number
        recv(players[i].socket_fd, &(players[i].hostname),
             sizeof(players[i].hostname), 0);
        recv(players[i].socket_fd, &(players[i].port), sizeof(players[i].port),
             0);

        // cout << "Hostname is " << players[i].hostname << endl;
        // cout << "Port is " << players[i].port << endl;
    }

    // Establish connections between neighbors, left is server, right is client
    for (int i = 0; i < num_players; i++) {
        int left_id = i == 0 ? (num_players - 1) : (i - 1);
        int right_id = i == num_players - 1 ? 0 : (i + 1);

        player left = players[left_id];
        // player right = players[right_id];

        // The left neighbor is a server, send its hostname and port number
        send(players[i].socket_fd, &(left.port), sizeof(left.port), 0);
        send(players[i].socket_fd, &(left.hostname), strlen(left.hostname) + 1,
             0);

        // cout << "Master sending left hostname of " << left_id << " as "
        //      << left.hostname << " in length " << strlen(left.hostname) <<
        //      endl;
        // cout << "Master sending left port as " << left.port << " in size "
        //      << sizeof(left.port) << endl;

        // The right neighbor is a client, don't need to send anything
    }

    Potato potato = Potato(num_hops);
    // potato.num_hops = num_hops;

    if (potato.num_hops != 0) {
        // Send first potato
        srand((unsigned int)time(NULL) + num_players);
        int first_player = rand() % num_players;

        // cout << "First player fd:" << players[first_player].socket_fd <<
        // endl;

        int size =
            send(players[first_player].socket_fd, &potato, sizeof(potato), 0);
        // cout << "Ready to start the game, sending potato with hops "
        //      << potato.num_hops << endl;
        if (size <= 0) {
            cerr << "Error: fail to send potato to first player" << endl;
            exit(EXIT_FAILURE);
        }

        cout << "Ready to start the game, sending potato to player "
             << first_player << endl;

        // Waiting to receive the final potato
        fd_set rd_fd;
        FD_ZERO(&rd_fd);
        int maxfd = 0;
        for (player& p : players) {
            FD_SET(p.socket_fd, &rd_fd);
            if (p.socket_fd > maxfd)
                maxfd = p.socket_fd;
        }

        select(maxfd + 1, &rd_fd, NULL, NULL, NULL);

        for (player& p : players) {
            if (FD_ISSET(p.socket_fd, &rd_fd)) {
                recv(p.socket_fd, &potato, sizeof(potato), 0);
                // cout << "Receiving from player " << p.id << " with hops "
                //      << potato.num_hops << " fd:" << p.socket_fd << endl;
                break;
            }
        }
    }

    // Shut down the players
    for (player& p : players) {
        send(p.socket_fd, &potato, sizeof(potato), 0);
    }

    cout << "Trace of potato:" << endl;
    for (int i = 0; i < potato.stops; i++) {
        if (i > 0)
            cout << ",";
        cout << potato.trace[i];
    }
    cout << endl;

    for (int i = 0; i < num_players; i++) {
        close(players[i].socket_fd);
    }
    close(master_socket_fd);

    return EXIT_SUCCESS;
}