#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

#include "../include/protocol.h"
#include "../include/server.h"
#include "../include/serverutil.h"
#include "../include/common.h"
pkt_node *recieve_until_full(usr_data *users, fd_set *rfd, fd_set *mfd, pkt_node *pkts, server_data *server){
    for (int i = 0; i < server->num_clients; i++){
        if (server->num_packets >= 32) break;
        if (FD_ISSET(users[i].fd, rfd)){
            printf("Incoming Packet\n");
            Packet *in_pkt = malloc(sizeof(Packet));
            
            if (recv_packet(users[i].fd, in_pkt) != 0){
                printf("client disconnect\n");
                remove_client_from_list(users, users[i], server);
                printf("Remaining clients: %d\n", server->num_clients);
                FD_CLR(users[i].fd, mfd);
                continue;
            }

            // print_packet(in_pkt);        // debug print packet
            strncat(in_pkt->usrid, users[i].username, MAX_DEST-1);
            pkts = add_to_pkt_list(users[i].fd, pkts, in_pkt, server);
            server->num_packets++;
        }
    }
    return pkts;
}

void generate_random_name(char *name, usr_data *list, int n){
    do{
        snprintf(name, MAX_USER, "%d", rand() % 99999);
    }while(find_client_by_name(list, n, name) != -1);
}

int connect_new_client(fd_set *master, usr_data *usr_list, server_data *server){
    printf("new incoming connection\n");
    struct sockaddr_in nc_addr;
    nc_addr.sin_family = AF_INET;
    unsigned int client_len = sizeof(struct sockaddr_in);

    int new_client_fd = accept(server->server_fd, (struct sockaddr *) &nc_addr, &client_len);
    
    if (new_client_fd < 0){
        perror("accept failed");
        return -1;
    }
    
    //create new_client
    if ((server->num_clients) >= server->max_clients){
        return -1;
    }
    usr_list[server->num_clients].fd = new_client_fd;
    memset(usr_list[server->num_clients].room_id, 0, MAX_DEST);
    char temp_name[MAX_USER];
    generate_random_name(temp_name, usr_list, server->num_clients);
    strncpy(usr_list[server->num_clients].username, temp_name, 32);
    
    //set new max_fd
    if (new_client_fd > server->max_fd) server->max_fd = new_client_fd;
    server->num_clients++;

    FD_SET(new_client_fd, master);

    return 0;
}


int main(){
    server_data server;
    pkt_node *pkt_head = NULL;
    fd_set master_list;
    chatroom *room_head = NULL;
    fd_set ignore_list;
    server.num_rooms = 4;
    FD_ZERO(&master_list);
    FD_ZERO(&ignore_list);
    server.max_clients = 16;
    usr_data *usr_list = malloc(sizeof(usr_data *) * server.max_clients);
    // Uhh is this even good to do?

    server.num_clients = 0;
    server.num_packets = 0;


    struct sockaddr_in addr;

    struct timeval timeout = {1, 0};
    if ((server.server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(1);

    }
    FD_SET(server.server_fd, &master_list);
    FD_SET(server.server_fd, &ignore_list);
    server.max_fd = server.server_fd;

    // bind to port
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    // todo fix port
    addr.sin_port = htons(PORT);
    memset(&(addr.sin_zero), 0, 8);

    if (bind(server.server_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1){
        perror("bind");
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
        select(server.max_fd + 1, &rfds, NULL, NULL, &timeout);
        // connect  block
        if (FD_ISSET(server.server_fd, &rfds)){
            printf("connection code: %d\n", connect_new_client(&master_list, usr_list, &server));
        }

        // read block
        pkt_head = recieve_until_full(usr_list, &rfds, &master_list, pkt_head, &server);

        // write block
        if (server.num_packets > 0){
            fd_set wfds;
            FD_ZERO(&wfds);
            wfds = master_list;
            select(server.max_fd +1, NULL, &wfds, &ignore_list, &timeout);

            pkt_node *curr_node = pkt_head;
            while(curr_node){
                if (!FD_ISSET(curr_node->sender_fd, &wfds)){
                    curr_node = curr_node->next;
                    continue;
                }

                switch (curr_node->pkt->type){
                    case MSG_TEXT:
                        process_text(usr_list, server.num_clients, curr_node->sender_fd, &room_head, curr_node->pkt);
                        break;
                    case MSG_DM:
                        process_dm(usr_list, curr_node->pkt, &server);
                        break;
                    case MSG_LEAVE:
                        process_leave(usr_list, server.num_clients, &room_head, curr_node->sender_fd);
                        break;
                    case MSG_JOIN:
                        process_join(usr_list, server.num_clients, &room_head, curr_node->pkt, curr_node->sender_fd);
                        break;
                    case MSG_NICK:
                        process_nick(usr_list, server.num_clients, curr_node->sender_fd, curr_node->pkt);
                        break;
                    case MSG_WHO:
                        process_who(usr_list, curr_node->sender_fd, room_head, server.num_clients);
                        break;
                    case MSG_LIST:
                        process_list(usr_list, curr_node->sender_fd, room_head, server.num_clients);
                        break;
                    case MSG_QUIT:
                        process_quit(&master_list, usr_list, &room_head, curr_node->sender_fd, &server);
                        break;
                    default:
                }
                pkt_head = remove_pkt_and_deallocate(pkt_head, curr_node, &server);
                curr_node = curr_node->next;
            }
        }
        fflush(stdout);
    }
    
    return 0;        
}


