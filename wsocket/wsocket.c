#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>

#include "wsocket.h"

int CreateTCPSocket(){
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))==0){
        perror("socket");
        exit(-1);
    }
    return sockfd;
}
int CreateUDPSocket(){
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))==0){
        perror("socket");
        exit(-1);
    }
    return sockfd;
}
void SetSocketOptions(int sockfd){
    int opt=1;
    // Set socket options to allow reuse of the address
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(-2);
    }
}
void BindSocket(int sockfd, char* ip, int port){
    struct sockaddr_in address;

    // Set address structure
    address.sin_family = AF_INET;
    if(ip == NULL){
        address.sin_addr.s_addr = INADDR_ANY;
    }else{
        inet_pton(AF_INET, ip, &(address.sin_addr));
    }
    address.sin_port = htons(port);

    if(bind(sockfd, (struct sockaddr*)&address, sizeof(address))<0){
        perror("bind");
        exit(-3);
    }
}
void ListenOnSocket(int sockfd, int backlog){
    if(listen(sockfd, backlog)<0){
        perror("listen");
        exit(-4);
    }
}

