#ifndef SERVERUTIL_H
#define SERVERUTIL_H

#include "../include/protocol.h"
#include "../include/server.h"
#include "../include/common.h"

#include <netinet/in.h>
#include <sys/select.h>

int remove_client_from_list(usr_node *list, usr_node *client, server_data *server);
int add_client_to_list(usr_node *list, usr_node *client, server_data *server);
// void get_client_by_id(int id, usr_node *list);
// void get_client_by_fd(int fd, usr_node *list);
// void get_client_by_name(char *name, usr_node *list);

void add_to_pkt_list(int fd, pkt_node *pkts, Packet *pkt, server_data *server);
void remove_pkt_and_deallocate(pkt_node *pkts, server_data *server);

int process_dm(usr_data *clients, Packet *pkt, server_data *server);
int process_text(chatroom *rooms, Packet *pkt, int num_rooms);
int process_join(chatroom *rooms, Packet *pkt, int fd, int num_rooms);
int process_nick(usr_data *clients, int num_clients, int fd, Packet *pkt);
int process_who();
int process_list();
int process_quit(chatroom *rooms, int num_rooms, int fd, Packet *pkt);


#endif