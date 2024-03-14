#include "ringbuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


uint32_t RBGetMaxSize(){return MAX_BUFFER_SIZE;}

void RBInit(RingBuffer *rb)
{
    rb->pHead = 0;
    rb->pTail = 0;
    rb->lastError = RB_OK;
    pthread_mutex_init(&rb->lock, NULL);
    rb->containsData = 0;
    //pipe(rb->triggerfd);
}

uint8_t RBRead(RingBuffer *rb, void *data, uint16_t maxLength)
{
    pthread_mutex_lock(&rb->lock);
    uint16_t bytesRemaining = RBGetPending(rb);
    if(bytesRemaining == 0){
        printf("no bytes!\n");
        rb->lastError = RB_MEMORY_EMPTY;
        pthread_mutex_unlock(&rb->lock);
        return RB_MEMORY_EMPTY;
    }
    if(bytesRemaining < maxLength){
        maxLength = bytesRemaining;
    }
    uint16_t readPos = rb->pTail;
    rb->pTail = (rb->pTail + maxLength) % MAX_BUFFER_SIZE;
    if(readPos + maxLength <= MAX_BUFFER_SIZE){
        // Single read!
        memcpy(data, rb->buffer+readPos, maxLength);
    } else {
        // Data wraps around to the beginning of the buffer
        uint16_t firstRead = MAX_BUFFER_SIZE - readPos;
        uint16_t secondRead = maxLength - firstRead;
        memcpy(data, rb->buffer + readPos, firstRead);
        memcpy(data + firstRead, rb->buffer, secondRead);
    }
    if(RBGetPending(rb) == 0){
        rb->containsData = 0;
    }
    pthread_mutex_unlock(&rb->lock);
    return RB_OK;
}
uint8_t RBWrite(RingBuffer *rb, void *data, uint16_t dataLength)
{
    pthread_mutex_lock(&rb->lock);
    uint16_t bytesRemaining = RBGetFree(rb);
    if(bytesRemaining < dataLength){
        rb->lastError = RB_MEMORY_FULL;
        pthread_mutex_unlock(&rb->lock);
        return RB_MEMORY_FULL;
    }

    uint16_t writePos = rb->pHead;
    rb->pHead = (rb->pHead + dataLength) % MAX_BUFFER_SIZE;
    if(writePos + dataLength <= MAX_BUFFER_SIZE){
        // All in one write
        memcpy(rb->buffer+writePos, data, dataLength);
    } else {
        // Data wraps around to the beginning of the buffer
        uint16_t firstWrite = MAX_BUFFER_SIZE - writePos;
        uint16_t secondWrite= dataLength - firstWrite;
        memcpy(rb->buffer + writePos, data, firstWrite);
        memcpy(rb->buffer, data + firstWrite, secondWrite);
    }
    //write(rb->triggerfd[1], "1", 1);    // Write to trigger reads
    rb->containsData = 1;
    pthread_mutex_unlock(&rb->lock);
    return RB_OK;
}
uint16_t RBGetPending(RingBuffer* rb){
    return (rb->pHead - rb->pTail + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
}
uint16_t RBGetError(RingBuffer *rb)
{
    return rb->lastError;
}
uint16_t RBGetFree(RingBuffer *rb)
{
    return (rb->pTail - rb->pHead - 1 + MAX_BUFFER_SIZE) % MAX_BUFFER_SIZE;
}

uint8_t RBHasData(RingBuffer* rb){
    return RBGetPending(rb)==0?0:1;
}






size_t serialize_uint8(RingBuffer *ringBuffer, uint8_t value) {
    return RBWrite(ringBuffer, &value, sizeof(uint8_t));
}

size_t deserialize_uint8(RingBuffer *ringBuffer, uint8_t *value) {
    return RBRead(ringBuffer, value, sizeof(uint8_t));
}

size_t serialize_uint16(RingBuffer *ringBuffer, uint16_t value) {
    value = htons(value);
    return RBWrite(ringBuffer, (uint8_t *)&value, sizeof(uint16_t));
}

size_t deserialize_uint16(RingBuffer *ringBuffer, uint16_t *value) {
    size_t bytesRead = RBRead(ringBuffer, (uint8_t *)value, sizeof(uint16_t));
    *value = ntohs(*value);
    return bytesRead;
}

size_t serialize_uint32(RingBuffer *ringBuffer, uint32_t value) {
    value = htonl(value);
    return RBWrite(ringBuffer, (uint8_t *)&value, sizeof(uint32_t));
}

size_t deserialize_uint32(RingBuffer *ringBuffer, uint32_t *value) {
    size_t bytesRead = RBRead(ringBuffer, (uint8_t *)value, sizeof(uint32_t));
    *value = ntohl(*value);
    return bytesRead;
}

size_t serialize_uint64(RingBuffer *ringBuffer, uint64_t value) {
    value = htonll(value);
    return RBWrite(ringBuffer, (uint8_t *)&value, sizeof(uint64_t));
}

size_t deserialize_uint64(RingBuffer *ringBuffer, uint64_t *value) {
    size_t bytesRead = RBRead(ringBuffer, (uint8_t *)value, sizeof(uint64_t));
    *value = ntohll(*value);
    return bytesRead;
}

size_t serialize_int8(RingBuffer *ringBuffer, int8_t value) {
    return serialize_uint8(ringBuffer, (uint8_t)value);
}

size_t deserialize_int8(RingBuffer *ringBuffer, int8_t *value) {
    return deserialize_uint8(ringBuffer, (uint8_t *)value);
}

size_t serialize_int16(RingBuffer *ringBuffer, int16_t value) {
    return serialize_uint16(ringBuffer, (uint16_t)value);
}

size_t deserialize_int16(RingBuffer *ringBuffer, int16_t *value) {
    return deserialize_uint16(ringBuffer, (uint16_t *)value);
}

size_t serialize_int32(RingBuffer *ringBuffer, int32_t value) {
    return serialize_uint32(ringBuffer, (uint32_t)value);
}

size_t deserialize_int32(RingBuffer *ringBuffer, int32_t *value) {
    return deserialize_uint32(ringBuffer, (uint32_t *)value);
}

size_t serialize_int64(RingBuffer *ringBuffer, int64_t value) {
    return serialize_uint64(ringBuffer, (uint64_t)value);
}

size_t deserialize_int64(RingBuffer *ringBuffer, int64_t *value) {
    return deserialize_uint64(ringBuffer, (uint64_t *)value);
}