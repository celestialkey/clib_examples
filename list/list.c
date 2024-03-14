#include "list.h"
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#define DPRINTF(...) {printf(__VA_ARGS__); fflush(stdout);}
#else
#define DPRINTF(...)
#endif

// Item Functions
static ListItem* _MakeItem(void* data){
    ListItem* li = (ListItem*)malloc(sizeof(ListItem));
    if(li==NULL) {
        DPRINTF("_MakeItem(): Malloc failed.\n");
        return NULL;
    }
    li->data = data;
    li->next = NULL;
    li->prev = NULL;
    return li;
}
// Iterator Functions
ListItem* _Peek (ListIter* iter){
    if(iter == NULL){
        DPRINTF("_Peek(): Bad iter.\n");
        return NULL;
    }
    if(iter->current == NULL) {
        DPRINTF("_Peek(): Current element is NULL.\n");
        return NULL;
    }
    return iter->current->next;
}
ListItem* _Next (ListIter* iter){
    if(iter == NULL){
        DPRINTF("_Next(): Bad iter.\n");
        return NULL;
    }
    if(iter->current == NULL) {
        DPRINTF("_Next(): Current element is NULL.\n");
        return NULL;
    }
    iter->current = iter->current->next;
    return iter->current;
}
uint8_t   _IsEnd(ListIter* iter){
    if(iter == NULL){
        DPRINTF("_IsEnd(): Bad iter.\n");
        return 1;
    }
    if(iter->current == NULL){
        return 1;
    }
    return 0;
}

