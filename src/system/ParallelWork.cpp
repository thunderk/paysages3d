#include "ParallelWork.h"

#include "Thread.h"
#include "System.h"
#include "Semaphore.h"
#include "Mutex.h"
#include "ParallelWorker.h"
#include <cassert>

/**
 * Compatibility class for code that uses ParallelUnitFunction.
 */
class ParallelWorkerCompat : public ParallelWorker {
  public:
    ParallelWorkerCompat(ParallelWork *work, ParallelWork::ParallelUnitFunction func, void *data)
        : ParallelWorker(), work(work), func(func), data(data) {
    }

    virtual void processParallelUnit(int unit) {
        func(work, unit, data);
    }

  private:
    ParallelWork *work;
    ParallelWork::ParallelUnitFunction func;
    void *data;
};

/**
 * Thread sub-class to perform units of work
 */
class ParallelWork::ParallelThread : public Thread {
  public:
    ParallelThread(ParallelWork *work) : Thread(), work(work), sem(0) {
        interrupted = false;
        unit = -1;
    }

    void feedUnit(int unit) {
        this->unit = unit;
        sem.release();
    }

    void interrupt() {
        interrupted = true;
        sem.release();
    }

  protected:
    virtual void run() override {
        while (unit >= 0 or not interrupted) {
            // Wait for a unit (or interrupt)
            sem.acquire();

            // Process the unit
            if (unit >= 0) {
                work->worker->processParallelUnit(unit);
                unit = -1;
                work->returnThread(this);
            }
        }
    }

  private:
    ParallelWork *work;
    Semaphore sem;
    int unit;
    bool interrupted;
};

ParallelWork::ParallelWork(ParallelWorker *worker, int units) {
    this->units = units;
    this->running = 0;
    this->worker = worker;
    this->worker_compat = false;
}

ParallelWork::ParallelWork(ParallelUnitFunction func, int units, void *data) {
    this->units = units;
    this->running = 0;
    this->worker = new ParallelWorkerCompat(this, func, data);
    this->worker_compat = true;
}

ParallelWork::~ParallelWork() {
    assert(not running);
    if (worker_compat) {
        delete worker;
    }
}

int ParallelWork::perform(int thread_count) {
    int i, done;
    assert(not running);

    // Get thread count
    if (thread_count <= 0) {
        thread_count = System::getCoreCount();
    }
    if (thread_count > PARALLEL_MAX_THREADS) {
        thread_count = PARALLEL_MAX_THREADS;
    }
    this->thread_count = thread_count;
    running = 1;

    // Init threads
    semaphore = new Semaphore(thread_count);
    mutex = new Mutex();
    threads = new ParallelThread *[thread_count];
    available = new ParallelThread *[thread_count];
    available_offset = 0;
    available_length = thread_count;
    for (i = 0; i < thread_count; i++) {
        threads[i] = available[i] = new ParallelThread(this);
        threads[i]->start();
    }

    // Perform all units
    for (done = 0; done < units; done++) {
        // Take first available thread
        semaphore->acquire();
        mutex->acquire();
        ParallelThread *thread = available[available_offset];
        available_offset++;
        if (available_offset >= thread_count) {
            available_offset = 0;
        }
        available_length--;
        mutex->release();

        // Feed the unit to it
        thread->feedUnit(done);
    }

    // Wait for all threads to end, then cleanup
    for (i = 0; i < thread_count; i++) {
        threads[i]->interrupt();
    }
    for (i = 0; i < thread_count; i++) {
        threads[i]->join();
        delete threads[i];
    }
    delete[] threads;
    delete[] available;
    delete semaphore;
    delete mutex;

    running = 0;
    return done;
}

void ParallelWork::interrupt() {
    worker->interrupt();
}

void ParallelWork::returnThread(ParallelWork::ParallelThread *thread) {
    mutex->acquire();
    available[(available_offset + available_length) % thread_count] = thread;
    available_length++;
    semaphore->release();
    mutex->release();
}
