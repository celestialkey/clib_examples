#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stdint.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 2048

//=========================================================
//                  Basic Ring Buffer
//=========================================================
typedef struct {
    uint8_t     buffer[MAX_BUFFER_SIZE];
    uint16_t    pHead;
    uint16_t    pTail;
    uint8_t     containsData;
    uint8_t     lastError;
    pthread_mutex_t lock;
} RingBuffer;


enum {
    RB_OK,
    RB_MEMORY_EMPTY,
    RB_MEMORY_FULL,
};

void RBInit(RingBuffer* rb);
uint8_t RBRead(RingBuffer* rb, void *data, uint16_t maxLength);
uint8_t RBWrite(RingBuffer* rb, void* data, uint16_t dataLength);
uint16_t RBGetFree(RingBuffer* rb);
uint16_t RBGetPending(RingBuffer* rb);
uint16_t RBGetError(RingBuffer* rb);
uint8_t RBHasData(RingBuffer* rb);
uint32_t RBGetMaxSize();

//-----------------------------------------------------------
//===========================================================
//-----------------------------------------------------------

// Define htonll() and ntohll() if not available
#if !defined(htobe64) && !defined(be64toh)
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        #define htonll(x) __builtin_bswap64(x)
        #define ntohll(x) __builtin_bswap64(x)
    #else
        #define htonll(x) (x)
        #define ntohll(x) (x)
    #endif
#endif

// Function to serialize a uint8_t into a ring buffer (big-endian)
size_t serialize_uint8(RingBuffer *ringBuffer, uint8_t value);

// Function to deserialize a uint8_t from a ring buffer (host byte order)
size_t deserialize_uint8(RingBuffer *ringBuffer, uint8_t *value);

// Function to serialize a uint16_t into a ring buffer (big-endian)
size_t serialize_uint16(RingBuffer *ringBuffer, uint16_t value);

// Function to deserialize a uint16_t from a ring buffer (host byte order)
size_t deserialize_uint16(RingBuffer *ringBuffer, uint16_t *value);

// Function to serialize a uint32_t into a ring buffer (big-endian)
size_t serialize_uint32(RingBuffer *ringBuffer, uint32_t value);

// Function to deserialize a uint32_t from a ring buffer (host byte order)
size_t deserialize_uint32(RingBuffer *ringBuffer, uint32_t *value);

// Function to serialize a uint64_t into a ring buffer (big-endian)
size_t serialize_uint64(RingBuffer *ringBuffer, uint64_t value);

// Function to deserialize a uint64_t from a ring buffer (host byte order)
size_t deserialize_uint64(RingBuffer *ringBuffer, uint64_t *value);

// Function to serialize an int8_t into a ring buffer (big-endian)
size_t serialize_int8(RingBuffer *ringBuffer, int8_t value);

// Function to deserialize an int8_t from a ring buffer (host byte order)
size_t deserialize_int8(RingBuffer *ringBuffer, int8_t *value);

// Function to serialize an int16_t into a ring buffer (big-endian)
size_t serialize_int16(RingBuffer *ringBuffer, int16_t value);

// Function to deserialize an int16_t from a ring buffer (host byte order)
size_t deserialize_int16(RingBuffer *ringBuffer, int16_t *value);

// Function to serialize an int32_t into a ring buffer (big-endian)
size_t serialize_int32(RingBuffer *ringBuffer, int32_t value);

// Function to deserialize an int32_t from a ring buffer (host byte order)
size_t deserialize_int32(RingBuffer *ringBuffer, int32_t *value);

// Function to serialize an int64_t into a ring buffer (big-endian)
size_t serialize_int64(RingBuffer *ringBuffer, int64_t value);

// Function to deserialize an int64_t from a ring buffer (host byte order)
size_t deserialize_int64(RingBuffer *ringBuffer, int64_t *value);


#endif