// List Functions
ListIter*   _Begin  (List* list){
    if(list == NULL){
        DPRINTF("_Begin(): Null list passed in.\n");
        return NULL;
    }
    if(list->_head == NULL){
        DPRINTF("_Begin(): List is empty.\n");
        return NULL;
    }
    return list->Iterator(list);
}
ListIter*   _End    (List* list){
    if(list == NULL) {
        DPRINTF("_End(): Null list passed in.\n");
        return NULL;
    }
    if(list->_head == NULL){
        DPRINTF("_End(): List is empty.\n");
        return NULL;
    }
    ListIter* iter = list->Iterator(list);
    if(iter == NULL){
        DPRINTF("_End(): Bad iter.\n");
        return NULL;
    }
    while(iter->Peek(iter) != NULL){
        iter->Next(iter);
    }
    return iter;
}
ListIter*   _Iterator(List* list){
    if(list == NULL) {
        DPRINTF("_Iterator(): Null list passed in.\n");
        return NULL;
    }
    // Create a new iterator
    ListIter* iter = (ListIter*)malloc(sizeof(ListIter));
    if(iter==NULL) {
        DPRINTF("_Iterator(): Bad iter.\n");
        return NULL;
    }
    iter->current = list->_head;
    iter->Peek = _Peek;
    iter->Next = _Next;
    iter->IsEnd= _IsEnd;
    return iter;
}
uint8_t     _Insert (List* list, uint64_t index, void* data){
    if(list == NULL) return 0;
    if(index > list->_size) return 0;
   
    ListIter* iter = list->At(list, index);
    if(iter == list->End(list)){
        ListIterDestroy(iter);
        return 0;
    }

    ListItem* li = _MakeItem(data);
    if(li==NULL){
        ListIterDestroy(iter);
        return 0;
    }
    // Insert the item at the new location
    ListItem* prev = iter->current->prev;
    ListItem* next = iter->current->next;
    li->prev = prev;
    li->next = next;
    if(prev!=NULL) { prev->next = li; }
    if(next!=NULL) { next->prev = li; }
    list->_size++;
    ListIterDestroy(iter);
    return 1;
}
ListIter*   _At     (List* list, uint64_t index){
    //DPRINTF("_At(): Called.\n");
    if(list==NULL) {
        DPRINTF("_At(): Null list passed in.\n");
        return NULL;
    }
    if(list->Size(list) <= index){
        DPRINTF("_At(): requested index greater than list size.\n");
        return NULL;
    }
    ListIter* iter = list->Iterator(list);
    if(list==NULL) {
        DPRINTF("_At(): Bad iter.\n");
        return NULL;
    }
    for(uint64_t i=0; i<index; i++){
        iter->Next(iter);
        if(iter->IsEnd(iter)){
            //DPRINTF("_At(%lu/%lu): Searching for index...\n", index, i);
            if(i != index){
                DPRINTF("_At(): Index out of range.\n");
                return NULL;
            }
            else break;
        }
    }
    return iter;
}
uint8_t     _Erase  (List* list, uint64_t index){
    //DPRINTF("_Erase(): Called...\n");
    if(list == NULL) {
        DPRINTF("_Erase(): Null list passed in.\n");
        return 0;
    }
    // Create an iterator
    ListIter* iter = list->At(list, index);
    if(iter==NULL) {
        DPRINTF("_Erase(): Bad iter.\n");
        return 0;
    }
    //DPRINTF("_Erase(): After At\n");

    if(index == 0){
        //DPRINTF("_Erase(): Index 0\n");
        ListItem* next =iter->current->next;
        if(next != NULL){
            next->prev = NULL;
            list->_head = next;
        }
        //DPRINTF("_Erase(): Free item.\n");
        ListItemDestroy(iter->current);
        //DPRINTF("_Erase(): Free Iter.\n");
        ListIterDestroy(iter);
        list->_size--;
        return 1;
    }

    // Repair the next and prev of links
    ListItem* prev = iter->current->prev;
    ListItem* next = iter->current->next;
    if(prev != NULL){ prev->next = iter->current->next;}
    if(next != NULL){ next->prev = iter->current->prev;}

    // Destroy the current list item data
    if(iter->current->data != NULL){
        free(iter->current->data);
    }
    // Destroy the list item container
    ListItemDestroy(iter->current);
    // Destroy the iterator
    ListIterDestroy(iter);
    list->_size--;
    return 1;
}
uint64_t    _Size   (List* list){
    if(list==NULL) return 0;
    return list->_size;
}
void        _Clear  (List* list){
    if(list==NULL) return;
    while(list->Size(list)){
        //DPRINTF("_Clear(): Pre-Erase size %lu.\n", list->Size(list));
        if(!list->Erase(list, 0)){
            DPRINTF("_Clear(): Erase failed on attempted free.\n");
            break;
        }
        //DPRINTF("_Clear(): Post-Erase size %lu.\n", list->Size(list));
    }
}
void*       _Remove(List* list, void* source, uint64_t size){
    if(list == NULL) return 0;
    if(source == NULL) return 0;
    if(size == 0) return 0;

    ListIter* iter = list->Iterator(list);
    if(iter == NULL) return 0;
    
    uint64_t i=0;
    while(iter != NULL){
        if(memcmp(iter->current->data, source, size)){
            void* data = iter->current->data;
            if(!list->Erase(list, i)){
                DPRINTF("_Remove(): Located memory, but failed to remove it.\n");
            }
            ListIterDestroy(iter);
            return data;
        }
        i++;
        iter->Next(iter);
    }
    ListIterDestroy(iter);
    return NULL;
}
void*       _Get(List* list, uint64_t index){
    if(list == NULL){
        DPRINTF("_Get(): Null list passed in.\n");
        return NULL;
    }
    ListIter* iter = list->At(list, index);
    if(iter == NULL){
        DPRINTF("_Get(): Bad iter.\n");
        return NULL;
    }

    void* data = iter->current->data;
    ListIterDestroy(iter);
    return data;
}
void*       _Back(List* list){
    if(list==NULL) return NULL;
    // Grab an iter
    ListIter* iter = list->End(list);
    if(iter==NULL) return NULL;
    if(iter->current==NULL) return NULL;
    // Extract the data
    void* data = iter->current->data;
    ListIterDestroy(iter);
    return data;
}
void*       _Front(List* list){
    if(list==NULL) return NULL;
    // Grab an iter
    ListIter* iter = list->Begin(list);
    if(iter==NULL) return NULL;
    void* data = iter->current->data;
    ListIterDestroy(iter);
    return data;
}
void*       _Pop_Back(List* list){
    if(list==NULL) return NULL;
    // Grab an iter
    ListIter* end = list->End(list);
    if(end==NULL) return NULL;
    if(end->current==NULL) return NULL;

    // Extract the data
    void* data = end->current->data;

    if(end->current->prev != NULL){
        end->current->prev->next = NULL;
    } else {
        list->_head = NULL;
    }
    // Destroy the item container (Shouldn't destroy the data)
    ListItemDestroy(end->current);
    // Destroy the itor
    ListIterDestroy(end);
    list->_size--;
    return data;
}
void*       _Pop_Front(List* list){
    if(list==NULL) return NULL;
    // Grab an iter
    ListIter* start = list->Begin(list);
    if(start==NULL) return NULL;
    if(start->current == NULL) return NULL;

    // Extract the data
    void* data = start->current->data;
    
    // Adjust the links
    ListItem* next = start->current->next;
    list->_head = next;
    
    if(next) next->prev = NULL;
    // Destroy the item (shouldn't destroy data)
    ListItemDestroy(start->current);
    // Destroy the itor
    ListIterDestroy(start);
    list->_size--;
    return data;
}
uint8_t     _Push_Front(List* list, void* data){
    ListItem* newItem = _MakeItem(data);
    if(newItem == NULL){
        DPRINTF("_Push_Front(): _MakeItem() failed!\n");
        return 0;
    }
    ListIter* start = list->Begin(list);
    if(start == NULL){
        list->_head = newItem;
        DPRINTF("_Push_Front(): Added first element to list.\n");
        ListIterDestroy(start);
        list->_size++;
        return 1;
    }

    // Update the first element to have a prev
    list->_head->prev = newItem;
    // Update the new element to point to the old first
    newItem->next = list->_head;
    // Update the head to be the new element
    list->_head = newItem;
    list->_size++;
    ListIterDestroy(start);
    return 1;
}
uint8_t     _Push_Back(List* list, void* data){
    if(list==NULL){
        DPRINTF("_Push_Back(): Null list passed in.\n");
        return 0;
    }
    ListItem* newItem = _MakeItem(data);
    if(newItem == NULL){
        DPRINTF("_Push_Back(): Bad ListItem.\n");
        return 0;
    }
    if(list->_head == NULL){
        // Set this item as the first element
        list->_head = newItem;
        DPRINTF("_Push_Back(): Added first element to the list.\n");
        list->_size++;
        return 1;
    }

    ListIter* end = list->End(list);
    if(end == NULL){
        DPRINTF("_Push_Back(): Bad iter.\n");
        return 0;
    }
    end->current->next = newItem;
    newItem->prev = end->current;
    list->_size++;
    return 1;
}
uint8_t     _Empty(List* list){
    return (list->Size(list)==0)?1:0;
}


