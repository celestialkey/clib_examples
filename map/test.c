#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "map.h"

typedef struct {
    int data;
    int data2;
} RandomData;

void freeFunc(void* data){
    printf("Free Called!\n");
    RandomData* d = (RandomData*) data;
    free(d);
}

int main(){
    Map* map1 = MapCreate(NULL);
    Map* map2 = MapCreate(freeFunc); //

    char ckey[128];
    char* txt;
    int b = 0;
    printf("Adding data to map1 with default clean.\n");
    for(int i=0; i<250; i++){
        b++;
        memset(ckey, 0, 128);
        sprintf(ckey, "my%d ckey%d is%d %d", i,i,i,i);
        txt = malloc(128);
        memset(txt, 0, 128);
        sprintf(txt, "Stored text with value %d", i);
        if(!MapAdd(map1, MapGetKey_Char(ckey), txt)){
            printf("Failed to add on %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    printf("Adding data to map2 with custom clean.\n");
    for(int i=20; i<30; i++){
        memset(ckey, 0, 128);
        sprintf(ckey, "my%d ckey%d is%d %d", i,i,i,i);
        RandomData* d = (RandomData*)malloc(sizeof(RandomData));
        d->data = i;
        d->data2 = i + b;
        if(!MapAdd(map2, MapGetKey_Char(ckey), d)){
            printf("Failed to add on %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    
    printf("Updating data on map1...\n");
    memset(ckey, 0, 128);
    sprintf(ckey, "my50 ckey50 is50 50");
    txt = malloc(128);
    memset(txt, 0, 128);
    sprintf(txt, "Stored text with value 50");
    if(!MapUpdate(map1, MapGetKey_Char(ckey), txt)){
        printf("Failed to update map1 id 50\n");
        exit(EXIT_FAILURE);
    }

    printf("Updating data on map2...\n");
    memset(ckey, 0, 128);
    sprintf(ckey, "my25 ckey25 is25 25");
    RandomData* d = (RandomData*)malloc(sizeof(RandomData));
    d->data2 = 9999;
    d->data = 9998;
    if(!MapUpdate(map2, MapGetKey_Char(ckey), d)){
        printf("Failed to update map2 id 25\n");
        exit(EXIT_FAILURE);
    }

    printf("Get single element from map1.\n");
    memset(ckey, 0, 128);
    sprintf(ckey, "my30 ckey30 is30 30");
    txt = MapGet(map1, MapGetKey_Char(ckey));
    if(!txt){
        printf("Failed to get map1 id 30\n");
        exit(EXIT_FAILURE);
    }
    printf("Get Results for 30 are: %s\n", txt);


    printf("Get single element from map2.\n");
    memset(ckey, 0, 128);
    sprintf(ckey, "my25 ckey25 is25 25");
    d = NULL;
    d = MapGet(map2, MapGetKey_Char(ckey));
    if(!d){
        printf("Failed to get map2 id 25\n");
        exit(EXIT_FAILURE);
    }
    printf("Get results for 25 are: %d, %d\n", d->data, d->data2);

    printf("Destroy a single element from map1\n");
    memset(ckey, 0, 128);
    sprintf(ckey, "my127 ckey127 is127 127");
    MapRemove(map1, MapGetKey_Char(ckey));

    printf("Destroy a single element from map2\n");
    memset(ckey, 0, 128);
    sprintf(ckey, "my27 ckey27 is27 27");
    MapRemove(map2, MapGetKey_Char(ckey));



    printf("Adding data to map1 with default clean.\n");
    MapIterator* it = MapCreateIterator(map2);
    int i=0;
    while(MapIterPeek(it) != NULL){
        RandomData* d = (RandomData*)MapIterNext(it);
        printf("%d] Got from iter %d %d\n", i, d->data, d->data2);
        i++;
    }
    printf("Iterated %d/%d elements.\n", i,b);

    printf("Cleaning up map1\n");
    MapDestroy(map1);

    printf("Cleaning up map2\n");
    MapDestroy(map2);

    return 0;
}