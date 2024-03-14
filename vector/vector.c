#include "vector.h"

#define INITIAL_CAPACITY 10

Vector* VectorCreate() {
    Vector* vec = (Vector*)malloc(sizeof(Vector));
    if(vec == NULL){
        return NULL;
    }
    vec->data = (void**)malloc(INITIAL_CAPACITY * sizeof(void*));
    if (vec->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(vec);
        return NULL;
    }
    vec->size = 0;
    vec->capacity = INITIAL_CAPACITY;
    return vec;
}

uint8_t VectorInit(Vector *vec) {
    vec->data = (void**)malloc(INITIAL_CAPACITY * sizeof(void*));
    if (vec->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }
    vec->size = 0;
    vec->capacity = INITIAL_CAPACITY;
    return 1;
}

uint8_t VectorReserve(Vector* vec, size_t size){
    if(vec->capacity >= size) return 1; // Plenty of space
    // Increase the size until a desired allocation is reached
    do { vec->capacity *= 2; } while(vec->capacity < size);
    // Realloc enough space
    vec->data = realloc(vec->data, vec->capacity * sizeof(void*));
    if (vec->data == NULL) {
        fprintf(stderr, "Memory reallocation failed\n");
        return 0;
    }
    return 1;
}

uint8_t VectorPush_back(Vector *vec, void* value) {
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity * sizeof(void*));
        if (vec->data == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return 0;
        }
    }
    vec->data[vec->size++] = value;
    return 1;
}

void* VectorPop_front(Vector *vec) {
    if (vec->size == 0) {
        fprintf(stderr, "Vector is empty\n");
        return NULL;
    }
    void* retData = vec->data[0];
    for (size_t i = 1; i < vec->size; ++i) {
        vec->data[i - 1] = vec->data[i];
    }
    vec->size--;
    return retData;
}

void* VectorAt(Vector *vec, size_t index) {
    if (index < 0 || index >= vec->size) {
        fprintf(stderr, "Index out of bounds\n");
        return NULL;
    }
    return vec->data[index];
}

size_t VectorSize(Vector *vec) {
    return vec->size;
}

size_t VectorCapacity(Vector* vec){
    return vec->capacity;
}

void VectorFree(Vector *vec, void (*freeFunc)(void*)) {
    for(int i=0; i<vec->size; i++){
        if(vec->data[i] != NULL){
            if(freeFunc) freeFunc(vec->data[i]);
            else free(vec->data[i]);
        }
    }
    free(vec->data);
    vec->size = 0;
    vec->capacity = 0;
}