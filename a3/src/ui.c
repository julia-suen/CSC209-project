#include "../include/ui.h"

#include <stdio.h>

void print_system_message(const char *msg) {
    if (msg == NULL) {
        return;
    }
    printf("[SYSTEM] %s\n", msg);
}

void print_error_message(const char *msg) {
    if (msg == NULL) {
        return;
    }
    printf("[ERROR] %s\n", msg);
}

void print_chat_message(const char *user, const char *msg) {
    if (user == NULL || msg == NULL) {
        return;
    }
    printf("[%s] %s\n", user, msg);
}

void print_dm_message(const char *user, const char *msg) {
    if (user == NULL || msg == NULL) {
        return;
    }
    printf("[DM from %s] %s\n", user, msg);
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

void print_prompt(void) {
    printf("> ");
    fflush(stdout);
}

void display_packet(const Packet *pkt) {
    if (pkt == NULL) {
        return;
    }

    switch (pkt->type) {
        case MSG_TEXT:
            print_chat_message(pkt->usrid, pkt->message);
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
            print_system_message(pkt->message);
            break;

        case MSG_LEAVE:
            print_system_message(pkt->message);
            break;

        case MSG_NICK:
            print_system_message(pkt->message);
            break;

        case MSG_WHO:
            print_system_message(pkt->message);
            break;

        case MSG_LIST:
            print_system_message(pkt->message);
            break;

        case MSG_QUIT:
            print_system_message(pkt->message);
            break;

        default:
            printf("[TYPE %d] %s\n", pkt->type, pkt->message);
            break;
    }
}