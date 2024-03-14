#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

int main(){
    Queue* myQueue = QueueCreateQueue();

    char* qCmd = NULL;
    for(int i=0; i<20; i++){
        qCmd = (char*)malloc(128);
        sprintf(qCmd, "Enqueued command %d/20", i);
        QueueEnqueueFIFO(myQueue, (void*)qCmd);
    }

    void* data = QueueDequeueFIFO(myQueue);
    while(data != NULL){
        printf("Dequeued item [%s]\n", (char*) data);
        printf("Items remaining [%d]\n", (int)QueueCount(myQueue));
        free(data);
        data = QueueDequeueFIFO(myQueue);
    }

    QueueDestroy(myQueue, NULL);
    return 0;
}