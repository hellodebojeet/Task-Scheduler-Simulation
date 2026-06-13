#ifndef METRICS_HPP
#define METRICS_HPP

#include <chrono>
#include <string>
#include <vector>

struct CpuMetrics {
    double totalUsage;
    double userUsage;
    double systemUsage;
    double idleUsage;
    std::chrono::system_clock::time_point timestamp;
    
    CpuMetrics() : totalUsage(0.0), userUsage(0.0), systemUsage(0.0), idleUsage(0.0) {}
};

struct SchedulerMetrics {
    int contextSwitches;
    int taskDispatchRate;
    int queueSize;
    int wakeups;
    double schedulingLatency;
    std::chrono::system_clock::time_point timestamp;
    
    SchedulerMetrics() : contextSwitches(0), taskDispatchRate(0), queueSize(0), 
                        wakeups(0), schedulingLatency(0.0) {}
};

struct TaskMetrics {
    int pid;
    std::string taskName;
    int priority;
    int weight;
    long long vruntime;
    long long cpuTime;
    std::string state;
    std::chrono::system_clock::time_point timestamp;
    
    TaskMetrics() : pid(0), priority(0), weight(0), vruntime(0), cpuTime(0), state("idle") {}
};

struct SystemMetrics {
    int totalTasks;
    int runningTasks;
    int sleepingTasks;
    int finishedTasks;
    double schedulerTickRate;
    double averageLatency;
    long long contextSwitchCount;
    double throughput;
    std::chrono::system_clock::time_point timestamp;
    
    SystemMetrics() : totalTasks(0), runningTasks(0), sleepingTasks(0), finishedTasks(0),
                     schedulerTickRate(0.0), averageLatency(0.0), contextSwitchCount(0), throughput(0.0) {}
};

struct VRuntimeMetrics {
    std::vector<long long> vruntimeValues;
    long long minVRuntime;
    long long avgVRuntime;
    std::chrono::system_clock::time_point timestamp;
    
    VRuntimeMetrics() : minVRuntime(0), avgVRuntime(0) {}
};

struct QueueMetrics {
    int currentLength;
    int peakLength;
    double avgLength;
    std::chrono::system_clock::time_point timestamp;
    
    QueueMetrics() : currentLength(0), peakLength(0), avgLength(0.0) {}
};

struct TimelineEvent {
    int pid;
    std::string taskName;
    long long startTime;
    long long endTime;
    std::string state; // running, waiting, blocked, completed
    
    TimelineEvent() : pid(0), startTime(0), endTime(0), state("idle") {}
};

#endif
