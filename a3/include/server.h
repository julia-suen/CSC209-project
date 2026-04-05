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

typedef struct {
    int room_id;
    int num_users;
    int *user_fds[MAX_USER];
} chatroom;

typedef struct{
    Packet* pkt;
    pkt_node *next;
    pkt_node *prev;
} pkt_node;

typedef struct{
    int fd;
    char usrid[MAX_USER];
}usr_data;

#endif