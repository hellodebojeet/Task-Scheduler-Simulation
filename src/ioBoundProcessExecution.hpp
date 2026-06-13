#ifndef IO_BOUND_PROCESS_EXECUTION_HPP
#define IO_BOUND_PROCESS_EXECUTION_HPP

#include "processService.hpp"

class IOBoundProcessExecution {
public:
    static void execute(Process* process, int timeSlice);
};

#endif
