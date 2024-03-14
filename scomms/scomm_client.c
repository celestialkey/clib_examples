#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "scomms.h"

void client(){
    struct sockaddr_un server_addr;
    int client_socket;
    char buffer[MAX_TX_LENGTH];
    ssize_t bytes_sent, bytes_received;

    // Create socket
    client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(-1);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, CONNECT_TARGET);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_socket);
        exit(-2);
    }

    while(1){
        printf("Cmd: ");
        if(fgets(buffer, MAX_TX_LENGTH, stdin)==NULL){
            break;
        }
        bytes_sent = send(client_socket, buffer, MAX_TX_LENGTH, 0);
        if(bytes_sent == -1){
            perror("send");
            break;
        }

        // Receive echoed message from server
        bytes_received = recv(client_socket, buffer, MAX_TX_LENGTH, 0);
        if (bytes_received == -1) {
            perror("recv");
            break;
        }
        printf("Echo from server: %s", buffer);
    }
    close(client_socket);
}