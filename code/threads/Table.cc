#include "copyright.h"
#include "Table.h"

using namespace synch_sleep;

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

    tableLock->Release();
    
    return now;
}

void *Table::Get(int index)
{
    ASSERT(index >= 0 && index < maxTableSize);

    tableLock->Acquire();
    void *p = index >= maxTableSize ? NULL : table[index];
    tableLock->Release();

    return p;
}

void Table::Release(int index)
{
    ASSERT(index >= 0 && index < maxTableSize);

    tableLock->Acquire();
    table[index] = NULL;
    tableLock->Release();
}