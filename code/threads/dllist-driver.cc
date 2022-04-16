// dllist-driver.cc
#ifndef dllist_driver_cc
#define dllist_driver_cc

#include "copyright.h"
#include "system.h"
#include "utility.h"
#include "dllist.h"
#include <string.h>

extern int error_type;

int thread_A[10] = {6,5,10};
int thread_B[10] = {1,9,3};

void GenerateN(DLList *dllist, int N)
{
    if (error_type > 0)
        while (N--)
        {
            // int keyValue=Random()%100;
            int keyValue = 0;
            if (!strcmp(currentThread->getName(), "main"))
                keyValue = thread_A[N];
            else
                keyValue = thread_B[N];
            int *item = new int;
            *item = keyValue;
            dllist->SortedInsert(item, keyValue);
            printf("[thread]:");
            currentThread->Print();
            printf("%d:%d has been inserted into the DLList\n", *item, keyValue);
            dllist->Show1();
            dllist->Show2();
        }
    else
        while (N--)
        {
            int keyValue = Random() % 100;
            int *item = new int;
            *item = keyValue;
            dllist->SortedInsert(item, keyValue);
            printf("[thread]:");
            currentThread->Print();
            printf("%d:%d has been inserted into the DLList\n", *item, keyValue);
        }
}

void RemoveN(DLList *dllist, int N)
{
    int *keyValue = NULL;
    if (error_type > 0)
        while (N--)
        {
            int *item = (int *)dllist->Remove(keyValue);
            printf("[thread]:");
            currentThread->Print();
            printf("%d has been removed from the DLList\n", *item);
            dllist->Show1();
            dllist->Show2();
        }
    else
        while (N--)
        {
            int *item = (int *)dllist->Remove(keyValue);
            printf("[thread]:");
            currentThread->Print();
            printf("%d has been removed from the DLList\n", *item);
        }
}

#endif
