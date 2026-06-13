#include "cfs.hpp"
#include "../monitoring/scheduler_monitor.hpp"
#include <algorithm>
#include <iostream>
#include <chrono>

CFS::CFS() : monitor(nullptr) {}

void CFS::setMonitor(std::shared_ptr<SchedulerMonitor> mon) {
    monitor = std::move(mon);
}

void CFS::createProcessLog(std::vector<std::unique_ptr<ProcessLog>>& logs, long long startTime, long long endTime, int pid) {
    logs.push_back(std::make_unique<ProcessLog>(pid, startTime, endTime));
}

std::vector<std::unique_ptr<ProcessLog>> CFS::schedule(const std::vector<std::unique_ptr<Process>>& processList) {
    std::vector<std::unique_ptr<ProcessLog>> logs;
    std::vector<const Process*> readyQueue;
    long long currentTime = 0;
    int completedProcesses = 0;
    const int totalProcesses = processList.size();

    while (completedProcesses < totalProcesses) {
        for (const auto& process : processList) {
            if (process->getArrivalTime() <= currentTime && process->getRemainingTime() > 0) {
                const auto inQueue = std::any_of(readyQueue.begin(), readyQueue.end(),
                    [pid = process->getPid()](const Process* p) { return p->getPid() == pid; });
                if (!inQueue) {
                    readyQueue.push_back(process.get());
                    if (monitor) {
                        monitor->recordWakeup();
                    }
                }
            }
        }

        if (monitor) {
            monitor->updateQueueSize(static_cast<int>(readyQueue.size()));
            monitor->updateTaskStates(totalProcesses, static_cast<int>(readyQueue.size()),
                                    totalProcesses - static_cast<int>(readyQueue.size()) - completedProcesses,
                                    completedProcesses);
        }

        if (readyQueue.empty()) {
            ++currentTime;
            continue;
        }

        const Process* selected = readyQueue[0];
        for (const auto* process : readyQueue) {
            if (process->getRemainingTime() < selected->getRemainingTime()) {
                selected = process;
            }
        }

        if (monitor) {
            monitor->recordContextSwitch();
            monitor->recordTaskDispatch();
        }

        const long long startTime = currentTime;
        const auto start = std::chrono::high_resolution_clock::now();

        constexpr int timeSlice = 1;
        CPUBoundProcessExecution::execute(const_cast<Process*>(selected), timeSlice);
        currentTime += timeSlice;

        const auto end = std::chrono::high_resolution_clock::now();
        const double latency = std::chrono::duration<double, std::milli>(end - start).count();

        if (monitor) {
            monitor->recordLatency(latency);
        }

        createProcessLog(logs, startTime, currentTime, selected->getPid());

        if (monitor) {
            std::vector<long long> vruntimeValues;
            for (const auto* p : readyQueue) {
                vruntimeValues.push_back(p->getRemainingTime() * 1024LL);
            }
            monitor->updateVRuntime(vruntimeValues);
        }

        if (selected->getRemainingTime() == 0) {
            readyQueue.erase(std::remove(readyQueue.begin(), readyQueue.end(), selected),
                           readyQueue.end());
            ++completedProcesses;
        }
    }

    return logs;
}
