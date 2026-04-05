#ifndef SERVER_H
#define SERVER_H

#include "../include/protocol.h"
#include <netinet/in.h>
#include <sys/select.h>

#define MAX_PKTS 32
#define TIMEOUT 1
typedef struct {
    int server_fd;
    int num_clients;
    int max_fd;
    int num_packets;
    Packet *pkts;
    int *clients;
} server_data;


typedef struct pkt_node pkt_node;
struct pkt_node{
    Packet* pkt;
    pkt_node *next;
    pkt_node *prev;
};


#endif