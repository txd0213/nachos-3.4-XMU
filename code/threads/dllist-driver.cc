// dllist-driver.cc
#include "copyright.h"
#include "utility.h"
#include "dllist.h"

void GenerateN(DLList dllist, int N)
{
    
    while(N--)
    {
        int keyValue=Random();
        dllist.SortedInsert(NULL,keyValue);
    }
}

void RemovesN(DLList dllist, int N)
{
    int *keyValue=NULL;
    while(N--)
    {
        dllist.Remove(keyValue);
    }
}