#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include "copyright.h"
#include "synch-sem.h"
#include "synch-sleep.h"
#include "synch.h"


class BoundedBuffer {
   public:
     // create a bounded buffer with a limit of 'maxsize' bytes
     BoundedBuffer(int maxsize);
     ~BoundedBuffer();
     
     // read 'size' bytes from the bounded buffer, storing into 'data'.
     // ('size' may be greater than 'maxsize')
     void Read(void *data, int size);
     void _read(void *data, int size);  
     
     // write 'size' bytes from 'data' into the bounded buffer.
     // ('size' may be greater than 'maxsize')
     void Write(void *data, int size);
     void _write(void *data, int size);

   private:
     synch_sleep::Condition *notFull,*notEmpty;
     synch_sleep::Lock* bufferLock;

     char* buffer;
     int count,nextin,nextout;
     int maxSize;

};

#endif

