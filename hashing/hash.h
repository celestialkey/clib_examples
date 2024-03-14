#ifndef __HASH_H__
#define __HASH_H__

#include <stdint.h>

#define MAX_HASH_TABLE_SIZE     100

typedef struct _hashNode_t{
    uint16_t key;
    void* data;
    struct _hashNode_t* next;
} HashNode;


typedef struct {
    HashNode* table[MAX_HASH_TABLE_SIZE];
} HashMap;

// Identify the index in the table associated with the key. 
// This is mostly for internal usage and does not have much practical
// value for the developer.
unsigned int Hash(uint16_t key);

// Create a hash table
HashMap* HashCreateTable();

// Destroy a hash table
void HashDestroyTable(HashMap* map, void (*freeValueFunc)(void*));

// Add an entry to the hash table
void HashInsert(HashMap* map, uint16_t key, void* data);

// Retrieve a value from the hash table
void* HashGet(HashMap* map, uint16_t key);

// Remove and cleanup a value from the hashtable
void HashRemove(HashMap* map, uint16_t key, void (*freeValueFunc)(void*));


#endif