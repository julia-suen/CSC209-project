#include "../include/protocol.h"
#include "../include/server.h"

void remove_client_from_list(int *client_fds, int client_fd, int *num_clients){
    int i;
    for (i = 0; i < num_clients; i++){
        if (client_fds[i] == client_fd){
            break;
        }
    }
    for (i; i< num_clients - 1; i++){
        client_fds[i] = client_fds[i+1];
    }
    (*num_clients)--;
}


void add_to_pkt_list(pkt_node *pkts, Packet *pkt, server_data *server){
    pkt_node *new_node = malloc(sizeof(pkt_node));
    new_node->pkt = pkt;

    if (!pkts){
        pkts = new_node;
        server-> num_packets = 1;
    }
    else{
        while(pkts->next != NULL){
            pkts = pkts->next;
        }
        new_node->prev = pkts;
        pkts->next = new_node;
        server->num_packets++;
    }
    return;
}

void remove_pkt_and_deallocate(pkt_node *pkt, server_data *server){
    pkt->prev->next = pkt->next;
    free(pkt->pkt);
    free(pkt);
    return
}

void process_dm(int* clients, Packet* pkt){
    return;
}

void process_text(int* clients, Packet* pkt){
    return;
}