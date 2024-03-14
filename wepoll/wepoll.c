
#include <sys/epoll.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "wepoll.h"



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