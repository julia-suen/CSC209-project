#include "../include/ui.h"

#include <stdio.h>
#include <time.h>

static void get_time_str(char *buf, size_t size){
    time_t now = time(NULL);
    struct tm *tm_info;

    if (buf == NULL || size == 0){
        return;
    }
    tm_info = localtime(&now);
    if (tm_info == NULL){
        buf[0] = '\0';
        return;
    }
    strftime(buf, size, "%H:%M", tm_info);
}

void print_system_message(const char *msg) {
    char timebuf[6];

    if (msg == NULL) {
        return;
    }
    get_time_str(timebuf, sizeof(timebuf));
    printf("[%s] [SYSTEM] %s\n",timebuf, msg);
}

void print_error_message(const char *msg) {
    char timebuf[6];
    if (msg == NULL) {
        return;
    }
    get_time_str(timebuf, sizeof(timebuf));
    printf("[%s] [ERROR] %s\n",timebuf, msg);
}

void print_chat_message(const char *user, const char *room, const char *msg) {
    char timebuf[6];
    if (user == NULL || msg == NULL) {
        return;
    }
    get_time_str(timebuf, sizeof(timebuf));
    if (room != NULL && room[0] != '\0') {
        printf("[%s] [%s] %s: %s\n", timebuf, room, user, msg);
    } else {
        printf("[%s] %s: %s\n", timebuf, user, msg);
    }
}

void print_dm_message(const char *user, const char *msg) {
    char timebuf[6];
    if (user == NULL || msg == NULL) {
        return;
    }
    get_time_str(timebuf, sizeof(timebuf));
    printf("[%s] [DM from %s] %s\n", timebuf, user, msg);
}

void print_help_menu(void) {
    printf("Available commands:\n");
    printf("  /nick <username>\n");
    printf("  /join <room>\n");
    printf("  /leave <room>\n");
    printf("  /msg <user> <message>\n");
    printf("  /who\n");
    printf("  /list\n");
    printf("  /quit\n");
    printf("  /help\n");
}

void print_prompt(const char *user, const char *room) {
    if (user != NULL && room != NULL &&
        user[0] != '\0' && room[0] != '\0') {
        printf("[%s@%s] > ", user, room);
    } else if (user != NULL && user[0] != '\0') {
        printf("[%s] > ", user);
    } else {
        printf("> ");
    }

    fflush(stdout);

}

void display_packet(const Packet *pkt) {
    if (pkt == NULL) {
        return;
    }

    switch (pkt->type) {
        case MSG_TEXT:
            print_chat_message(pkt->usrid, pkt->destination, pkt->message);
            break;

        case MSG_DM:
            print_dm_message(pkt->usrid, pkt->message);
            break;

        case MSG_SYSTEM:
            print_system_message(pkt->message);
            break;

        case MSG_ERROR:
            print_error_message(pkt->message);
            break;

        case MSG_JOIN:
        case MSG_LEAVE:
        case MSG_NICK:
        case MSG_WHO:
        case MSG_LIST:
        case MSG_QUIT:
            print_system_message(pkt->message);
            break;

        default:
            printf("[UNKNOWN TYPE %d] %s\n", pkt->type, pkt->message);
            break;
    }
}