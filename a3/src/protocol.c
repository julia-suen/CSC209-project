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
            return 1;  // client disconnected
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
    pkt->timestamp = time(NULL);   // real timestamp
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