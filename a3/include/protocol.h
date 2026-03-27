#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <time.h>

#define MAX_USER 32
#define MAX_DEST 16
#define MAX_MSG 512

typedef enum {
    MSG_TEXT,
    MSG_DM,
    MSG_JOIN,
    MSG_LEAVE,
    MSG_NICK,
    MSG_WHO,
    MSG_LIST,
    MSG_QUIT,
    MSG_SYSTEM,
    MSG_ERROR
} MessageType;

typedef struct {
    MessageType type;
    char usrid[MAX_USER];
    char destination[MAX_DEST];
    char message[MAX_MSG];
    time_t timestamp;
} Packet;

int send_packet(int fd, const Packet *pkt);
int recv_packet(int fd, Packet *pkt);
void init_packet(Packet *pkt);
void print_packet(const Packet *pkt);

#endif