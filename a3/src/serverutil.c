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
        strncat(pkt.message, "ERROR: ", MAX_MSG - 1);
    } else{   
        pkt.type = MSG_SYSTEM;
    }
    strncat(pkt.message, msg, MAX_MSG - strlen(pkt.message) - 1);
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
        strncat(pkt.message, msg, MAX_MSG - strlen(pkt.message) - 1);
        strncat(pkt.destination, clients[find_client_index(clients, num_clients, room->user_fds[i])].username, MAX_DEST - 1);
        strncat(pkt.usrid, "SERVER", MAX_USER - 1);
        pkt.timestamp = time(NULL);
        send_packet(room->user_fds[i], &pkt);
    }
}

void send_sys_msg_to_all(usr_data* clients, int is_error, int num_clients, char* msg){  

    for (int i = 0; i < num_clients; i++){    
        Packet pkt;
        memset(&pkt, 0, sizeof(Packet));
        if (is_error == 1){
            pkt.type = MSG_ERROR;
            strncat(pkt.message, "ERROR: ", MAX_MSG-1);
        } else{   
            pkt.type = MSG_SYSTEM;
        }
        strncat(pkt.message, msg, MAX_MSG - strlen(pkt.message) - 1);
        strncat(pkt.destination, clients[find_client_index(clients, num_clients, clients[i].fd)].username, MAX_DEST - 1);
        strncat(pkt.usrid, "SERVER", MAX_USER - 1);
        pkt.timestamp = time(NULL);
        send_packet(clients[i].fd, &pkt);
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

chatroom *create_new_room(chatroom **head, char* name){
    chatroom *new_room = malloc(sizeof(chatroom));
    memset(new_room, 0, sizeof(chatroom));
    strncat(new_room->room_id, name, MAX_DEST - 1);
    
    if(!(*head)){
        *head = new_room;
        return *head;
    }

    chatroom *curr = *head;

    while(curr->next){
        curr = curr->next;
    }
    curr->next = new_room;
    new_room->prev = curr;

    return new_room;
}
int close_room_if_empty(chatroom **head, chatroom *room){
    if (room->num_users != 0){
        return 1;
    }

    if(!room->prev){
        *head = room->next;
        return 0;
    }

    room->prev->next = room->next;
    free(room->room_id);
    free(room);
    return 0;
}


int process_dm(usr_data *clients, Packet *pkt, server_data *server){
    int recipient;
    if ((recipient = find_client_by_name(clients, server->num_clients, pkt->destination)) == -1 ){
        return -1;
    }
    int error = send_packet(clients[recipient].fd, pkt);
    return error;
}

int process_text(usr_data *clients, int num_user, int fd, chatroom **room_head, Packet *pkt, int num_rooms){
    int user_index = find_client_index(clients, num_user, fd);
    chatroom *destination = get_room_by_name(*room_head, clients[user_index].room_id);
    if (!destination) return -1;
    if(strlen(destination->room_id) == 0){
        return -1;
    }
    for (int i = 0; i < destination->num_users; i++){
        send_packet(destination->user_fds[i], pkt);
    }
    return 0;
}

int process_leave(usr_data *clients, int num_clients, chatroom **room_head, int fd){
    int i = find_client_index(clients, num_clients, fd);

    if (strlen(clients[i].room_id) <= 0){
        return -1;
    }
    chatroom *room = get_room_by_name(*room_head, clients[i].room_id);
    if (!room) return -1;

    char msg[MAX_MSG];
    memset(msg, 0, MAX_MSG);
    strncat(msg, clients[i].username, MAX_MSG - 1);
    strncat(msg, " left the room", MAX_MSG - 1 - strlen(msg));
    send_sys_msg_to_room(room, 0, clients, num_clients, msg);
    memset(clients[i].room_id, 0, MAX_DEST);
    remove_user_from_room(room, clients[i].fd);
    close_room_if_empty(room_head, room);
    return 0;
}

int process_join(usr_data* clients, int num_clients, chatroom **room_head, Packet *pkt, int fd){
    int client_index = find_client_index(clients, num_clients, fd);
    if (strlen(clients[client_index].room_id) > 0){
        process_leave(clients, num_clients, room_head, clients[client_index].fd);
    }
    
    chatroom *destination;
    if((destination = get_room_by_name(*room_head, pkt->destination)) == NULL){
        destination = create_new_room(room_head, pkt->destination);
    }

    add_user_to_room(destination, fd);
    strncpy(clients[client_index].room_id, destination->room_id, MAX_DEST);
    char message[MAX_MSG];
    memset(message, 0, MAX_DEST);
    snprintf(message, MAX_MSG, "%s joined room %s", clients[client_index].username, destination->room_id);
    send_sys_msg_to_room(destination, 0, clients, num_clients, message);

    return 0;
}

int process_nick(usr_data *clients, int num_clients, int fd, Packet *pkt){
    printf("Nick change Requested.\n");
    int user_index = find_client_index(clients, num_clients, fd);
    if (find_client_by_name(clients, num_clients, pkt->message) != -1){
        send_sys_msg(MSG_ERROR, pkt->usrid, fd, "DUPLICATE USERNAME");
        return -1;
    }
    strncpy(clients[user_index].username, pkt->message, MAX_USER);
    send_sys_msg(MSG_SYSTEM, pkt->usrid, fd, "USERNAME CHANGED");
    return 0;
}

int process_who(usr_data* clients, int fd, chatroom *room_head, int num_clients){
    char msg[MAX_MSG];
    memset(msg, 0, MAX_MSG);
    if(strlen(clients[find_client_index(clients, num_clients, fd)].room_id) == 0){
        strncat(msg, "Currently in server: ", MAX_MSG-1);
        for (int i = 0; i < num_clients; i++){
            strncat(msg, clients[i].username, MAX_DEST-strlen(msg));
            if (i < num_clients - 1) {
                strncat(msg, ", ", MAX_MSG - strlen(msg) - 1); // Add separator if not last
            }
        }
    }else{
        chatroom *room = get_room_by_name(room_head, clients[find_client_index(clients, num_clients, fd)].room_id);
        snprintf(msg, MAX_MSG-1, "Currently in room %s: ", room->room_id);
        for (int i = 0; i < room->num_users; i++){
            char *name = clients[find_client_index(clients, num_clients, room->user_fds[i])].username;
            strncat(msg, name, MAX_MSG - strlen(msg) - 1); 
            if (i < room->num_users - 1) {
                strncat(msg, ", ", MAX_DEST-strlen(msg)); // Add separator if not last
            }
        }
    }

    send_sys_msg(0, clients[find_client_index(clients, num_clients, fd)].username, fd, msg);
    return -1;
}

int process_list(usr_data* clients, int fd, chatroom *room_head, int num_clients){
    char msg[MAX_MSG];
    memset(&msg, 0, MAX_DEST);

    chatroom *curr = room_head;
    while(curr){
        strncat(msg, curr->room_id, MAX_MSG - 1);
            if (curr->next) {
                strncat(msg, ", ", MAX_DEST-strlen(msg)-1); // Add separator if not last
            }
        curr = curr->next;
    }
    send_sys_msg(0, clients[find_client_index(clients, num_clients, fd)].username, fd, msg);
    return 0;
}

int process_quit(fd_set *master_list, usr_data* clients, chatroom *rooms, int fd, server_data *server){
    int client_index = find_client_index(clients, server->num_clients, fd);
    if (strlen(clients[client_index].room_id) != 0){
        process_leave(clients, server->num_clients, room_head, fd);
    }
    char msg[MAX_MSG];
    memset(msg, 0, MAX_MSG);
    strncat(msg, clients[client_index].username, MAX_MSG-1);
    strncat(msg, " has disconnected from the server", MAX_MSG-strlen(msg)-1);
    FD_CLR(fd, master_list);
    remove_client_from_list(clients, clients[client_index], server);
    
    send_sys_msg_to_all(clients, 0, server->num_clients, msg);
    return 0;
}
