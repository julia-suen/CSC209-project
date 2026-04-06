#ifndef COMMON_H
#define COMMON_H
#include "../include/protocol.h"
#include <stddef.h>

/* shared constants */
#define MAX_USER 32
#define MAX_MSG 512
#define MAX_DEST 64
/* user data */
typedef struct {
    int fd;
    char username[MAX_USER];
} usr_data;

typedef struct {
    int room_id;
    int num_users;
    int *user_fds[MAX_USER];
} chatroom;

/* utility functions */

/* remove newline from string */
void trim_newline(char *s);

/* initialize rooms */
chatroom *rooms_set_up(int n);

/* find client index by fd */
int find_client_index(int *clients, int num_clients, int fd);

/* find client by username */
int find_client_by_name(usr_data *users, int num_users, const char *name);

/* add/remove user from room*/

int add_user_to_room(chatroom *room, int client_fd);

int remove_user_from_room(chatroom *room, int client_fd);

int get_room_by_name(chatroom *list, int num_room, char* name);

#endif