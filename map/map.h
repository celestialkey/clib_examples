#ifndef __MAP_H__
#define __MAP_H__

#include <stdint.h>

#define MAX_MAP_TABLE_SIZE  5

typedef struct _keyvaluepair_t {
    uint64_t first;
    void* second;
    struct _keyvaluepair_t* next;
} KeyValuePair;

typedef struct {
    KeyValuePair* pairs[MAX_MAP_TABLE_SIZE];
    uint64_t size;
} Map;

// Create a new hashmap map
Map* MapCreate();

// Destroy a existing map
void MapDestroy(Map* map);

// Insert a new entry 
uint8_t MapAdd(Map* map, uint64_t key, void* value);

// Update an existing value
uint8_t MapUpdate(Map* map, uint64_t key, void* value);

// Find data within the sea of data
KeyValuePair* MapFind(Map* map, uint64_t key);

// Grab a entry from the map
void* MapGet(Map* map, uint64_t key);

// Remove an entry from the map
void MapRemove(Map* map, uint64_t key);

// Get the number of items tracked.
uint64_t MapSize(Map* map);

// Different mechanisms to create keys...
uint64_t MapGetKey_Char     (const char*key);
uint64_t MapGetKey_UInt8    (uint8_t   key);
uint64_t MapGetKey_Int8     (int8_t    key);
uint64_t MapGetKey_UInt16   (uint16_t   key);
uint64_t MapGetKey_Int16    (int16_t    key);
uint64_t MapGetKey_UInt32   (uint32_t   key);
uint64_t MapGetKey_Int32    (int32_t    key);
uint64_t MapGetKey_UInt64   (uint64_t   key);
uint64_t MapGetKey_Int64    (int64_t    key);

typedef struct {
    Map* pMap;
    uint64_t index;
    KeyValuePair* current;
} MapIterator;

// Create a iterator for a map object
MapIterator* MapCreateIterator(Map* map);
// Peek to see if there's a value next entry
KeyValuePair* MapIterPeek(MapIterator* iter);
// Get the next element in the iter
KeyValuePair* MapIterNext(MapIterator* iter);
// Destroy the iterator
void MapDestroyIterator(MapIterator* iter);
#endif
