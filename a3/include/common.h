#ifndef COMMON_H
#define COMMON_H

#include "protocol.h"
#include <stddef.h>

typedef struct {
    int fd;
    char username[MAX_USER];
} usr_data;

typedef struct {
    int room_id;
    int num_users;
    int user_fds[MAX_USER];
} chatroom;

void trim_newline(char *s);
chatroom *rooms_set_up(int n);

int find_client_index(int *clients, int num_clients, int fd);
int find_client_by_name(usr_data *users, int num_users, const char *name);

int add_user_to_room(chatroom *room, int client_fd);
int remove_user_from_room(chatroom *room, int client_fd);

#endif