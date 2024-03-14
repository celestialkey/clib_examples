#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "map.h"


uint64_t MapGetKey_Char(const char* key){
    uint64_t hash = 0;
    while(*key){
        hash = (hash << 5)+*key++;
    }
    return hash;
}

uint64_t MapGetKey_UInt8   (uint8_t   key){
    uint64_t hash = (uint64_t)key;

    // Apply a simple hash function (for demonstration purposes)
    // This is just an example and might not be a good hash function for your specific use case
    hash ^= (hash >> 11);
    hash *= 0xdeadbeefcafebabeULL;
    hash ^= (hash >> 31);

    return hash;
}
uint64_t MapGetKey_Int8    (int8_t    key)  {return MapGetKey_UInt8(key);}

uint64_t MapGetKey_UInt16   (uint16_t   key){
     uint64_t hash = (uint64_t)key;

    // Apply a simple hash function (for demonstration purposes)
    // This is just an example and might not be a good hash function for your specific use case
    hash ^= (hash >> 23);
    hash *= 0x2127599bf4325c37ULL;
    hash ^= (hash >> 47);

    return hash;
}
uint64_t MapGetKey_Int16    (int16_t    key){return MapGetKey_UInt16(key);}

uint64_t MapGetKey_UInt32(uint32_t   key){
    // Choose a prime number for mixing
    const uint64_t prime = 4294967311UL; // Large prime less than 2^32

    // Mix the input value with the prime number
    return ((uint64_t)key * prime);
}
uint64_t MapGetKey_Int32(int32_t    key){return MapGetKey_UInt32((uint32_t)key);}

uint64_t MapGetKey_UInt64(uint64_t   key){return key;}
uint64_t MapGetKey_Int64(int64_t    key){return (uint64_t)key;}



static uint64_t _hashIndex(uint64_t key){
    return key % MAX_MAP_TABLE_SIZE;
}

static KeyValuePair* _createMapPair(uint64_t key, void* value){
    KeyValuePair* pair = (KeyValuePair*)malloc(sizeof(KeyValuePair));
    if(!pair) return NULL;
    pair->first   = key;
    pair->second = value;
    pair->next  = NULL;
    return pair;
}

// Create a new hashmap map
Map* MapCreate(){
    Map* map = (Map*)malloc(sizeof(Map));
    if(!map) return NULL;

    for(int i=0; i<MAX_MAP_TABLE_SIZE; i++){
        map->pairs[i] = NULL;
    }
    map->size = 0;
    return map;
}

// Destroy a existing map
void MapDestroy(Map* map){
    for(int i =0; i<MAX_MAP_TABLE_SIZE; i++){
        KeyValuePair* current = map->pairs[i];
        while(current != NULL){
            KeyValuePair* temp = current;
            current = current->next;
            if(temp->second)
                free(temp->second);
            free(temp);
        }
    }
    free(map);
    map->size--;
}

// Insert a new entry 
uint8_t MapAdd(Map* map, uint64_t key, void* value){
    KeyValuePair* kv = _createMapPair(key, value);
    if(!kv) return 0;
    uint32_t index = _hashIndex(key);
    if(map->pairs[index] == NULL){
        map->pairs[index] = kv;
    } else {
        KeyValuePair* current = map->pairs[index];
        while(current->next != NULL){
            current = current->next;
        }
        current->next = kv;
    }
    map->size++;
    return 1;
}

// Update an existing value
uint8_t MapUpdate(Map* map, uint64_t key, void* value){
    uint32_t index = _hashIndex(key);
    KeyValuePair* current = map->pairs[index];
    while(current != NULL){
        // Skip until the key is matched
        if(current->first != key){
            current = current->next;
            continue;
        }
        if(!current->second){
            // No prior value, just update
            current->second = value;
            return 1;
        } else{
            // Cleanup the old pointer prior to updating
            free(current->second);
            current->second = value;
            return 1;
        }
    }
    return 0;
}

// Grab a entry from the map
void* MapGet(Map* map, uint64_t key){
    uint32_t index = _hashIndex(key);
    
    KeyValuePair* current = map->pairs[index];
    while (current != NULL) {
        if(current->first == key){
            return current->second;
        }
        current = current->next;
    }
    return NULL;
}

// Grab a entry from the map
void MapRemove(Map* map, uint64_t key){
    uint64_t index = _hashIndex(key);
    KeyValuePair* current = map->pairs[index];
    KeyValuePair* prev = NULL;
    while (current != NULL) {
        // Find entry to remove
        if(current->first != key){
            prev = current;
            current = current->next;
            continue;
        }
        // Link previous and current
        prev->next = current->next;
        // Cleanup current
        if(current->second)
            free(current->second);
        free(current);
        map->size--;
        return;
    }
}

uint64_t MapSize(Map* map){
    return map->size;
}

KeyValuePair* MapFind(Map* map, uint64_t key){
    MapIterator* iter = MapCreateIterator(map);
    KeyValuePair* current = MapIterNext(iter);
    while(current != NULL){
        if(current->first == key){
            break;
        }
        current = MapIterNext(iter);
    }
    MapDestroyIterator(iter);
    return current;
}

// Create a iterator for a map object
MapIterator* MapCreateIterator(Map* map){
    MapIterator* iter = (MapIterator*)malloc(sizeof(MapIterator));
    if(!iter) return NULL;
    iter->pMap = map;
    iter->index = -1;
    iter->current = NULL;
    return iter;
}

KeyValuePair* _PeekNext(MapIterator* iter){
    if(iter == NULL) return NULL;

    int index = iter->index;
    KeyValuePair* current = iter->current;
    
    if(current != NULL && current->next != NULL){
        return current->next->second;
    }

    do{
        index++;
    } while(index < MAX_MAP_TABLE_SIZE && iter->pMap->pairs[index] == NULL);

    if(index < MAX_MAP_TABLE_SIZE){
        return iter->pMap->pairs[index]->second;
    }
    return NULL;
}
// Peek to see if there's a value next entry
KeyValuePair* MapIterPeek(MapIterator* iter){
    if(iter==NULL) return NULL;
    return _PeekNext(iter);
}

KeyValuePair* _GetNext(MapIterator* iter){
    if(iter == NULL) return NULL;

    if(iter->current != NULL && iter->current->next != NULL){
        iter->current = iter->current->next;
        return iter->current;
    }
    
    do{
        iter->index++;
    } while(iter->index < MAX_MAP_TABLE_SIZE && iter->pMap->pairs[iter->index] == NULL);

    if(iter->index < MAX_MAP_TABLE_SIZE){
        iter->current = iter->pMap->pairs[iter->index];
        return iter->current;
    }
    return NULL;
}
// Get the next element in the iter
KeyValuePair* MapIterNext(MapIterator* iter){
    if(iter == NULL) return NULL;
    return _GetNext(iter);
}
// Destroy the iterator
void MapDestroyIterator(MapIterator* iter){
    free(iter);
}