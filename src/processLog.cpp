#include "processLog.hpp"

ProcessLog::ProcessLog(int pid, long long startTime, long long endTime) 
    : pid(pid), startTime(startTime), endTime(endTime) {}

int ProcessLog::getPid() const { return pid; }
long long ProcessLog::getStartTime() const { return startTime; }
long long ProcessLog::getEndTime() const { return endTime; }
long long ProcessLog::getDuration() const { return endTime - startTime; }
