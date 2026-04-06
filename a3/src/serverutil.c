#include "../include/protocol.h"
#include "../include/common.h"
#include "../include/server.h"
#include "../include/serverutil.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void send_sys_msg(int is_error, char* name, int fd, char* msg){
    Packet pkt;
    memset(&pkt, 0, sizeof(Packet));
    if (is_error == 1){
        pkt.type = MSG_ERROR;
        strncat(pkt.message, "ERROR: ", MAX_MSG-1);
    } else{   
        pkt.type = MSG_SYSTEM;
    }
    strncat(pkt.message, msg, MAX_MSG - strlen(pkt.message));
    strncat(pkt.destination, name, MAX_DEST - 1);
    strncat(pkt.usrid, "SERVER", MAX_USER - 1);
    pkt.timestamp = time(NULL);

    send_packet(fd, &pkt);
}

void send_sys_msg_to_room(chatroom* room, int is_error, usr_data* clients, int num_clients, char* msg){  

    for (int i = 0; i < room->num_users; i++){    
        Packet pkt;
        memset(&pkt, 0, sizeof(Packet));
        if (is_error == 1){
            pkt.type = MSG_ERROR;
            strncat(pkt.message, "ERROR: ", MAX_MSG-1);
        } else{   
            pkt.type = MSG_SYSTEM;
        }
        strncat(pkt.message, msg, MAX_MSG - strlen(pkt.message));
        strncat(pkt.destination, clients[find_client_index(clients, num_clients, room->user_fds[i])].username, MAX_DEST - 1);
        strncat(pkt.usrid, "SERVER", MAX_USER - 1);
        pkt.timestamp = time(NULL);
        send_packet(room->user_fds[i], &pkt);
    }
}

int remove_client_from_list(usr_data *list, usr_data client, server_data *server){
    int index;
    if ((index = find_client_by_name(list, server->num_clients, client.username)) < 0){
        return -1;
    }
    for (int i = index; i < server->max_clients - 1; i++){
        list[i] = list[i+1];
    }
    server->num_clients--;
    return 0;
}


pkt_node *add_to_pkt_list(int fd, pkt_node *pkt_list, Packet *pkt, server_data *server){
    pkt_node *new_node = malloc(sizeof(pkt_node));
    new_node->pkt = pkt;
    new_node->sender_fd = fd;
    if (!pkt_list){
        return new_node;
        server-> num_packets = 1;
    }
    else{
        while(pkt_list->next != NULL){
            pkt_list = pkt_list->next;
        }
        new_node->prev = pkt_list;
        pkt_list->next = new_node;
        server->num_packets++;
    }
    return pkt_list;
}

pkt_node *remove_pkt_and_deallocate(pkt_node *pkt_list, pkt_node *pkt, server_data *server_data){
    if (!pkt->prev){
        server_data->num_packets--;
        return pkt->next;
    }
    pkt->prev->next = pkt->next;
    free(pkt->pkt);
    free(pkt);
    server_data->num_packets--;
    return pkt_list;
}

// usr_data *get_client_by_id(int id, usr_node *list){
//     if(!list) return NULL;

//     usr_node *curr = list;
//     while(curr){
//         if (curr->data->usr_id == id){
//             return curr->data;
//         }
//     }

//     return NULL;
// }

int process_dm(usr_data *clients, Packet *pkt, server_data *server){
    int recipient;
    if ((recipient = find_client_by_name(clients, server->num_clients, pkt->destination)) == -1 ){
        return -1;
    }
    int error = send_packet(clients[recipient].fd, pkt);
    return error;
}

int process_text(chatroom *rooms, Packet *pkt, int num_rooms){
    int room_index = get_room_by_name(rooms, num_rooms, (int) strtol(pkt->destination, NULL, 10));
    if(room_index == -1){
        return -1;
    }
    chatroom destination = rooms[room_index];
    for (int i = 0; i < destination.num_users; i++){
        send_packet(destination.user_fds[i], pkt);
    }
    return 0;
}