List* ListCreate(){
    List* l = (List*)malloc(sizeof(List));
    if(l==NULL) return NULL;
    l->Push_Front=_Push_Front;
    l->Pop_Front= _Pop_Front;
    l->Iterator = _Iterator;
    l->Push_Back= _Push_Back;
    l->Pop_Back = _Pop_Back;
    l->Insert   = _Insert;
    l->Remove   = _Remove;
    l->Begin    = _Begin;
    l->Clear    = _Clear;
    l->Erase    = _Erase;
    l->Front    = _Front;
    l->Empty    = _Empty;
    l->Back     = _Back;
    l->Size     = _Size;
    l->Get      = _Get;
    l->End      = _End;
    l->At       = _At;

    l->_head = NULL;
    l->_size = 0;
    return l;
}
void ListDestroy(List* list){
    if(list==NULL) return;
    //DPRINTF("ListDestroy(): Attempt to free list.\n");
    list->Clear(list);
}
void ListItemDestroy(ListItem* item){
    if(item == NULL) return;
    //DPRINTF("ListItemDestroy(): Attempt to free item.\n");
    free(item);
    item=NULL;
}
void ListIterDestroy(ListIter* iter){
    if(iter == NULL) return;
    //DPRINTF("ListIterDestroy(): Attempt to free Iter.\n");
    // Iterators don't need their data destroyed. 
    // Just the iterator itself needs cleaning.
    free(iter);
    iter = NULL;
}

//