//#define USE_FD_SET
#define USE_EPOLL

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#ifdef USE_FD_SET
#include <sys/select.h>
#elif defined(USE_EPOLL)
#include <sys/epoll.h>
#endif

#include "../ringbuffer/ringbuffer.h"
#include "commlinks.h"

int CreateTCPSocket();
int CreateUDPSocket();
void SetSocketOptions(int sockfd);
void BindSocket(int sockfd, char* ip, int port);
void ListenOnSocket(int sockfd, int backlog);

int ParseArgs(int argc, char* argv[]);

#ifdef USE_FD_SET

typedef struct {
    fd_set  set;
    int     maxfd;
} FDSetControls;

void ClearFDSet(FDSetControls* fdctrl);
void AddToFDSet(FDSetControls* fdctrl, int sockfd);
int PendingFDSet(FDSetControls* fdctrl);
int QueryFDSet(FDSetControls* fdctrl, int sockfd);
#elif defined(USE_EPOLL)
int CreateEpoll();
uint8_t AddSocketToEpoll(int epollfd, int sockfd, uint32_t flags);
uint8_t RemoveSocketFromEpoll(int epollfd, int sockfd);
int QueryEpoll(int epollfd, struct epoll_event* events);
#endif


//=====================================================================
//      Main Example Start
//=====================================================================
struct {
    uint8_t mode;
} args;

int ReadDataFromServerUDP(CommLink* link){
    int serverUDPLen = sizeof(link->address);
    int readLength = RBGetFree(&link->pendingRead);
    char buffer[readLength+1];
    int bytesRead = 0;
    if((bytesRead = recvfrom(link->socketfd, buffer, readLength, 0, (struct sockaddr*)&link->address, (socklen_t*)&serverUDPLen))==-1){
        perror("recvfrom");
        return 0;
    }
    RBWrite(&link->pendingRead, buffer, bytesRead);
    return 1;
}
int ReadDataFromTCPSocket(CommLink* link){
    int readLength = RBGetFree(&link->pendingRead);
    
    if(!readLength) return 0;
    char buffer[readLength+1];
    
    int bytesRead = recv(link->socketfd, buffer, readLength, 0);
    if(!bytesRead) {
        return -1;
    } else if (bytesRead == -1){
        perror("recv");
        return -1;
    }

    RBWrite(&link->pendingRead, buffer, bytesRead);
    return 1;
}

#define MAX_CLIENTS 20
int FindLinkFromSockFd(CommLink* clientlist, int sockfd){
    for(int i=0; i<MAX_CLIENTS; i++){
        if(clientlist[i].socketfd == sockfd)
            return i;
    }
    return -1;
}
void HandleInboundClient(CommLink* clientlist, int sockfd){
    // Add the new client if there's a slot, close it if not.
    for(int i=0; i<MAX_CLIENTS; i++){
        if(!clientlist[i].socketfd){
            clientlist[i].socketfd = sockfd;
            sockfd = 0;
            break;
        }
    }
    if(sockfd) close(sockfd);
}

#ifdef USE_FD_SET

