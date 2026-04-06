#include "../include/common.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static void trim_newline(char *s) {
    size_t len;

    if (s == NULL) {
        return;
    }

    len = strlen(s);

    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

chatroom* rooms_set_up (int n){
    chatroom *rooms = malloc(sizeof(chatroom) * n);
    if (!rooms) return NULL;

    for (int i = 0; i < n; i ++){
        rooms[i].room_id = i;
        rooms[i].num_users = 0;
    }
    return rooms;
}

int find_client_index(int *clients, int num_clients, int fd){
    for (int i = 0; i < num_clients; i++){
        if (clients[i] == fd){
            return i;
        }
    }
    return -1;
}

int find_client_by_name(usr_data *users, int num_users, const char *name){
    for (int i = 0; i < num_users; i++){
        if (strcmp(users[i].username, name) == 0){
            return i;
        }
    }
    return -1;
}

int add_user_to_room(chatroom *room, int client_fd) {
    if (!room) return -1;

    if (room->num_users >= MAX_USER) {
        return -1; // room full
    }

    room->user_fds[room->num_users] = client_fd;
    room->num_users++;

    return 0;
}

int remove_user_from_room(chatroom *room, int client_fd) {
    if (!room) return -1;

    int index = -1;

    for (int i = 0; i < room->num_users; i++) {
        if (room->user_fds[i] == client_fd) {
            index = i;
            break;
        }
    }

    if (index == -1) return -1;

    // shift left
    for (int i = index; i < room->num_users - 1; i++) {
        room->user_fds[i] = room->user_fds[i + 1];
    }

    room->num_users--;

    return 0;
}


int get_room_by_name(chatroom *list, int num_room, char* name){
    int room_name = atoi(name);
    for (int i = 0; i < num_room; i++){
        if (room_name == list[i].room_id){
            return i;
        }
    }
    return -1;
}