#ifndef QUEUE_SERVICE_HPP
#define QUEUE_SERVICE_HPP

#include "processService.hpp"
#include <queue>
#include <vector>

class QueueService {
private:
    std::queue<Process*> readyQueue;
    
public:
    void addProcess(Process* process);
    Process* getNextProcess();
    bool isEmpty() const;
    int size() const;
};

#endif