void HandleClients_FDSET(CommLink* serverTCP, CommLink* serverUDP){
    int serverTCPLen = sizeof(serverTCP->address);
    

    int acceptClientSocket;
    FDSetControls fds;
    CommLink clientLinks[MAX_CLIENTS] = {0};
    while(1){
        // Clear the list
        ClearFDSet(&fds);
        // Add the server's sockets to the list
        AddToFDSet(&fds, serverTCP->socketfd);
        AddToFDSet(&fds, serverUDP->socketfd);

        // Add all active clients to the list
        for(int i=0; i<MAX_CLIENTS; i++){
            if(!clientLinks[i].socketfd) continue;
            AddToFDSet(&fds, clientLinks[i].socketfd);
        }

        // Wait for activity on the socket
        if(!PendingFDSet(&fds)) continue;

        // Check for pending TCP data
        if(QueryFDSet(&fds, serverTCP->socketfd)){
            if((acceptClientSocket = accept(serverTCP->socketfd, (struct sockaddr*)&serverTCP->address, (socklen_t*)&serverTCPLen))<0){
                perror("accept");
                exit(-4);
            }
            HandleInboundClient(clientLinks, acceptClientSocket);
        }

        // Check for pending UDP data
        if(QueryFDSet(&fds, serverUDP->socketfd)){
            if(!ReadDataFromServerUDP(serverUDP)){
                printf("error udp\n");
            }
        }
        // Check for active client data
        for(int i=0; i<MAX_CLIENTS; i++){
            if(!clientLinks[i].socketfd) continue;
            if(!QueryFDSet(&fds, clientLinks[i].socketfd)) continue;
            if(!ReadDataFromTCPSocket(&clientLinks[i])){
                close(clientLinks[i].socketfd);
                CLInitCommLink(&clientLinks[i]);
            }
        }

        //===================================
        //  Everything below here is 
        //      acting on ringbuffer data
        //      that was stored earlier.
        //===================================
        // Dump data to screen
        for(int i=0; i<MAX_CLIENTS; i++){
            if(!clientLinks[i].socketfd) continue;
            if(!RBHasData(&clientLinks[i].pendingRead)) continue;

            int readableData = RBGetPending(&clientLinks[i].pendingRead);
            char buffer[readableData+1];
            RBRead(&clientLinks[i].pendingRead, buffer, readableData);
            buffer[readableData] = 0;
            printf("READ: %s\n", buffer);
        }

        // Read commands off command socket
        if(!RBHasData(&serverUDP->pendingRead)) continue;
        int readableData = RBGetPending(&serverUDP->pendingRead);
        char buffer[readableData+1];
        RBRead(&serverUDP->pendingRead, buffer, readableData);
        buffer[readableData] = 0;
        printf("CMD: %s\n", buffer);
    }
}

#elif defined(USE_EPOLL)
#define MAX_EVENTS  20
void HandleClients_EPOLL(CommLink* serverTCP, CommLink* serverUDP){
    int epollfd = CreateEpoll();
    if(!epollfd) return;

    struct epoll_event events[MAX_EVENTS];
    CommLink clientLinks[MAX_CLIENTS] = {0};
    int nfds, acceptClientSocket;
    int addressLength = sizeof(serverTCP->address);

    // Add server sockets to the list
    AddSocketToEpoll(epollfd, serverTCP->socketfd, EPOLLIN);
    AddSocketToEpoll(epollfd, serverUDP->socketfd, EPOLLIN);

    while(1){
        // Check for events
        nfds = QueryEpoll(epollfd, events);
        if(!nfds) break;

        // Loop through events pending action
        for(int i=0; i<nfds; i++){
            int pendingSocket = events[i].data.fd;
            // Check if the event is related to the server TCP socket
            if(pendingSocket == serverTCP->socketfd){
                if((acceptClientSocket = accept(serverTCP->socketfd, (struct sockaddr*)&serverTCP->address, (socklen_t*)&addressLength))<0){
                    perror("accept");
                    exit(-4);
                }
                HandleInboundClient(clientLinks, acceptClientSocket);
                AddSocketToEpoll(epollfd, acceptClientSocket, EPOLLIN);
                continue; // Move on to the next event
            }
            
            // check if the event is related to the server UDP socket
            if(pendingSocket == serverUDP->socketfd){
                if(!ReadDataFromServerUDP(serverUDP)){
                    printf("error udp\n");
                }
                continue; // Move on to the next event
            }

            // Attempt to find the client with a pending event
            int index = FindLinkFromSockFd(clientLinks, pendingSocket);
            if(index == -1) {
                // Unknown client, close the socket
                RemoveSocketFromEpoll(epollfd, pendingSocket);
                close(pendingSocket);
                continue; // Move on to the next event
            }
            // Read data for the client of interest.
            if(!ReadDataFromTCPSocket(&clientLinks[index])){
                RemoveSocketFromEpoll(epollfd, clientLinks[index].socketfd);
                close(clientLinks[index].socketfd);
                CLInitCommLink(&clientLinks[index]);
            }
        }

        //===================================
        //  Everything below here is 
        //      acting on ringbuffer data
        //      that was stored earlier.
        //===================================
        // Dump data to screen
        for(int i=0; i<MAX_CLIENTS; i++){
            if(!clientLinks[i].socketfd) continue;
            if(!RBHasData(&clientLinks[i].pendingRead)) continue;

            int readableData = RBGetPending(&clientLinks[i].pendingRead);
            char buffer[readableData+1];
            RBRead(&clientLinks[i].pendingRead, buffer, readableData);
            buffer[readableData] = 0;
            printf("READ: %s\n", buffer);
        }

        // Read commands off command socket
        if(!RBHasData(&serverUDP->pendingRead)) continue;
        int readableData = RBGetPending(&serverUDP->pendingRead);
        char buffer[readableData+1];
        RBRead(&serverUDP->pendingRead, buffer, readableData);
        buffer[readableData] = 0;
        printf("CMD: %s\n", buffer);
    }
}

