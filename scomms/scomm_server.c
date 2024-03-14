#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "scomms.h"


void server(){
    struct sockaddr_un server_addr, client_addr;
    int server_socket, client_socket;
    
    socklen_t client_len;
    ssize_t bytes_received;

    char buffer[MAX_TX_LENGTH];

    // Create the comms socket
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("socket");
        exit(-1);
    }

     // Setup the server address
     memset(&server_addr, 0, sizeof(server_addr));
     server_addr.sun_family = AF_UNIX;
     strcpy(server_addr.sun_path, CONNECT_TARGET);

     // Bind to the socket
     if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))==-1){
        perror("bind");
        close(server_socket);
        exit(-2);
     }

     // Listen for connections
     if(listen(server_socket, 5) == -1){
        perror("listen");
        close(server_socket);
        exit(-3);
     }

     printf("Server listening on %s.\n", CONNECT_TARGET);

     // Accept connections
     while(1){
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if(client_socket == -1){
            perror("accept");
            close(server_socket);
            exit(-4);
        }

        printf("Client connected.\n");

        // Receive data from the client
        while((bytes_received = recv(client_socket, buffer, MAX_TX_LENGTH, 0))>0){
            // Echo data
            if(send(client_socket, buffer, bytes_received, 0)==-1){
                perror("send");
                close(client_socket);
                close(server_socket);
                exit(-5);
            }
        }

        if(bytes_received==-1){
            perror("send");
            close(client_socket);
            close(server_socket);
            exit(-6);
        }

        // Close connection
        printf("Client disconnected\n");
        close(client_socket);
        break;
     }

     // Shutdown server socket
     close(server_socket);

     // remove socket file (unbind)
     unlink(CONNECT_TARGET);
}