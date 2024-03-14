#include <stdlib.h>
#include "queue.h"


// Create a new item to add to a queue
QueueItem* QueueCreateItem(void* data){
    QueueItem* item = (QueueItem*)malloc(sizeof(QueueItem));
    item->data = data;
    item->next = NULL;
    return item;
}

// Create a new Queue structure
Queue* QueueCreateQueue(){
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    return queue;
}

// Destroy the queue
void QueueDestroy(Queue* queue, void (*freeDataFunc)(void*)){
    QueueItem* item = QueueDequeueFIFO(queue);
    while(item != NULL){
        QueueItem* temp = item;
        item = QueueDequeueFIFO(queue);
        if(freeDataFunc != NULL){ 
            // Call the custom cleanup for data pointer
            if(temp->data != NULL)
                freeDataFunc(temp->data);
        } else {
            // Perform default cleanup
            if(temp->data != NULL)
                free(temp->data);
        }
        // Destroy the item container
        free(temp);
    }
    // Release the mutex
    pthread_mutex_destroy(&queue->mutex);
    // Destroy the queue
    free(queue);
}

// Check if there are any items in the FIFO queue
uint8_t QueueIsEmpty(Queue* queue){
    return (queue->front == NULL);
}

// Count the number of items in the queue
uint32_t QueueCount(Queue* queue){
    uint32_t itemCount = 0;
    QueueItem* n = queue->front;
    while(n != NULL){
        itemCount++;
        n = n->next;
    }
    return itemCount;
}

// Add an item to the queue, FIFO style
void QueueEnqueueFIFO(Queue* queue, void* data){
    // Lock the queue mutex
    pthread_mutex_lock(&queue->mutex);
    // Create a new container and attach the data
    QueueItem* item = QueueCreateItem(data);
    // Check if the queue is empty
    if(QueueIsEmpty(queue)){
        queue->front = queue->rear = item;
        pthread_mutex_unlock(&queue->mutex);
        return;
    }
    // Add an item to the queue
    queue->rear->next = item;
    // Adjust the rear pointer to point to the new item
    queue->rear = item;
    // Unlock the mutex
    pthread_mutex_unlock(&queue->mutex);
}

// Dequeue an item from the queue, FIFO style
void* QueueDequeueFIFO(Queue* queue){
    void* item = NULL;
    // Lock the mutex for this queue
    pthread_mutex_lock(&queue->mutex);
    // Check if there's any items to dequeue
    if(QueueIsEmpty(queue)){
        pthread_mutex_unlock(&queue->mutex);
        return item;
    }
    // Extract the stored data
    item = queue->front->data;
    // Track the container tasked for free
    QueueItem* tmp = queue->front;
    // Migrate the front pointer to point to the next container
    queue->front = queue->front->next;
    // Ensure empty queues are handled appropriately
    if(queue->front == NULL)
        queue->rear = NULL;
    // Destroy the container
    free(tmp);
    // Free the mutex
    pthread_mutex_unlock(&queue->mutex);
    // Return the stack item
    return item;
}