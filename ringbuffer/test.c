#include "ringbuffer.h"

#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>

void* producer(void *arg){
    RingBuffer* rb = (RingBuffer*)arg;
    char writeBuffer[1024];
    for(int i=0; i<100; i++){
        sprintf(writeBuffer, "Item %d/100", i);
        if(RBWrite(rb, (void*)writeBuffer, 1024) != RB_OK){
            i--;
            //int s = rand() % 5;
            //sleep(s);
            continue;
        }
        printf("P: [%d/100] Remaining [%d] '%s'\n", i, (int)RBGetFree(rb), writeBuffer);
    }
    pthread_exit(NULL);
}
void* consumer(void *arg){
    RingBuffer* rb = (RingBuffer*)arg;
    char readBuffer[1024];
    for(int i=0; i<100; i++){
        if(RBHasData(rb)){
            RBRead(rb, readBuffer, 1024);
            printf("C: Got '%s'\n", readBuffer);
            continue;
        }
        i--;
        int s = rand() % 5;
        printf("C: Waiting for data...(%d)[%d]\n", i, s);
        //sleep(s);
    }
    pthread_exit(NULL);
}

int main(){
    RingBuffer rbmem;
    RBInit(&rbmem);

    pthread_t prod, consu;
    pthread_create(&prod, NULL, producer, (void*)&rbmem);
    pthread_create(&consu, NULL, consumer, (void*)&rbmem);

    pthread_join(prod, NULL);
    pthread_join(consu, NULL);

    printf("Testing serialize...\n");
    int32_t testInt = 0xFFDDCCBB;
    serialize_int32 (&rbmem, testInt);
    serialize_int32 (&rbmem, 0xFFFFFFFF);
    serialize_uint32(&rbmem, 0xFEDCBA98);
    serialize_uint64(&rbmem, 0xFEDCBA9876543210);

    int32_t read1;  deserialize_int32(&rbmem, &read1);
    int32_t read2;  deserialize_int32(&rbmem, &read2);
    uint32_t read3; deserialize_uint32(&rbmem, &read3);
    uint64_t read4; deserialize_uint64(&rbmem, &read4);

    if(read1 != testInt)            printf("read1 failed!\n");
    if(read2 != (int32_t)0xFFFFFFFF)printf("read2 failed!\n");
    if(read3 != 0xFEDCBA98)         printf("read3 failed!\n");
    if(read4 != 0xFEDCBA9876543210) printf("read4 failed!\n");
    printf("Finished!\n");



    return 0;
}