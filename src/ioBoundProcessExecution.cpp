#include "ioBoundProcessExecution.hpp"

void IOBoundProcessExecution::execute(Process* process, int timeSlice) {
    process->execute(timeSlice);
}
