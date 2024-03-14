#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include <assert.h>

#ifdef DEBUG
#include <stdio.h>
#define DPRINTF(...) {printf(__VA_ARGS__); fflush(stdout);}
#else
#define DPRINTF(...)
#endif


void AddEntriesToList_PushFront(List* list){
    char* txt;
    printf("(Push_Front) Fulling list with 5 elements of data.\n");
    for(int i=0; i<5; i++){
        txt = malloc(128);
        memset(txt, 0, 128);
        sprintf(txt, "Counting %d", i);
        DPRINTF("(Push_Front) Attempting to add '%s'\n", txt);
        assert(list->Push_Front(list, txt));
    }
}
void AddEntriesToList_PushBack(List* list){
    char* txt;
    printf("(Push_Back) Fulling list with 5 elements of data.\n");
    for(int i=5; i<10; i++){
        txt = malloc(128);
        memset(txt, 0, 128);
        sprintf(txt, "Counting %d", i);
        DPRINTF("(Push_Back) Attempting to add '%s'\n", txt);
        assert(list->Push_Back(list, txt));
    }
}

void FreeList(List* l){
    void* data = l->Pop_Front(l);
    while(data != NULL){
        DPRINTF("Free data...[%lu]\n", l->Size(l));
        free(data);
        data = l->Pop_Front(l);
    }
}

void UnitTest_Iterators(){
    List* l = ListCreate();
    assert(l);

    // Prepare test data
    int* startVal   = (int*)malloc(sizeof(int)); // 25;
    int* endVal     = (int*)malloc(sizeof(int)); // 99
    int* midVal     = (int*)malloc(sizeof(int)); // 66;
    *startVal   = 25;
    *midVal     = 66;
    *endVal     = 99;
    l->Push_Back(l, startVal);
    for(int i=0; i<10; i++){ l->Push_Back(l, midVal); }
    l->Push_Back(l, endVal);

    // Test different iterators
    ListIter* startIter = l->Begin(l);
    assert(*(int*)(startIter->current->data) == *startVal);
    ListIter* endIter = l->End(l);
    assert(*(int*)(endIter->current->data) == *endVal);
    ListIter* iter = l->Iterator(l);
    assert(*(int*)iter->current->data == *startVal);
    DPRINTF("UnitTest_Iterators(): Free startIter.\n");
    ListIterDestroy(startIter);
    DPRINTF("UnitTest_Iterators(): Free endIter.\n");
    ListIterDestroy(endIter);
    DPRINTF("UnitTest_Iterators(): Free iter.\n");
    ListIterDestroy(iter);
    iter = l->At(l, 5);
    assert(*(int*)iter->current->data == *midVal);
    DPRINTF("UnitTest_Iterators(): Free iter2.\n");
    ListIterDestroy(iter);

    DPRINTF("UnitTest_Iterators(): Front.\n");
    int* val = l->Front(l);
    iter = l->Begin(l);
    assert(*(int*)(iter->current->data) == *val);
    ListIterDestroy(iter);
    DPRINTF("UnitTest_Iterators(): Back.\n");
    val = l->Back(l);
    iter = l->End(l);
    assert(*(int*)(iter->current->data) == *val);
    ListIterDestroy(iter);
    DPRINTF("UnitTest_Iterators(): Mid\n");
    *val = 66;
    iter = l->At(l, 3);
    assert(*(int*)(iter->current->data) == *val);
    ListIterDestroy(iter);
    free(startVal);
    free(midVal);
    free(endVal);

    DPRINTF("UnitTest_Iterators(): Clear list.\n");
    l->Clear(l);
    DPRINTF("UnitTest_Iterators(): Destroy list.\n");
    ListDestroy(l);
}

int main(){
    UnitTest_Iterators();
    return 0;
}