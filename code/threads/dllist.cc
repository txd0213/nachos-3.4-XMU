// dllist.cc
#include "copyright.h"
#include "dllist.h"
#include "system.h"

using namespace synch_sleep;

extern int error_type;

DLList::DLList()
{
    first = last = NULL;
    lock = new synch_sleep::Lock("dllist lock");
    dllistEmpty = new synch_sleep::Condition("dllist empty condition");
}

DLList::~DLList()
{
    while (Remove(NULL) != NULL)
        ;
    delete lock;
    delete dllistEmpty;
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
    lock->Acquire();
    while(IsEmpty()){
        dllistEmpty->Wait(lock);
    }
    ASSERT(!IsEmpty());


    DLLElement *element = first;
    void *thing;

    // if (IsEmpty())
    //     return NULL;

    thing = first->item;
    if (first == last)
    { // dllist had one item, now has none
        first = NULL;
        last = NULL;
    }
    else
    {
        //bug 2
        if(error_type==2)
        {
            printf("[bug 2] Thread: ");
            currentThread->Print();
            printf("    Yield before remove first node (dllist.cc line78)\n\n");
            fflush(stdout);
            currentThread->Yield();
        }
        first = element->next;
        first->prev = NULL;
    }
    if (keyPtr != NULL)
        *keyPtr = element->key;
    delete element;


    ASSERT(thing != NULL);
    lock->Release();
    return thing;
}

bool DLList::IsEmpty()
{
    return first == NULL ? TRUE : FALSE;
}

void DLList::SortedInsert(void *item, int sortKey)
{
    lock->Acquire();

    DLLElement *element = new DLLElement(item, sortKey);
    DLLElement *ptr; // keep track

    //bug 3
    if(error_type==3)
        {
            printf("---[bug 3]--- Thread: ");
            currentThread->Print();
            printf("    Yield after 'new' a node space (dllist.cc line107)\n\n");
            fflush(stdout);
            currentThread->Yield();
        }
    if (IsEmpty())
    {   // if list is empty
        first = element;

        //bug 4
        if(error_type==4)
        {
            printf("---[bug 4]--- Thread: ");
            currentThread->Print();
            printf("    Yield when list is empty (dllist.cc line120)\n\n");
            fflush(stdout);
            currentThread->Yield();
        }
        last = element;
    }
    else if (sortKey < first->key)
    {
        // item goes on front of list
        element->next = first;
        element->prev = NULL;

        //bug 5
        if(error_type==5)
        {
            printf("---[bug 5]--- Thread: ");
            currentThread->Print();
            printf("    Yield when insert first node (dllist.cc line137)\n\n");
            fflush(stdout);
            currentThread->Yield();
        }
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

                //bug 6
                if(error_type==6)
                {
                    printf("---[bug 6]--- Thread: ");
                    currentThread->Print();
                    printf("    Yield when insert common node (dllist.cc line159)\n\n");
                    fflush(stdout);
                    currentThread->Yield();
                }
                element->next->prev = element;
                // return;
                break;
            }
        }
        // item goes at end of list
        if(ptr == last){
            element->prev = last;
            element->next = NULL;

            last->next = element;
            last = element;
        }
    }


    dllistEmpty->Signal(lock);
    lock->Release();
}

//lock used in SortedRemove can be ignored
void *DLList::SortedRemove(int sortKey)
{
    lock->Acquire();
    while(IsEmpty()){
        dllistEmpty->Wait(lock);
    }
    ASSERT(!IsEmpty());
    
    //--------------in lock--------------
    void *thing;
    DLLElement *ptr; // keep track

    // if (IsEmpty())
    //     return NULL;

    // goes at the begin of dllist
    ptr = first;
    if (ptr->key == sortKey)
    {
        thing = ptr->item;
        first = ptr->next;
        ptr->next->prev = NULL;
        // delete ptr;
        // return thing;
    }

    for (ptr = first->next; ptr->next != NULL; ptr = ptr->next)
    {
        if (sortKey == ptr->key)
        {
            ptr->next->prev = ptr->prev;
            ptr->prev->next = ptr->next;
            thing = ptr->item;

            break;
            // delete ptr;
            // return thing;
        }
    }

    // goes at the end of dllist
    if (ptr->key == sortKey)
    {
        thing = ptr->item;
        last = ptr->prev;
        ptr->prev->next = NULL;
        // delete ptr;
        // return thing;
    }

    ASSERT(thing != NULL);
    delete ptr;
    lock->Release();
    return thing;
}

void DLList::Show1()
{
    DLLElement *ptr;
    if(IsEmpty())
    {
        printf("show1 list is empty!\n");
    }
    else
    {
        printf("show1 list (first->last): ");
        for(ptr=first;ptr!=NULL;ptr=ptr->next)
        {
            printf("%d  ",ptr->key);
        }
        printf("\n\n");
    }
}

void DLList::Show2()
{
    DLLElement *ptr;
    if(IsEmpty())
    {
        printf("show2 list is empty!\n");
    }
    else
    {
        printf("show2 list (last->first): ");
        for(ptr=last;ptr!=NULL;ptr=ptr->prev)
        {
            printf("%d  ",ptr->key);
        }
        printf("\n\n");
    }
}
