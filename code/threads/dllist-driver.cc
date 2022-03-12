// dllist-driver.cc
#ifndef dllist_driver_cc
#define dllist_driver_cc

#include "copyright.h"
#include "system.h"
#include "utility.h"
#include "dllist.h"
#include "system.h"


extern int error_type;

void GenerateN(DLList *dllist, int N)
{
    while(N--)
    {        
        int keyValue=Random()%100;
        int *item=new int;
        *item=keyValue-1;
        dllist->SortedInsert(item,keyValue);
        printf("[thread]:");
        currentThread->Print();
        printf("%d:%d has been inserted into the DLList\n",*item,keyValue);
    }
}

void RemoveN(DLList *dllist, int N)
{
    int *keyValue=NULL;
    while(N--)
    {
        int *item=(int*)dllist->Remove(keyValue);
        printf("[thread]:");
        currentThread->Print();
        printf("%d has been removed from the DLList\n",*item);
    }
}

#endif