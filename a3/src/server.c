#include "../include/protocol.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

void fd_check_readable(fd_set *read_fds, int server_fd, int *clients, int num_clients){
    int max_fd = server_fd;
    FD_SET(server_fd, read_fds);

    for (int i = 0; i < num_clients; i++)
    {
        FD_SET(clients[i], read_fds);
        if (clients[i] > max_fd){
            max_fd = clients[i];
        }
    }

    return select(max_fd + 1, read_fds, NULL, NULL, NULL);
}

void connect_new_client(fd_set *read_fds, int server_fd, int *clients, int *client_num){

    struct sockaddr_in new_client;
    new_client.sin_family = AF_INET;
    unsigned int client_len = sizeof(struct sockaddr_in);

    int new_client_fd = accept(server_fd, (struct sockaddr *) &new_client, &client_len);
   
    clients[*client_num] = new_client_fd;
    (*client_num)++;

    FD_SET(new_client_fd, read_fds);

    return;
}

void read_from_client(int client_fd){
    return;
}

void main(){
    // setup
    int server_fd;
    fd_set read_fds;
    // temp max connection of 16
    int client_fds[16];
    int num_clients = 0;
    FD_ZERO(&read_fds);

    struct sockaddr_in addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(1);

    }

    // bind to port
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(/*TODO: GET THE PORT LOL*/);
    memset(&(addr.sin_zero), 0, 8);

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1){
        perror('bind');
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 5) < 0){
        perror("listen");
        exit(1);
    }

    while(1){
        fd_check_readable(&read_fds, server_fd, client_fds, num_clients);
        if (FD_ISSET(server_fd, &read_fds)){
            connect_new_client(&read_fds, server_fd, client_fds, &num_clients);
        }
        for (int i = 0; i < num_clients; i++)
        {
            if (FD_ISSET(client_fds[i], &read_fds)){
                read_from_client(client_fds[i]);
            }
        }
    }

}