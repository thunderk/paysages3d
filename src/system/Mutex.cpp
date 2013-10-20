#include "Mutex.h"

Mutex::Mutex()
{
}

// Transitional C-API

Mutex* mutexCreate()
{
    return new Mutex();
}

void mutexDestroy(Mutex* mutex)
{
    delete mutex;
}

void mutexAcquire(Mutex* mutex)
{
    mutex->acquire();
}

void mutexRelease(Mutex* mutex)
{
    mutex->release();
}
