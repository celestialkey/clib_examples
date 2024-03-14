
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "hash.h"

unsigned int Hash(uint16_t key){
    return key % MAX_HASH_TABLE_SIZE;
}

HashMap* HashCreateTable(){
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if(map == NULL){
        return NULL;
    }
    for(int i=0; i<MAX_HASH_TABLE_SIZE; i++){
        map->table[i] = NULL;
    }
    return map;
}

void HashDestroyTable(HashMap* map, void (*freeValueFunc)(void*)){
    for(int i=0; i<MAX_HASH_TABLE_SIZE; i++){
        HashNode* current = map->table[i];
        while(current != NULL){
            HashNode* temp = current;
            current = current->next;
            if(freeValueFunc != NULL){
                freeValueFunc(temp->data);
            }
            free(temp);

        }
    }
    if(map != NULL)
        free(map);
}

void HashInsert(HashMap* map, uint16_t key, void* data){
    uint16_t index = Hash(key);
    HashNode* node = (HashNode*)malloc(sizeof(HashNode));
    if(node == NULL){
        return;
    }
    node->key   = key;
    node->data  = data;
    node->next  = NULL;

    if(map->table[index]==NULL){
        map->table[index] = node;
    } else {
        // Collision detected, add to the end of the linked list instead
        HashNode* temp = map->table[index];
        while(temp->next != NULL) { 
            temp = temp->next;
        }
        temp->next = node;
    }
}

void* HashGet(HashMap* map, uint16_t key){
    uint16_t index = Hash(key);
    HashNode* node = map->table[index];
    while(node != NULL){
        if(node->key == key){
            return node->data;
        }
        node = node->next;
    }
    return NULL; // Key not found
}

void HashRemove(HashMap* map, uint16_t key, void (*freeValueFunc)(void*)){
    uint16_t index = Hash(key);
    HashNode* prev = NULL;
    HashNode* curr = map->table[index];
    while(curr != NULL){
        if(curr->key == key){
            if(prev == NULL){
                map->table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            if(freeValueFunc != NULL){
                freeValueFunc(curr->data);
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}
