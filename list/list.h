#ifndef __LIST_H__
#define __LIST_H__

#include <stdint.h>

typedef struct ListItem ListItem;
typedef struct ListIter ListIter;
typedef struct List List;

struct ListItem {
    void* data;
    ListItem* next;
    ListItem* prev;
};

/*
A basic iterator that requires all methods called to have itself passed in.
e.g. myIter->Next(myIter);
*/
struct ListIter {
    // The current container that stores data.
    ListItem*   current;
    // Check the status of the next item in the link
    ListItem*   (*Peek)(ListIter*);
    // Get and move the iterator to the next link in the chain
    ListItem*   (*Next)(ListIter*);
    // Is this the last item in the link?
    uint8_t     (*IsEnd)(ListIter*);
} ;

/*
A self contained list that emulated the C++ implementation. All methods called
with this struct require you to pass itself into it.
e.g. mylist->Size(myList); 
or
void* data = myList->Get(myList, 12);
*/
struct List {
    ListItem*   _head;
    uint64_t    _size;

    // Get the number of elements stored in the list
    uint64_t    (*Size)     (List*);
    // Insert a new element at position
    uint8_t     (*Insert)   (List*, uint64_t, void*);
    // Retrieve an iterator for a specific position
    ListIter*   (*At)       (List*, uint64_t);
    // Erase the element at the indicated index. (Does NOT free the stored data, only the container)
    uint8_t     (*Erase)    (List*, uint64_t);
    // Erase all elements in the list. (Does NOT free the stored data in each element, only the container)
    void        (*Clear)    (List*);
    // Retrieve an iterator for the list. Requires a call to ListIterDestroy to free.
    ListIter*   (*Iterator) (List*);
    // Retrieve an iterator from the start of the list. Requires a call to ListIterDestroy to free.
    ListIter*   (*Begin)    (List*);
    // Retrieve an iterator from the end of the list. Requires a call to ListIterDestroy to free.
    ListIter*   (*End)      (List*);

    // Add an element to the back of the list
    uint8_t     (*Push_Back)(List*, void*);
    // Add an element to the front of the list
    uint8_t     (*Push_Front)(List*, void*);
    // Get the data contained within an element at a particular index.
    void*       (*Get)      (List*, uint64_t);
    // Get the data stored at the start of the list. Destroyed the container on retrieval
    void*       (*Pop_Front)(List*);
    // Get the data stored at the end of the list. Destroyed the container on retrieval
    void*       (*Pop_Back) (List*);
    uint8_t     (*Empty)    (List*);
    // Get the data stored at the start of the list.
    void*       (*Front)    (List*);
    // Get the data stored at the end of the list.
    void*       (*Back)     (List*);
    // Attempt to remove a container from list based off memory comparisons
    void*       (*Remove)   (List*, void*, uint64_t);
} ;

List* ListCreate();

void ListDestroy(List* list);
void ListItemDestroy(ListItem* item);
void ListIterDestroy(ListIter* iter);
#endif
