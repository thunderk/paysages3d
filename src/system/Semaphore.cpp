#include "Semaphore.h"

Semaphore::Semaphore(int resources):
    QSemaphore(resources)
{
}
