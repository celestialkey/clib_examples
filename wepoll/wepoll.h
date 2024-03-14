#ifndef __WEPOLL_H__
#define __WEPOLL_H__

#include <stdint.h>

#define MAX_EVENTS  20
struct epoll_event;


int QueryEpoll(int epollfd, struct epoll_event* events);
int CreateEpoll();

uint8_t RemoveSocketFromEpoll(int epollfd, int sockfd);
uint8_t AddSocketToEpoll(int epollfd, int sockfd, uint32_t flags);

#endif
