#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <stdint.h>

// Define a LLNode structure
typedef struct LLNode {
    void* data;
    struct LLNode* next;
} LLNode;

// Function prototypes
LLNode* LLCreateNode(void* data);
LLNode* LLInsertAtBeginning(LLNode* head, void* data);
LLNode* LLInsertAtEnd(LLNode* head, void* data);

uint32_t LLSize(LLNode* head);

void LLFreeList(LLNode* head, void (*freeFunc)(void*));

#endif