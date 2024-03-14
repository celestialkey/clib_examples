#include "linkedlists.h"
#include <stdio.h>
#include <stdlib.h>

// Function to create a new LLNode
LLNode* LLCreateNode(void* data) {
    LLNode* newLLNode = (LLNode*)malloc(sizeof(LLNode));
    if (newLLNode == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    newLLNode->data = data;
    newLLNode->next = NULL;
    return newLLNode;
}

// Function to insert a LLNode at the beginning of the linked list
LLNode* LLInsertAtBeginning(LLNode* head, void* data) {
    LLNode* newLLNode = LLCreateNode(data);
    newLLNode->next = head;
    return newLLNode;
}

uint32_t LLSize(LLNode* head){
    uint32_t size = 0;
    LLNode* current = head;
    while (current->next != NULL) {
        current = current->next;
        size++;
    }
    return size;
}

// Function to insert a LLNode at the end of the linked list
LLNode* LLInsertAtEnd(LLNode* head, void* data) {
    LLNode* newLLNode = LLCreateNode(data);
    if (head == NULL) {
        return newLLNode;
    }
    LLNode* current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newLLNode;
    return head;
}

// Function to free the memory allocated for the linked list
void LLFreeList(LLNode* head, void (*freeFunc)(void*)){
    LLNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        if(temp->data != NULL){
            if(freeFunc != NULL){
                printf("Custom Freeing '%s'\n", (char*)temp->data);
                fflush(stdout);
                freeFunc(temp->data);
            } else {
                printf("Freeing '%s'\n", (char*)temp->data);
                fflush(stdout);
                free(temp->data);
            }
        }
        free(temp);
    }
}