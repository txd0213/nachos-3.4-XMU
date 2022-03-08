// dllist.cc
#include "copyright.h"
#include "dllist.h"

DLList::DLList()
{
    first = last = NULL;
}

DLList::~DLList()
{
    while (Remove(NULL) != NULL)
        ;
}

void DLList::Prepend(void *item)
{
    DLLElement *element = new DLLElement(item, 0);
    if (IsEmpty())
    {
        first = last = element;
        element->key = 0;
    }
    else
    {
        element->next = first;
        element->prev = NULL;
        element->key = first->key - 1;

        first->prev = element;
        first = element;
    }
}

void DLList::Append(void *item)
{
    DLLElement *element = new DLLElement(item, 0);
    if (IsEmpty())
    {
        first = last = element;
        element->key = 0;
    }

    else
    {
        element->next = NULL;
        element->prev = last;
        element->key = last->key + 1;

        last->next = element;
        last = element;
    }
}

void *DLList::Remove(int *keyPtr)
{
    DLLElement *element = first;
    void *thing;

    if (IsEmpty())
        return NULL;

    thing = first->item;
    if (first == last)
    { // dllist had one item, now has none
        first = NULL;
        last = NULL;
    }
    else
    {
        first = element->next;
        first->prev = NULL;
    }
    if (keyPtr != NULL)
        *keyPtr = element->key;
    delete element;
    return thing;
}

bool DLList::IsEmpty()
{
    return first == NULL ? TRUE : FALSE;
}

void DLList::SortedInsert(void *item, int sortKey)
{
    DLLElement *element = new DLLElement(item, sortKey);
    DLLElement *ptr; // keep track

    if (IsEmpty())
    {   // if list is empty
        first = element;
        last = element;
    }
    else if (sortKey < first->key)
    {
        // item goes on front of list
        element->next = first;
        element->prev = NULL;

        first->prev = element;
        first = element;
    }
    else
    { // look for first elt in list bigger than item
        for (ptr = first; ptr->next != NULL; ptr = ptr->next)
        {
            if (sortKey < ptr->next->key)
            {
                element->next = ptr->next;
                element->prev = ptr;
                ptr->next = element;
                element->next->prev = element;
                return;
            }
        }
        // item goes at end of list
        element->prev = last;
        element->next = NULL;

        last->next = element;
        last = element;
    }
}

void *DLList::SortedRemove(int sortKey)
{
    void *thing;
    DLLElement *ptr; // keep track

    if (IsEmpty())
        return NULL;

    // go at the begin of dllist
    ptr = first;
    if (ptr->key == sortKey)
    {
        thing = ptr->item;
        first = ptr->next;
        ptr->next->prev = NULL;
        delete ptr;
        return thing;
    }

    for (ptr = first->next; ptr->next != NULL; ptr = ptr->next)
    {
        if (sortKey == ptr->key)
        {
            ptr->next->prev = ptr->prev;
            ptr->prev->next = ptr->next;
            thing = ptr->item;
            delete ptr;
            return thing;
        }
    }

    // go at the end of dllist
    if (ptr->key == sortKey)
    {
        thing = ptr->item;
        last = ptr->prev;
        ptr->prev->next = NULL;
        delete ptr;
        return thing;
    }

    return NULL;
}