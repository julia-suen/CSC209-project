#include "../include/protocol.h"
#include "../include/common.h"
#include "../include/server.h"
#include "../include/serverutil.h"

int remove_client_from_list(usr_data *list, usr_data client, server_data *server){
    int index;
    if (index = find_client_by_name(list, server->num_clients, client.username) < 1){
        return -1;
    }
    for (int i = index; i < server->num_clients - 1; i++){
        list[i] = list[i+1];
    }
    server->clients--;
    return 0;
}


void add_to_pkt_list(int fd, pkt_node *pkt_list, Packet *pkt, server_data *server){
    pkt_node *new_node = malloc(sizeof(pkt_node));
    new_node->pkt = pkt;
    new_node->sender_fd = fd;
    if (!pkt_list){
        pkt_list = new_node;
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
    return;
}

void remove_pkt_and_deallocate(pkt_node *pkt_list, pkt_node *pkt, server_data *server){
    if (!pkt->prev){
        pkt_list = pkt->next;
        return;
    }
    pkt->prev->next = pkt->next;
    free(pkt->pkt);
    free(pkt);
    return;
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
    if (recipient = find_client_by_name(clients, server->num_clients, pkt->destination) == -1 ){
        return -1;
    }
    int error = send_packet(clients[recipient].fd, pkt);
    return error;
}

int process_text(chatroom *rooms, Packet *pkt, int num_rooms){
    int room_index = get_room_by_name(rooms, num_rooms, pkt->destination);
    if(room_index == -1){
        return -1;
    }
    chatroom destination = rooms[room_index];
    for (int i = 0; i < destination.num_users; i++){
        send_packet(destination.user_fds[i], pkt);
    }
    return 0;
}

int process_join(chatroom *rooms, Packet *pkt, int fd, int num_rooms){
    int room_index = get_room_by_name(rooms, num_rooms, pkt->destination);   
    if(room_index == -1){
        return -1;
    }
    chatroom destination = rooms[room_index];
    add_user_to_room(&destination, fd);
    return 0;
}

int process_nick(usr_data *clients, int num_clients, int fd, Packet *pkt){
    int user_index = find_client_index(clients, fd, num_clients);
    if (user_index == -1 || find_client_by_name(clients, num_clients, pkt->message) != -1){
        return -1;
    }
    strncpy(clients[user_index], pkt->message, MAX_USER);
    return 0;
}

int process_who(){
    return -1;
}

int process_list(){
    return -1;
}

int process_quit(chatroom *rooms, int num_rooms, int fd, Packet *pkt){
    int room_index = get_room_by_name(rooms, num_rooms, atoi(pkt->destination));
    remove_user_from_room(&rooms[room_index], fd);
    return 0;
}