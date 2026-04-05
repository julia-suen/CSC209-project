#ifndef COMMON_H
#define COMMON_H
#include "../include/protocol.h"
#include <stddef.h>


/* user data */
typedef struct {
    int fd;
    char username[MAX_USER];
} usr_data;

/* chatroom */
typedef struct {
    int room_id;
    int num_users;
    int user_fds[MAX_USER];     // an array of int client_fd 
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

#endif