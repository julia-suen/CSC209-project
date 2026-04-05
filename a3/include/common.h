#ifndef COMMON_H
#define COMMON_H
#include "../include/protocol.h"

typedef struct {
    int room_id;
    int num_users;
    int *user_fds[MAX_USER];
} chatroom;

#endif