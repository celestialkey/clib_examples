#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdint.h>

// From ../ringbuffer/ringbuffer.h
typedef struct RingBuffer;

typedef struct {
    uint16_t packetSize;
    uint32_t packetCRC;
} PacketHeader;

uint32_t PacketCalculateCRC(RingBuffer* rb);
uint8_t PacketValidateCRC(RingBuffer* rb);

uint8_t PacketWrite(RingBuffer* rb, void* data, uint32_t length);


#endif