int process_leave(chatroom *rooms, usr_data *clients, int fd, int num_clients){
    int i = find_client_index(clients, num_clients, fd);
    int room_id = clients[i].room_id;

    char msg[MAX_MSG];
    memset(msg, 0, MAX_MSG);
    strncat(msg, clients[i].username, MAX_MSG - 1);
    strncat(msg, " left the room", MAX_MSG - 1 - strlen(msg));
    send_sys_msg_to_room(&rooms[room_id], 0, clients, num_clients, msg);

    (&clients[i])->room_id = -1;
    remove_user_from_room(&rooms[room_id], clients[i].fd);
    return 0;
}

int process_join(usr_data* clients, int num_client, chatroom *rooms, Packet *pkt, int fd, int num_rooms){
    int room_index = get_room_by_name(rooms, num_rooms, (int) strtol(pkt->destination, NULL, 10));   
    if(room_index == -1){
        send_sys_msg(MSG_ERROR, pkt->usrid, fd, "ROOM DOES NOT EXIST");
        return -1;
    }
    chatroom *destination = &rooms[room_index];
    add_user_to_room(destination, fd);
    int client_index = find_client_index(clients, num_client, fd);
    (&clients[client_index])->room_id = room_index;
    char message[MAX_MSG];
    memset(message, 0, MAX_DEST);
    snprintf(message, MAX_MSG, "%s joined room %d", clients[client_index].username, rooms[room_index].room_id);
    send_sys_msg_to_room(&rooms[room_index], 0, clients, num_client, message);

    return 0;
}

int process_nick(usr_data *clients, int num_clients, int fd, Packet *pkt){
    printf("Nick change Requested.");
    int user_index = find_client_index(clients, num_clients, fd);
    if (find_client_by_name(clients, num_clients, pkt->message) != -1){
        send_sys_msg(MSG_ERROR, pkt->usrid, fd, "DUPLICATE USERNAME");
        return -1;
    }
    strncpy(clients[user_index].username, pkt->message, MAX_USER);
    send_sys_msg(MSG_SYSTEM, pkt->usrid, fd, "USERNAME CHANGED");
    return 0;
}

int process_who(usr_data* clients, int fd, chatroom *rooms, server_data *server){
    char msg[MAX_MSG];
    memset(msg, 0, MAX_MSG);
    if((clients[find_client_index(clients, server->num_clients, fd)].room_id) == -1){
        strncat(msg, "Currently in server: ", MAX_MSG-1);
        for (int i = 0; i < server->num_clients; i++){
            strncat(msg, clients[i].username, MAX_DEST-strlen(msg));
            if (i < server->num_clients - 1) {
                strncat(msg, ", ", MAX_DEST-strlen(msg)); // Add separator if not last
            }
        }
    }else{
        chatroom room = rooms[clients[find_client_index(clients, server->num_clients, fd)].room_id];
        snprintf(msg, MAX_MSG-1, "Currently in room %d: ", room.room_id);
        for (int i = 0; i < room.num_users; i++){
            char *name = clients[find_client_index(clients, server->num_clients, room.user_fds[i])].username;
            strncat(msg, name, MAX_MSG - strlen(msg)); 
            if (i < room.num_users - 1) {
                strncat(msg, ", ", MAX_DEST-strlen(msg)); // Add separator if not last
            }
        }
    }

    send_sys_msg(0, clients[find_client_index(clients, server->num_clients, fd)].username, fd, msg);
    return -1;
}

int process_list(usr_data* clients, int fd, chatroom *rooms, server_data *server){
    char msg[MAX_MSG];
    char buffer[MAX_MSG];
    memset(&msg, 0, MAX_DEST);

    for (int i = 0; i < server->num_rooms; i++){
        snprintf(buffer, MAX_MSG, "%d", rooms[i].room_id);
        strncat(msg, buffer, MAX_DEST-strlen(msg));
            if (i < server->num_rooms - 1) {
                strncat(msg, ", ", MAX_DEST-strlen(msg)); // Add separator if not last
            }
    }
    send_sys_msg(0, clients[find_client_index(clients, server->num_clients, fd)].username, fd, msg);
    return 0;
}

int process_quit(chatroom *rooms, int num_rooms, int fd, Packet *pkt){
    int room_index = get_room_by_name(rooms, num_rooms, (int) strtol(pkt->destination, NULL, 10));
    remove_user_from_room(&rooms[room_index], fd);
    return 0;
}
