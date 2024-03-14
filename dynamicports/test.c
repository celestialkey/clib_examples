#define USE_EPOLL

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#include <sys/epoll.h>

#include "../ringbuffer/ringbuffer.h"
#include "../commlinks/commlinks.h"
#include "../map/map.h"
#include "../wsocket/wsocket.h"
#include "../wepoll/wepoll.h"

#define MAX_CLIENTS 20

struct {
    uint8_t mode;
} args;

typedef struct {
    // The channel this is associated with
    uint64_t channelId;
    // The data itself
    CommLink PointA;
} endpoint;

struct {
    Map* socketEndpoints;
} globals;


//=====================================================================
//      Prototypes
//=====================================================================
int ParseArgs(int argc, char* argv[]);

void HandleInboundConnection(CommLink* clientlist, int sockfd);
int FindLinkFromSockFd(CommLink* clientlist, int sockfd);

//=====================================================================
//      Main Example Start
//=====================================================================
int ReadDataFromUDPSocket(CommLink* link){
    int cmdChannelLen = sizeof(link->address);
    int readLength = RBGetFree(&link->pendingRead);
    char buffer[readLength+1];
    int bytesRead = 0;
    if((bytesRead = recvfrom(link->socketfd, buffer, readLength, 0, (struct sockaddr*)&link->address, (socklen_t*)&cmdChannelLen))==-1){
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

void HandleTraffic_EPOLL(CommLink* cmdChannel){
    int epollfd = CreateEpoll();
    if(!epollfd) return;

    struct epoll_event events[MAX_EVENTS];
    int nfds, acceptClientSocket;

    // Add server sockets to the list
    AddSocketToEpoll(epollfd, cmdChannel->socketfd, EPOLLIN);

    while(1){
        // Check for events
        nfds = QueryEpoll(epollfd, events);
        if(!nfds) break;

        // Loop through events pending action
        for(int i=0; i<nfds; i++){
            int pendingSocket = events[i].data.fd;
            // check if the event is related to the server UDP socket
            if(pendingSocket == cmdChannel->socketfd){
                if(!ReadDataFromUDPSocket(cmdChannel)){
                    printf("error udp\n");
                }
                continue; // Move on to the next event
            }

            // Attempt to find the client with a pending event
            uint64_t hashKey = MapGetKey_Int32(pendingSocket);
            CommLink* clientLink = (CommLink*)MapGet(globals.socketEndpoints, hashKey);
            if(clientLink == NULL) {
                // Unknown client, close the socket
                RemoveSocketFromEpoll(epollfd, pendingSocket);
                close(pendingSocket);
                continue; // Move on to the next event
            }
            // Read data for the client of interest.
            if(!ReadDataFromTCPSocket(clientLink)){
                RemoveSocketFromEpoll(epollfd, clientLink->socketfd);
                close(clientLink->socketfd);
                MapRemove(globals.socketEndpoints, hashKey);
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
        if(!RBHasData(&cmdChannel->pendingRead)) continue;
        int readableData = RBGetPending(&cmdChannel->pendingRead);
        char buffer[readableData+1];
        RBRead(&cmdChannel->pendingRead, buffer, readableData);
        buffer[readableData] = 0;
        printf("CMD: %s\n", buffer);
    }
}

int main(){
    // Setup the map
    globals.socketEndpoints = MapCreate(NULL);

    // Setup server tcp/udp sockets
    CommLink cmdChannel;
    cmdChannel.socketfd = CreateUDPSocket();
    SetSocketOptions(cmdChannel.socketfd);
    BindSocket(cmdChannel.socketfd, "127.0.0.1", 5556);

    // Do stuff
    HandleTraffic_EPOLL(&cmdChannel);

    close(cmdChannel.socketfd);
    MapDestroy(globals.socketEndpoints);
    return 0;
}


//=====================================================================
//      Parse Arguments
//=====================================================================
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


//=====================================================================
//      Client Linkage
//=====================================================================
int FindLinkFromSockFd(CommLink* clientlist, int sockfd){
    for(int i=0; i<MAX_CLIENTS; i++){
        if(clientlist[i].socketfd == sockfd)
            return i;
    }
    return -1;
}
void HandleInboundConnection(CommLink* clientlist, int sockfd){
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
