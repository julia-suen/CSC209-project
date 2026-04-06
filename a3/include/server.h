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

typedef struct pkt_node{
    int sender_fd;
    Packet* pkt;
    struct pkt_node *next;
    struct pkt_node *prev;
} pkt_node;

#endif

