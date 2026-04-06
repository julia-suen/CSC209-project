#ifndef COMMON_H
#define COMMON_H

#include "protocol.h"
#include <stddef.h>


typedef struct chatroom{
    char room_id[MAX_DEST];
    int user_fds[MAX_USER];
    int num_users;
    struct chatroom *prev;
    struct chatroom *next;
} chatroom;

typedef struct {
    int fd;
    char username[MAX_USER];
    char room_id[MAX_DEST];
} usr_data;

void trim_newline(char *s);
chatroom *rooms_set_up(int n);

int find_client_index(usr_data *clients, int num_clients, int fd);
int find_client_by_name(usr_data *users, int num_users, const char *name);


int add_user_to_room(chatroom *room, int client_fd);
int remove_user_from_room(chatroom *room, int client_fd);

chatroom *get_room_by_name(chatroom *head, char* name);

#endif