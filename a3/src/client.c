#include <stdio.h>
#include <stdlib.h>
#include <protocol.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../include/protocol.h"
#include "../include/commands.h"
#include "../include/ui.h"

#define PORT 58086      // hardcode port number

// Prompt the user for a username right after connecting
// Return 0 if success, -1 if user quits/EOF
int get_username(char *username, int soc) {
    while (1) {
        printf("Enter your username: ");

        int n = read(STDIN_FILENO, username, MAX_USER - 1);
        if (n <= 0) { 
            fprintf(stderr, "EOF: exiting\n");
            return -1;  // exit
        }

        username[n] = '\0';
        trim_newline(username);

        if (strcmp(username, "/quit") == 0) {
            printf("Quitting.\n");
            return -1;  // exit
        }

        if (strlen(username) == 0) {
            printf("Invalid username, try again.\n");
            continue;
        }

        // send MSG_NICK types packet to server
        Packet pkt;
        init_packet(&pkt);
        pkt.type = MSG_NICK;
        strcpy(pkt.message, username);

        if (send_packet(soc, &pkt) < 0) {
            perror("send_packet");
            return -1;
        }

        // TODO: add server's check for duplicate username 

        break;  // exit loop when valid nickname packet sucessfully sent
    }

    return 0;
}


int handle_user_input(int soc, const char *buf) {
    // initialize command
    Command cmd;
    
    if (parse_command(buf, &cmd) != 0) {
        printf("Invalid command.\n");
        return 0;  // continue running
    }

    // debug
    // print_command(&cmd);

    if (cmd.type == CMD_QUIT) {
        close(soc);
        return 1;  // signal to exit
    }

    // initialize packet
    Packet pkt;
    init_packet(&pkt);

    // set packet according to the command type
    switch (cmd.type) {
        case CMD_TEXT:
            pkt.type = MSG_TEXT;
            strcpy(pkt.message, cmd.arg2);
            break;

        case CMD_MSG:
            pkt.type = MSG_PRIVATE;
            strcpy(pkt.destination, cmd.arg1);
            strcpy(pkt.message, cmd.arg2);
            break;

        case CMD_NICK:
            pkt.type = MSG_NICK;
            strcpy(pkt.message, cmd.arg1);      // put nickname in packet's message
            break;

        case CMD_JOIN:
            pkt.type = MSG_JOIN;
            strcpy(pkt.destination, cmd.arg1);
            break;

        case CMD_LEAVE:
            pkt.type = MSG_LEAVE;
            strcpy(pkt.destination, cmd.arg1);
            break;

        case CMD_WHO:
            pkt.type = MSG_WHO;
            break;

        case CMD_LIST:
            pkt.type = MSG_LIST;
            break;

        case CMD_HELP:
            pkt.type = MSG_HELP;
            break;

        default:
            printf("Invalid command.\n");
            return 0;
    }

    if (send_packet(soc, &pkt) < 0) {
        perrpr("send_packet");
        return -1;
    }

    return 0;
}


int main() {
    // create socket
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc == -1) {
        perror("client: socket");
        exit(1);
    }

    //initialize server address    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);  
    memset(&server.sin_zero, 0, 8);
    
    struct addrinfo *ai;
    char * hostname = "teach.cs.toronto.edu";

    // declares memory and populates ailist 
    getaddrinfo(hostname, NULL, NULL, &ai);
    // we only make use of the first element in the list 
    server.sin_addr = ((struct sockaddr_in *) ai->ai_addr)->sin_addr;

    // free the memory that was allocated by getaddrinfo for this list
    freeaddrinfo(ai);

    // connect to server
    int ret = connect(soc, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
    if (ret == -1){
        perror("connect");
        exit(1);
    }

    // ask user for username
    char username[MAX_USER];

    if (get_username(username, soc) < 0) {
        close(soc);
        exit(0);
    }

    // Send one packet test - no user input invovled 
    // Packet pkt;
    // init_packet(&pkt);
    // strcpy(pkt.message, "hello");
    // send_packet(soc, &pkt);

    fd_set read_fds;
    Packet pkt;
    char buf[512];

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);    // read user input from stdin
        FD_SET(soc, &read_fds);             // read message sent from server
        
        int num_fd;
        if (STDIN_FILENO > soc) {
            num_fd = STDIN_FILENO + 1;
        } else {
            num_fd = soc + 1;
        }
        
        // call select
        int ready = select(num_fd, &read_fds, NULL, NULL, NULL);

        if (ready < 0) {
            perror("select");
            exit(1);
        }

        // User input 
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            int n = read(STDIN_FILENO, buf, sizeof(buf) - 1);

            if (n == 0){
                close(soc);
                fprintf(stderr, "EOF: exiting\n");
                break;
            }
            if (n > 0) {
                buf[n] = '\0';
                
                if (handle_user_input(soc, buf) != 0) {   // if return 1, user quit 
                    break;
                }   
                // init_packet(&pkt);
                // strcpy(pkt.message, buf);

                // send_packet(soc, &pkt);
            }
        }

        // Server message 
        if (FD_ISSET(soc, &read_fds)) {
            int status = recv_packet(soc, &pkt);

            if (status <= 0) {
                fprintf(stderr, "Server disconnected.\n");
                break;
            }

            display_packet(&pkt);
        }
    }
    return 0;
}

