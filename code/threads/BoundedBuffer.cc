#include "copyright.h"
#include "BoundedBuffer.h"
//#include "system.h"

using namespace synch_sleep;

extern Thread *currentThread;

BoundedBuffer::BoundedBuffer(int maxsize)
{
    bufferLock = new Lock("BoundedBuffer");
    notEmpty = new Condition("BoundedBuffer_notEmpty");
    notFull = new Condition("BoundedBuffer_notFull");

    buffer = new char[maxsize];
    count = nextout = nextin = 0;
    maxSize = maxsize;
}

BoundedBuffer::~BoundedBuffer()
{
    delete bufferLock;
    delete notEmpty;
    delete notFull;
    delete buffer;
}

void BoundedBuffer::_write(void *data,int size)
{
    bufferLock->Acquire();
    while ((count + size) > maxSize)
        notFull->Wait(bufferLock);

    int i = 0;
    while (i < size)
    {
        ((char *)buffer)[nextin] = ((char *)data)[i], i++;
        nextin = (nextin + 1) % maxSize;
    }

    count += size,i=count;
    ASSERT(count <= maxSize);

    DEBUG('b', "%d bytes were written by %s", size, currentThread->getName());
    while(i--)
        DEBUG('b',"-");
    DEBUG('b',"\n");

    notEmpty->Signal(bufferLock);
    bufferLock->Release();
}

void BoundedBuffer::Write(void *data, int size)
{
    while (size--)
        _write(data,1);  
}

void BoundedBuffer::Read(void *data,int size)
{
    while (size--)
        _read(data,1);
}

void BoundedBuffer::_read(void *data, int size)
{
    bufferLock->Acquire();
    while (count < size)
        notEmpty->Wait(bufferLock);

    int i = 0;
    while (i < size)
    {
        ((char *)data)[i] = ((char *)buffer)[nextout], i++;
        nextout = (nextout + 1) % maxSize;
    }
    
    count -= size,i=count;
    ASSERT(count >= 0);
    
    DEBUG('b', "%d bytes were read by %s   ", size, currentThread->getName());
    while(i--)
        DEBUG('b',"-");
    DEBUG('b',"\n");

    notFull->Signal(bufferLock);
    bufferLock->Release();
}