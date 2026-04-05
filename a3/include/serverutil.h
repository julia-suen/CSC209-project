#ifndef SERVERUTIL_H
#define SERVERUTIL_H

#include "../include/protocol.h"
#include "../include/server.h"
#include "../include/common.h"

#include <netinet/in.h>
#include <sys/select.h>

void remove_client_from_list(usr_node *list, usr_node *client, server_data *server);
void add_client_to_list(usr_node *list, usr_node *client, server_data *server);
void get_client_by_id(int id, client *list);
void get_client_by_fd(int fd, client *list);

void add_to_pkt_list(pkt_node *pkts, Packet *pkt, server_data *server);
void remove_pkt_and_deallocate(pkt_node *pkts, server_data *server);

void process_dm(int* clients, Packet *pkt);
void process_msg(chatroom *rooms, Packet *pkt);
#endif