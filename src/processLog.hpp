#ifndef PROCESS_LOG_HPP
#define PROCESS_LOG_HPP

#include <string>

class ProcessLog {
private:
    int pid;
    long long startTime;
    long long endTime;
    
public:
    ProcessLog(int pid, long long startTime, long long endTime);
    int getPid() const;
    long long getStartTime() const;
    long long getEndTime() const;
    long long getDuration() const;
};

#endif
