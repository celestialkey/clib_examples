#ifndef __COMMSLINKS_H__
#define __COMMSLINKS_H__

#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../ringbuffer/ringbuffer.h"

typedef struct {
    int32_t             socketfd;
    struct sockaddr_in  address;
    RingBuffer          pendingRead;
    RingBuffer          pendingWrite;
} CommLink;

void CLInitCommLink(CommLink* link);

#endif
