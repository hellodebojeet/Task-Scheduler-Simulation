#include "cpu_monitor.hpp"
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cmath>

CpuMonitor::CpuMonitor() : running(false), lastTotalUsage(0.0), lastUserUsage(0.0), lastSystemUsage(0.0) {}

CpuMonitor::~CpuMonitor() {
    stop();
}

void CpuMonitor::start() {
    if (running.load()) return;
    running.store(true);
    monitorThread = std::make_unique<std::thread>(&CpuMonitor::monitorLoop, this);
}

void CpuMonitor::stop() {
    if (!running.load()) return;
    running.store(false);
    if (monitorThread && monitorThread->joinable()) {
        monitorThread->join();
    }
}

bool CpuMonitor::isRunning() const {
    return running.load();
}

CpuMetrics CpuMonitor::collectMetrics() {
    CpuMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now();
    
    // Read /proc/stat for CPU usage
    std::ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        std::string line;
        if (std::getline(statFile, line)) {
            std::istringstream iss(line);
            std::string cpu;
            long long user, nice, system, idle, iowait, irq, softirq, steal;
            
            iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
            
            long long total = user + nice + system + idle + iowait + irq + softirq + steal;
            long long work = total - idle - iowait;
            
            static long long prevTotal = 0;
            static long long prevWork = 0;
            
            long long totalDelta = total - prevTotal;
            long long workDelta = work - prevWork;
            
            if (totalDelta > 0) {
                metrics.totalUsage = (double)workDelta / totalDelta * 100.0;
                metrics.userUsage = (double)user / total * 100.0;
                metrics.systemUsage = (double)system / total * 100.0;
                metrics.idleUsage = (double)idle / total * 100.0;
            }
            
            prevTotal = total;
            prevWork = work;
        }
        statFile.close();
    }
    
    // Fallback to simulated values if /proc/stat not available
    if (metrics.totalUsage == 0.0) {
        metrics.totalUsage = lastTotalUsage.load();
        metrics.userUsage = lastUserUsage.load();
        metrics.systemUsage = lastSystemUsage.load();
        metrics.idleUsage = 100.0 - metrics.totalUsage;
    }
    
    lastTotalUsage.store(metrics.totalUsage);
    lastUserUsage.store(metrics.userUsage);
    lastSystemUsage.store(metrics.systemUsage);
    
    return metrics;
}

void CpuMonitor::monitorLoop() {
    while (running.load()) {
        CpuMetrics metrics = collectMetrics();
        metricsBuffer.push(metrics);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

CpuMetrics CpuMonitor::getLatestMetrics() {
    CpuMetrics metrics;
    metricsBuffer.peek(metrics);
    return metrics;
}

std::vector<CpuMetrics> CpuMonitor::getMetricsHistory() {
    return metricsBuffer.snapshot();
}

double CpuMonitor::getCurrentTotalUsage() const {
    return lastTotalUsage.load();
}

double CpuMonitor::getCurrentUserUsage() const {
    return lastUserUsage.load();
}

double CpuMonitor::getCurrentSystemUsage() const {
    return lastSystemUsage.load();
}
