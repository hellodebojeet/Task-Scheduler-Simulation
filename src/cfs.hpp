#ifndef CFS_HPP
#define CFS_HPP

#include <vector>
#include <ctime>
#include <memory>
#include "processService.hpp"
#include "processLog.hpp"
#include "queueService.hpp"
#include "cpuBoundProcessExecution.hpp"
#include "ioBoundProcessExecution.hpp"

class SchedulerMonitor;

class CFS {
private:
    std::shared_ptr<SchedulerMonitor> monitor;
    void createProcessLog(std::vector<std::unique_ptr<ProcessLog>>& logs, long long startTime, long long endTime, int pid);

public:
    CFS();
    void setMonitor(std::shared_ptr<SchedulerMonitor> mon);
    std::vector<std::unique_ptr<ProcessLog>> schedule(const std::vector<std::unique_ptr<Process>>& processList);
};

#endif
