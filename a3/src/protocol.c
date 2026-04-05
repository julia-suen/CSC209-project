#include "../include/protocol.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>

int send_packet(int fd, const Packet *pkt) {
    size_t total_sent = 0;
    size_t packet_size = sizeof(Packet);
    const char *buf = (const char *)pkt;

    while (total_sent < packet_size) {
        ssize_t n = send(fd, buf + total_sent, packet_size - total_sent, 0);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        if (n == 0) {
            return -1;
        }

        total_sent += (size_t)n;
    }

    return 0;
}

int recv_packet(int fd, Packet *pkt) {
    size_t total_read = 0;
    size_t packet_size = sizeof(Packet);
    char *buf = (char *)pkt;

    while (total_read < packet_size) {
        ssize_t n = recv(fd, buf + total_read, packet_size - total_read, 0);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        if (n == 0) {
            return 1;   /* peer disconnected cleanly */
        }

        total_read += (size_t)n;
    }

    return 0;
}

void init_packet(Packet *pkt) {
    if (pkt == NULL) {
        return;
    }

    memset(pkt, 0, sizeof(Packet));
    pkt->type = MSG_TEXT;
    pkt->timestamp = time(NULL);
}

void print_packet(const Packet *pkt) {
    if (pkt == NULL) {
        return;
    }

    printf("----- Packet Debug -----\n");
    printf("type: %d\n", pkt->type);
    printf("usrid: %s\n", pkt->usrid);
    printf("destination: %s\n", pkt->destination);
    printf("message: %s\n", pkt->message);
    printf("timestamp: %ld\n", (long)pkt->timestamp);
    printf("------------------------\n");
}

int valid_packet_type(MessageType type) {
    return type >= MSG_TEXT && type <= MSG_ERROR;
}

int validate_packet(const Packet *pkt) {
    if (pkt == NULL) {
        return 0;
    }

    if (!valid_packet_type(pkt->type)) {
        return 0;
    }

    switch (pkt->type) {
        case MSG_TEXT:
            /* room message: needs destination + message */
            if (pkt->destination[0] == '\0' || pkt->message[0] == '\0') {
                return 0;
            }
            break;

        case MSG_DM:
            /* dm: needs target user + message */
            if (pkt->destination[0] == '\0' || pkt->message[0] == '\0') {
                return 0;
            }
            break;

        case MSG_JOIN:
        case MSG_LEAVE:
            /* room actions need destination room */
            if (pkt->destination[0] == '\0') {
                return 0;
            }
            break;

        case MSG_NICK:
            /* requested nickname stored in message */
            if (pkt->message[0] == '\0') {
                return 0;
            }
            break;

        case MSG_WHO:
        case MSG_LIST:
        case MSG_QUIT:
            /* these can be empty */
            break;

        case MSG_SYSTEM:
        case MSG_ERROR:
            /* server-generated informational packets */
            if (pkt->message[0] == '\0') {
                return 0;
            }
            break;

        default:
            return 0;
    }

    return 1;
}