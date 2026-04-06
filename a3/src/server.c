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
    pkt_node *pkt_head = NULL;
    fd_set master_list;
    chatroom *rooms = rooms_set_up(4);
    server.num_rooms = 4;
    FD_ZERO(&master_list);
    server.max_clients = 16;
    usr_data *usr_list = malloc(sizeof(usr_data *) * server.max_clients);
    // Uhh is this even good to do?

    server.num_clients = 0;
    server.num_packets = 0;


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
        select(server.max_fd + 1, &rfds, NULL, NULL, NULL);
        // connect  block
        if (FD_ISSET(server.server_fd, &rfds)){
            connect_new_client(&rfds, usr_list, &server);
        }

        // read block
        recieve_until_full(usr_list, &rfds, pkt_head, &server);

        // write block
        if (server.num_packets > 0){
            fd_set wfds;
            FD_ZERO(&wfds);
            wfds = master_list;
            select(server.max_fd +1, NULL, &wfds, NULL, NULL);

            pkt_node *curr_node = pkt_head;
            while(curr_node){
                if (!FD_ISSET(curr_node->sender_fd, &wfds)){
                    continue;
                }

                switch (curr_node->pkt->type){
                    case MSG_TEXT:
                        process_text(rooms, curr_node->pkt, &server);
                        break;
                    case MSG_DM:
                        process_dm(usr_list, curr_node->pkt, &server);
                        break;
                    case MSG_LEAVE:
                        process_leave();
                        break;
                    case MSG_JOIN:
                        process_join(rooms, curr_node->pkt, curr_node->sender_fd, curr_node);
                        break;
                    case MSG_NICK:
                        process_nick(usr_list, server.num_clients, curr_node->sender_fd, curr_node->pkt);
                        break;
                    case MSG_WHO:
                        process_who();
                        break;
                    case MSG_LIST:
                        process_list();
                        break;
                    case MSG_QUIT:
                        process_quit(rooms, server.num_rooms, curr_node->sender_fd, curr_node->pkt);
                        break;
                    default:
                }
                remove_pkt_and_deallocate(pkt_head, curr_node);
            }
        }
    }
            
}


int recieve_until_full(usr_data *users, fd_set *rfd, pkt_node *pkts, server_data *server){
    for (int i = 0; i < server->num_clients; i++){
        if (server->num_packets >= 32) break;

        if (FD_ISSET(users[i].fd, rfd)){
            Packet *in_pkt = malloc(sizeof(Packet));
            
            recv_packet(users[i].fd, in_pkt);

            add_to_pkt_list(users[i].fd, pkts, in_pkt, server);
        }
    }
    return 0;
}


void connect_new_client(fd_set *master, usr_data *usr_list, server_data *server){

    struct sockaddr_in nc_addr;
    nc_addr.sin_family = AF_INET;
    unsigned int client_len = sizeof(struct sockaddr_in);

    int new_client_fd = accept(server->server_fd, (struct sockaddr *) &nc_addr, &client_len);
    
    if (new_client_fd < 0){
        perror("accept failed");
        return;
    }
    
    //create new_client
    if (server->num_clients = server->max_clients){
        return -1;
    }
    usr_list[server->num_clients].fd = new_client_fd;
    char *temp_name;
    generate_random_name(temp_name, usr_list, server->num_clients);
    strncpy(usr_list[server->num_clients].username, temp_name, 32);
    
    //set new max_fd
    if (new_client_fd > server->max_fd) server->max_fd = new_client_fd;
    server->num_clients++;

    FD_SET(new_client_fd, master);

    return;
}

void generate_random_name(char *name, usr_data *list, int n){
    do{
        name = itoa(rand() % 99999);
    }while(find_client_by_name(list, n, name) != -1);
}
