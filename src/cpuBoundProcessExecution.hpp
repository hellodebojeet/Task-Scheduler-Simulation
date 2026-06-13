#ifndef CPU_BOUND_PROCESS_EXECUTION_HPP
#define CPU_BOUND_PROCESS_EXECUTION_HPP

#include "processService.hpp"

class CPUBoundProcessExecution {
public:
    static void execute(Process* process, int timeSlice);
};

#endif
