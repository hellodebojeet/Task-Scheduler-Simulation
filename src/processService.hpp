#ifndef PROCESS_SERVICE_HPP
#define PROCESS_SERVICE_HPP

#include <string>
#include <vector>

class Process {
private:
    int pid;
    std::string name;
    long long arrivalTime;
    long long burstTime;
    int priority;
    long long remainingTime;

public:
    Process(int pid, std::string name, long long arrivalTime, long long burstTime, int priority);
    int getPid() const;
    const std::string& getName() const;
    long long getArrivalTime() const;
    long long getBurstTime() const;
    int getPriority() const;
    long long getRemainingTime() const;
    void setRemainingTime(long long time);
    void execute(long long time);
};

#endif
