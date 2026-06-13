#include "cpuBoundProcessExecution.hpp"

void CPUBoundProcessExecution::execute(Process* process, int timeSlice) {
    process->execute(timeSlice);
}
