#include "../include/commands.h"
#include "../include/common.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>


static void skip_spaces(const char **p) {
    while (**p != '\0' && isspace((unsigned char)**p)) {
        (*p)++;
    }
}

void init_command(Command *cmd) {
    if (cmd == NULL) {
        return;
    }

    memset(cmd, 0, sizeof(Command));
    cmd->type = CMD_INVALID;
}

int parse_command(const char *input, Command *cmd) {
    char buffer[MAX_MSG];
    const char *p;
    char *space;

    if (input == NULL || cmd == NULL) {
        return -1;
    }

    init_command(cmd);

    strncpy(buffer, input, MAX_MSG - 1);
    buffer[MAX_MSG - 1] = '\0';
    trim_newline(buffer);

    p = buffer;
    skip_spaces(&p);

    if (*p == '\0') {
        cmd->type = CMD_INVALID;
        return -1;
    }

    /* normal text message */
    if (*p != '/') {
        cmd->type = CMD_TEXT;
        strncpy(cmd->arg2, p, MAX_MSG - 1);
        cmd->arg2[MAX_MSG - 1] = '\0';
        return 0;
    }

    if (strncmp(p, "/nick", 5) == 0 &&
        (p[5] == '\0' || isspace((unsigned char)p[5]))) {
        p += 5;
        skip_spaces(&p);

        if (*p == '\0') {
            cmd->type = CMD_INVALID;
            return -1;
        }

        cmd->type = CMD_NICK;
        strncpy(cmd->arg1, p, MAX_DEST - 1);
        cmd->arg1[MAX_DEST - 1] = '\0';
        return 0;
    }

    if (strncmp(p, "/join", 5) == 0 &&
        (p[5] == '\0' || isspace((unsigned char)p[5]))) {
        p += 5;
        skip_spaces(&p);

        if (*p == '\0') {
            cmd->type = CMD_INVALID;
            return -1;
        }

        cmd->type = CMD_JOIN;
        strncpy(cmd->arg1, p, MAX_DEST - 1);
        cmd->arg1[MAX_DEST - 1] = '\0';
        return 0;
    }

    if (strncmp(p, "/leave", 6) == 0 &&
        (p[6] == '\0' || isspace((unsigned char)p[6]))) {
        p += 6;
        skip_spaces(&p);

        cmd->type = CMD_LEAVE;

        if (*p != '\0') {
            strncpy(cmd->arg1, p, MAX_DEST - 1);
            cmd->arg1[MAX_DEST - 1] = '\0';
        }

        return 0;
    }

    if (strncmp(p, "/msg", 4) == 0 &&
        (p[4] == '\0' || isspace((unsigned char)p[4]))) {
        p += 4;
        skip_spaces(&p);

        if (*p == '\0') {
            cmd->type = CMD_INVALID;
            return -1;
        }

        space = strchr((char *)p, ' ');
        if (space == NULL) {
            cmd->type = CMD_INVALID;
            return -1;
        }

        *space = '\0';
        strncpy(cmd->arg1, p, MAX_DEST - 1);
        cmd->arg1[MAX_DEST - 1] = '\0';

        p = space + 1;
        skip_spaces(&p);

        if (*p == '\0') {
            cmd->type = CMD_INVALID;
            return -1;
        }

        cmd->type = CMD_MSG;
        strncpy(cmd->arg2, p, MAX_MSG - 1);
        cmd->arg2[MAX_MSG - 1] = '\0';
        return 0;
    }

    if (strcmp(p, "/who") == 0) {
        cmd->type = CMD_WHO;
        return 0;
    }

    if (strcmp(p, "/list") == 0) {
        cmd->type = CMD_LIST;
        return 0;
    }

    if (strcmp(p, "/quit") == 0) {
        cmd->type = CMD_QUIT;
        return 0;
    }

    if (strcmp(p, "/help") == 0) {
        cmd->type = CMD_HELP;
        return 0;
    }

    cmd->type = CMD_INVALID;
    return -1;
}

int command_to_packet(const Command *cmd, Packet *pkt,
                      const char *usrid, const char *current_room) {
    if (cmd == NULL || pkt == NULL || usrid == NULL) {
        return -1;
    }

    init_packet(pkt);

    strncpy(pkt->usrid, usrid, MAX_USER - 1);
    pkt->usrid[MAX_USER - 1] = '\0';

    switch (cmd->type) {
        case CMD_TEXT:
            pkt->type = MSG_TEXT;

            if (current_room != NULL) {
                strncpy(pkt->destination, current_room, MAX_DEST - 1);
                pkt->destination[MAX_DEST - 1] = '\0';
            }

            strncpy(pkt->message, cmd->arg2, MAX_MSG - 1);
            pkt->message[MAX_MSG - 1] = '\0';
            return 0;

        case CMD_NICK:
            pkt->type = MSG_NICK;
            strncpy(pkt->message, cmd->arg1, MAX_MSG - 1);
            pkt->message[MAX_MSG - 1] = '\0';
            return 0;

        case CMD_JOIN:
            pkt->type = MSG_JOIN;
            strncpy(pkt->destination, cmd->arg1, MAX_DEST - 1);
            pkt->destination[MAX_DEST - 1] = '\0';
            return 0;

        case CMD_LEAVE:
            pkt->type = MSG_LEAVE;

            if (cmd->arg1[0] != '\0') {
                strncpy(pkt->destination, cmd->arg1, MAX_DEST - 1);
                pkt->destination[MAX_DEST - 1] = '\0';
            } else if (current_room != NULL) {
                strncpy(pkt->destination, current_room, MAX_DEST - 1);
                pkt->destination[MAX_DEST - 1] = '\0';
            }

            return 0;

        case CMD_MSG:
            pkt->type = MSG_DM;
            strncpy(pkt->destination, cmd->arg1, MAX_DEST - 1);
            pkt->destination[MAX_DEST - 1] = '\0';

            strncpy(pkt->message, cmd->arg2, MAX_MSG - 1);
            pkt->message[MAX_MSG - 1] = '\0';
            return 0;

        case CMD_WHO:
            pkt->type = MSG_WHO;
            return 0;

        case CMD_LIST:
            pkt->type = MSG_LIST;
            return 0;

        case CMD_QUIT:
            pkt->type = MSG_QUIT;
            return 0;

        case CMD_HELP:
            return -1;

        case CMD_INVALID:
        default:
            return -1;
    }
}

void print_command(const Command *cmd) {
    if (cmd == NULL) {
        return;
    }

    printf("----- Command Debug -----\n");
    printf("type: %d\n", cmd->type);
    printf("arg1: %s\n", cmd->arg1);
    printf("arg2: %s\n", cmd->arg2);
    printf("-------------------------\n");
}