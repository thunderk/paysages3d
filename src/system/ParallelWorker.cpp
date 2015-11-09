#include "ParallelWorker.h"

ParallelWorker::ParallelWorker() {
    interrupted = false;
}

ParallelWorker::~ParallelWorker() {
}

void ParallelWorker::interrupt() {
    interrupted = true;
}
