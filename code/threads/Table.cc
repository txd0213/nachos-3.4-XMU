#include "copyright.h"
#include "Table.h"

using namespace synch_sleep;

extern Thread* currentThread;

Table::Table(int size)
{
    this->maxTableSize = size;
    this->tableLock = new Lock("Table");

    table = (void **)malloc(sizeof(void *) * maxTableSize);
    ASSERT(table != NULL);
    memset(table, 0, sizeof(void *) * maxTableSize);
}

Table::~Table()
{
    delete table;
    delete tableLock;
}

int Table::Alloc(void *object)
{
    tableLock->Acquire();

    int now = 0;
    while (table[now] != NULL && now < maxTableSize)
        now++;
    if (now == maxTableSize)
        now = -1;
    else
        table[now] = object;

    DEBUG('T',"%d:%s was inserted into table by %s\n",now,(char*)object,currentThread->getName());
    tableLock->Release();
    
    return now;
}

void *Table::Get(int index)
{
    ASSERT(index >= 0 && index < maxTableSize);

    tableLock->Acquire();

    void *p = index >= maxTableSize ? NULL : table[index];
    DEBUG('T',"%d:%s was got by %s\n",index, (char*)p,currentThread->getName());

    tableLock->Release();

    return p;
}

void Table::Release(int index)
{
    ASSERT(index >= 0 && index < maxTableSize);

    tableLock->Acquire();

    void* p=table[index];
    table[index] = NULL;

    DEBUG('T',"%d:%s was released by %s\n",index,(char*)p,currentThread->getName());

    tableLock->Release();
}