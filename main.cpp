#include "cfs.hpp"
#include <iostream>
#include <vector>
#include <memory>

int main() {
    std::cout << "=== CFS Scheduler Simulation ===" << '\n';

    const std::vector<std::unique_ptr<Process>> processes = [] {
        std::vector<std::unique_ptr<Process>> procs;
        procs.push_back(std::make_unique<Process>(1, "Process 1", 0, 10, 1));
        procs.push_back(std::make_unique<Process>(2, "Process 2", 1, 5, 2));
        procs.push_back(std::make_unique<Process>(3, "Process 3", 2, 8, 1));
        procs.push_back(std::make_unique<Process>(4, "Process 4", 3, 3, 3));
        return procs;
    }();

    std::cout << "\nProcesses created:" << '\n';
    for (const auto& p : processes) {
        std::cout << "PID: " << p->getPid() << ", Name: " << p->getName()
                  << ", Arrival: " << p->getArrivalTime()
                  << ", Burst: " << p->getBurstTime()
                  << ", Priority: " << p->getPriority() << '\n';
    }

    CFS scheduler;
    std::cout << "\nRunning CFS scheduler..." << '\n';
    const auto logs = scheduler.schedule(processes);

    std::cout << "\nExecution Logs:" << '\n';
    for (const auto& log : logs) {
        std::cout << "PID: " << log->getPid()
                  << ", Start: " << log->getStartTime()
                  << ", End: " << log->getEndTime()
                  << ", Duration: " << log->getDuration() << '\n';
    }

    std::cout << "\nStatistics:" << '\n';
    std::cout << "Total processes: " << processes.size() << '\n';
    if (!logs.empty()) {
        std::cout << "Total execution time: " << logs.back()->getEndTime() << '\n';
    }

    std::cout << "\nSimulation complete!" << '\n';
    return 0;
}
