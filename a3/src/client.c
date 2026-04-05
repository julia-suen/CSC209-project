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

#define PORT 58086      # hardcode port number

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

    int ret = connect(soc, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
    if (ret == -1){
        perror("connect");
        exit(1);
    }

    // Send one packet test - no user input invovled 
    // Packet pkt;
    // init_packet(&pkt);
    // strcpy(pkt.message, "hello");
    // send_packet(soc, &pkt);

    // initialize pkt before loop
    // char buf[512];

    // while (1) {
    //     // Read user input
    //     int n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
    //     if (n > 0) {
    //         buf[n] = '\0';  // null terminate

    //         // TODO: maybe trim new line before sending
    //         init_packet(&pkt);
    //         strcpy(pkt.message, buf);

    //         send_packet(soc, &pkt);
    //     }
    // }

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
        
        // int max_fd = (soc > STDIN_FILENO ? soc : STDIN_FILENO) + 1;

        // call select
        int ready = select(num_fd, &read_fds, NULL, NULL, NULL);

        if (ready < 0) {
            perror("select");
            exit(1);
        }

        // User input 
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            int n = read(STDIN_FILENO, buf, sizeof(buf) - 1);

            if (n > 0) {
                buf[n] = '\0';
                // trim_newline(buf);   TODO: move trim_newline from commands to common?

                init_packet(&pkt);
                strcpy(pkt.message, buf);

                send_packet(soc, &pkt);
            }
        }

        // Server message 
        if (FD_ISSET(soc, &read_fds)) {
            int status = recv_packet(soc, &pkt);

            if (status <= 0) {
                printf("Server disconnected.\n");
                break;
            }

            print_packet(&pkt);  // TODO: change to ui.c's function to display message 
        }
    }
    return 0;
}

