
#include "hash.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void FreeHashData(void* data){
    if(data == NULL) return;
    free(data);
}

int main(){
    HashMap* map = HashCreateTable();

    char* data = malloc(128);
    memset(data, 0, 128);
    strncpy(data, "Hello world!\n", 128);
    HashInsert(map, 0x1234, (void*)data);

    data = malloc(128);
    memset(data, 0, 128);
    strncpy(data, "big test boi!\n", 128);
    HashInsert(map, 0x124, (void*)data);

    data = malloc(128);
    memset(data, 0, 128);
    strncpy(data, "just a sampler!\n", 128);
    HashInsert(map, 0x134, (void*)data);

    data = malloc(128);
    memset(data, 0, 128);
    strncpy(data, "shit happens!\n", 128);
    HashInsert(map, 0xF234, (void*)data);

    printf("Val: %s", (char*)HashGet(map, 0x1234));
    printf("Val: %s", (char*)HashGet(map, 0x124));
    printf("Val: %s", (char*)HashGet(map, 0x134));
    printf("Val: %s", (char*)HashGet(map, 0xF234));

    HashRemove(map, 0x1234, FreeHashData);
    HashRemove(map, 0x124, FreeHashData);
    HashRemove(map, 0x134, FreeHashData);
    HashRemove(map, 0x1234, FreeHashData);

    HashDestroyTable(map, FreeHashData);
    return 0;
}