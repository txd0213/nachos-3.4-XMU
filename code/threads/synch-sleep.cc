// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch-sleep.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) 
{
    name = debugName;
    threadGetLock = new Thread("thread get lock");
    lockValue = 0;
    queue = new List;
}
Lock::~Lock() 
{
    delete threadGetLock;
    delete queue;
}
void Lock::Acquire()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    while(lockValue == 1){//locked
        queue->Append((void *)currentThread);
        currentThread->Sleep();
    }
    lockValue = 1;
    threadGetLock = currentThread;
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

void Lock::Release()
{
    ASSERT(isHeldByCurrentThread());

    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts

    threadGetLock = NULL;
    lockValue = 0;
    thread = (Thread *)queue->Remove();
    if(thread != NULL){
        scheduler->ReadyToRun(thread);
    }
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

bool Lock::isHeldByCurrentThread()
{
    if(currentThread == threadGetLock && lockValue == 1){
        return true;
    }else{
        return false;
    }
}

Condition::Condition(char* debugName)
{
    name = debugName;
    queue = new List;
}
Condition::~Condition()
{
    delete queue;
}

// how to use wait
//// first: lock->acquire
//// then: wait()
void Condition::Wait(Lock* conditionLock)
{
    // when a thread use wait_cv, it already get lock
    // the lock means you can watch cv and judge by cv
    ASSERT(conditionLock->isHeldByCurrentThread());

    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // 1st: add currentThread to wait queue
    queue->Append((void *)currentThread);
    // 2nd: free the lock, because no need to use cv
    conditionLock->Release();
    // 3rd: go to sleep
    currentThread->Sleep();

    // 4th: when waked up, get the lock and return
    conditionLock->Acquire();

    (void) interrupt->SetLevel(oldLevel);
}

// how to use signal
//// first: lock->acquire
//// then: signal()
void Condition::Signal(Lock* conditionLock)
{
    ASSERT(conditionLock->isHeldByCurrentThread());

    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    Thread *thread;
    thread = (Thread *)queue->Remove();
    if(thread != NULL){
        scheduler->ReadyToRun(thread);
    }

    (void) interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock* conditionLock)
{
    ASSERT(conditionLock->isHeldByCurrentThread());

    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    Thread *thread;    
    thread = (Thread *)queue->Remove();
    while(thread != NULL){
        scheduler->ReadyToRun(thread);
        thread = (Thread *)queue->Remove();
    }
    
    (void) interrupt->SetLevel(oldLevel);
}
