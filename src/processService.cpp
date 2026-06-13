#include "processService.hpp"

Process::Process(int pid, std::string name, long long arrivalTime, long long burstTime, int priority)
    : pid(pid), name(std::move(name)), arrivalTime(arrivalTime), burstTime(burstTime),
      priority(priority), remainingTime(burstTime) {}

int Process::getPid() const { return pid; }
const std::string& Process::getName() const { return name; }
long long Process::getArrivalTime() const { return arrivalTime; }
long long Process::getBurstTime() const { return burstTime; }
int Process::getPriority() const { return priority; }
long long Process::getRemainingTime() const { return remainingTime; }

void Process::setRemainingTime(long long time) { remainingTime = time; }

void Process::execute(long long time) {
    if (remainingTime > 0) {
        remainingTime -= time;
        if (remainingTime < 0) remainingTime = 0;
    }
}
