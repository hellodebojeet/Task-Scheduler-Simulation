#include "queueService.hpp"

void QueueService::addProcess(Process* process) {
    readyQueue.push(process);
}

Process* QueueService::getNextProcess() {
    if (readyQueue.empty()) return nullptr;
    Process* process = readyQueue.front();
    readyQueue.pop();
    return process;
}

bool QueueService::isEmpty() const {
    return readyQueue.empty();
}

int QueueService::size() const {
    return readyQueue.size();
}