#endif

int main(){
    // Setup server tcp/udp sockets
    CommLink serverTCP, serverUDP;
    serverTCP.socketfd = CreateTCPSocket();
    serverUDP.socketfd = CreateUDPSocket();
    SetSocketOptions(serverTCP.socketfd);
    SetSocketOptions(serverUDP.socketfd);
    BindSocket(serverTCP.socketfd, "127.0.0.1", 5555);
    BindSocket(serverUDP.socketfd, "127.0.0.1", 5554);
    ListenOnSocket(serverTCP.socketfd, 10);

    // Do stuff
    #ifdef USE_FD_SET
    HandleClients_FDSET(&serverTCP, &serverUDP);
    #elif defined(USE_EPOLL)
    HandleClients_EPOLL(&serverTCP, &serverUDP);
    #endif

    close(serverTCP.socketfd);
    close(serverUDP.socketfd);
    return 0;
}


#ifdef USE_FD_SET

void ClearFDSet(FDSetControls* fdctrl){
    FD_ZERO(&fdctrl->set);
    fdctrl->maxfd = 0;
}
void AddToFDSet(FDSetControls* fdctrl, int sockfd){
    FD_SET(sockfd, &fdctrl->set);
    if(sockfd > fdctrl->maxfd){
        fdctrl->maxfd = sockfd;
    }
}
int PendingFDSet(FDSetControls* fdctrl){
    int ret = select(fdctrl->maxfd+1, &fdctrl->set, NULL, NULL, NULL);
    if((ret < 0) && (errno != EINTR)){
        perror("select");
        return 0;
    }
    return ret;
}
int QueryFDSet(FDSetControls* fdctrl, int sockfd){
    return FD_ISSET(sockfd, &fdctrl->set);
}

#elif defined(USE_EPOLL)
int QueryEpoll(int epollfd, struct epoll_event* events){
    int pendingEventCount = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if(pendingEventCount == -1){
        perror("epoll_wait");
        return 0;
    }
    return pendingEventCount;
}
int CreateEpoll(){
    int epollfd = epoll_create1(0);
    if(epollfd == -1){
        perror("epoll_create1");
        return 0;
    }
    return epollfd;
}
uint8_t RemoveSocketFromEpoll(int epollfd, int sockfd){
    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL) == -1){
        perror("epoll_ctl - del");
        return 0;
    }
    return 1;
}
uint8_t AddSocketToEpoll(int epollfd, int sockfd, uint32_t flags){
    struct epoll_event event;
    event.events    = flags;
    event.data.fd   = sockfd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1){
        perror("epoll_ctl - add");
        return 0;
    }
    return 1;
}
#endif

int ParseArgs(int argc, char** argv){
    int opt;
    while((opt = getopt(argc, argv, "sc"))!=-1){
        switch(opt){
            case 's':
                args.mode = 1;
                break;
            case 'c':
                args.mode = 2;
                break;
            default:
                return 1;
        }
    }
    return 0;
}

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

