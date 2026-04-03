#include "../include/command.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void trim_newline(char *s){
    size_t len;

    if (s == NULL){
        return;
    }
    len = strlen(s);

    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')){
        s[len - 1] = '\0';
        len--;
    }
}

static void skip_spaces(const char **p){
    while (**p != '\0' && isspace((unsigned char)**p)) {
        (*p)++;
    }
}

void init_command(Command *cmd){
    if (cmd == NULL){
        return;
    }
    memset(cmd, 0, sizeof(Command));
    cmd->type = CMD_INVALID;
}

int parse_command(const char *input, Command *cmd){
    char buffer[MAX_MSG];
    const char *p;
    char *space;

    if (input == NULL || cmd == NULL){
        return -1;
    }
    init_command(cmd);

    strncpy(buffer, input, MAX_MSG -1);
    buffer[MAX_MSG - 1] = '\0';
    trim_newline(buffer);

    p = buffer;
    skip_spaces(&p);

    if (*p == '\0'){
        cmd->type = CMD_INVALID;
        return -1;
    }

     /* normal text message */
    if (*p != '/'){
        cmd->type = CMD_TEXT;
        strncpy(cmd->arg2, p, MAX_MSG -1);
        cmd->arg2[MAX_MSG - 1] = '\0';
        return 0;
    }

}
 if (strncmp(p, "/nick", 5) == 0 && (p[5] == '\0' || isspace((unsigned char)p[5]))) {
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

    if (strncmp(p, "/join", 5) == 0 && (p[5] == '\0' || isspace((unsigned char)p[5]))) {
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

    if (strncmp(p, "/leave", 6) == 0 && (p[6] == '\0' || isspace((unsigned char)p[6]))) {
        p += 6;
        skip_spaces(&p);

        cmd->type = CMD_LEAVE;

        if (*p != '\0') {
            strncpy(cmd->arg1, p, MAX_DEST - 1);
            cmd->arg1[MAX_DEST - 1] = '\0';
        }

        return 0;
    }

    if (strncmp(p, "/msg", 4) == 0 && (p[4] == '\0' || isspace((unsigned char)p[4]))) {
        p += 4;
        skip_spaces(&p);

        if (*p == '\0') {
            cmd->type = CMD_INVALID;
            return -1;
        }

        space = strchr(p, ' ');
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