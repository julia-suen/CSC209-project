#ifndef SERVERUTIL_H
#define SERVERUTIL_H

#include "../include/protocol.h"
#include "../include/server.h"
#include "../include/common.h"

#include <netinet/in.h>
#include <sys/select.h>

int remove_client_from_list(usr_data *client_fds, usr_data client, server_data *server);
pkt_node *add_to_pkt_list(int fd, pkt_node *pkts, Packet *pkt, server_data *server);
pkt_node *remove_pkt_and_deallocate(pkt_node *pkts, pkt_node *pkt, server_data *server_data);
// void get_client_by_id(int id, usr_node *list);
// void get_client_by_fd(int fd, usr_node *list);
// void get_client_by_name(char *name, usr_node *list);

int process_dm(usr_data *clients, Packet *pkt, server_data *server);
int process_text(usr_data *clients, int num_user, int fd, chatroom **room_head, Packet *pkt);
int process_leave(usr_data *clients, int num_clients, chatroom **room_head, int fd);
int process_join(usr_data* clients, int num_clients, chatroom **room_head, Packet *pkt, int fd);
int process_nick(usr_data *clients, int num_clients, int fd, Packet *pkt);
int process_who(usr_data* clients, int fd, chatroom *room_head, int num_clients);
int process_list(usr_data* clients, int fd, chatroom *room_head, int num_clients);
int process_quit(fd_set *master_list, usr_data* clients, chatroom **room_head, int fd, server_data *server);

#endif