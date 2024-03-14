#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>
#include <pthread.h>

typedef struct _queueItem_t{
    void* data;
    struct _queueItem_t* next;
} QueueItem;

typedef struct {
    QueueItem* rear;
    QueueItem* front;
    pthread_mutex_t mutex;
} Queue;


// Create a new Queue structure
Queue* QueueCreateQueue();

// Destroy the queue. Pass NULL to freeDataFunc for the default free call.
void QueueDestroy(Queue* queue, void (*freeDataFunc)(void*));

// Check if there are any items in the queue
uint8_t QueueIsEmpty(Queue* queue);

// Count the number of items in the queue
uint32_t QueueCount(Queue* queue);

// Add an item to the queue, FIFO style
void QueueEnqueueFIFO(Queue* queue, void* data);

// Dequeue an item from the queue, FIFO style
void* QueueDequeueFIFO(Queue* queue);




#endif