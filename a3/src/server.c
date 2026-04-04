#include "../include/protocol.h"
#include "../include/server.h"
#include "../include/serverutil.h"

#include "../include/common.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

void main(){
    server_data server;
    pkt_node *node_head = NULL;
    fd_set master_list;
    chatroom *rooms = rooms_set_up(4);
    FD_ZERO(&master_list);
    int *clients[MAX_USER];

    // Uhh is this even good to do?

    server.num_clients = 0;
    server.num_packets = 0;
    server.clients = clients;


    struct sockaddr_in addr;

    if ((server.server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(1);

    }
    FD_SET(server.server_fd, &master_list);
    server.max_fd = server.server_fd;

    // bind to port
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    // todo fix port
    addr.sin_port = htons(8080);
    memset(&(addr.sin_zero), 0, 8);

    if (bind(server.server_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1){
        perror('bind');
        close(server.server_fd);
        exit(1);
    }

    if (listen(server.server_fd, 5) < 0){
        perror("listen");
        exit(1);
    }

    while(1){
        fd_set rfds;
        FD_ZERO(&rfds);
        rfds = master_list;
        select(server.max_fd + 1, &rfds, NULL, NULL, TIMEOUT);
        // connect  block
        if (FD_ISSET(server.server_fd, &rfds)){
            connect_new_client(&rfds, clients, &server);
        }

        // read block
        recieve_until_full(clients, &rfds, node_head, &server);

        // write block
        if (server.num_packets > 0){
            fd_set wfds;
            FD_ZERO(&wfds);
            wfds = master_list;

            pkt_node *curr_node = node_head;
            while(curr_node){
                switch (curr_node->pkt->type){
                    case MSG_DM:
                        process_dm(clients, curr_node->pkt);
                    case MSG_TEXT:
                        process_text(clients, curr_node->pkt);
                    default:
                }
            }
        }
    }
            
}

chatroom* rooms_set_up (int n){
    chatroom *rooms = malloc(sizeof(chatroom) * n);
    for (int i = 0; i < n; i ++){
        rooms[i].room_id = i;
        rooms[i].num_users = 0;
    }
    return rooms;
}


int recieve_until_full(int *fds, fd_set *rfd, pkt_node *pkts, server_data *server){
    for (int i = 0; i < server->num_clients; i++){
        if (server->num_packets >= 32) break;

        if (FD_ISSET(fds[i], rfd)){
            Packet *in_pkt = malloc(sizeof(Packet));
            recv_packet(fds[i], in_pkt);
        }
    }
    return;
}


void connect_new_client(fd_set *master, int *client_fds, server_data *server){

    struct sockaddr_in new_client;
    new_client.sin_family = AF_INET;
    unsigned int client_len = sizeof(struct sockaddr_in);

    int new_client_fd = accept(server->server_fd, (struct sockaddr *) &new_client, &client_len);
    
    if (new_client_fd < 0){
        perror("accept failed");
        return;
    }

    if (new_client_fd > server->max_fd) server->max_fd = new_client_fd;
    client_fds[server->num_clients] = new_client_fd;
    server->num_clients++;

    FD_SET(new_client_fd, master);

    return;
}
