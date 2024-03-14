#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    void **data;
    size_t size;
    size_t capacity;
} Vector;

Vector* VectorCreate();

uint8_t VectorInit(Vector *vec);

// Ensure there is enough space for at least 'size' elements
uint8_t VectorReserve(Vector* vec, size_t size);

uint8_t VectorPush_back(Vector *vec, void* value);

void* VectorAt(Vector *vec, size_t index);

// Get the current number of elements stored inside the vector
size_t VectorSize(Vector *vec);

// Get the total available capacity of the vector before it has to allocate more memory
size_t VectorCapacity(Vector* vec);

void VectorFree(Vector *vec, void (*freeFunc)(void*));

#endif /* __VECTOR_H__ */