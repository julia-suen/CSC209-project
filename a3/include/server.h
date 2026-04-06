#ifndef SERVER_H
#define SERVER_H

#include "../include/protocol.h"
#include "../include/common.h"
#include <netinet/in.h>
#include <sys/select.h>

#define MAX_PKTS 32
typedef struct {
    int server_fd;
    int num_clients;
    int max_clients;
    int max_fd;
    int num_packets;
    Packet *pkts;
    int num_rooms;
    int *clients;
} server_data;

typedef struct{
    int sender_fd;
    Packet* pkt;
    pkt_node *next;
    pkt_node *prev;
} pkt_node;

typedef struct{
    int fd;
    usr_data *data;
    usr_node *prev;
    usr_node *next;
}usr_node;

#endif