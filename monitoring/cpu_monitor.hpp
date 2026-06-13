#ifndef CPU_MONITOR_HPP
#define CPU_MONITOR_HPP

#include "metrics.hpp"
#include "ring_buffer.hpp"
#include <thread>
#include <atomic>
#include <memory>

class CpuMonitor {
private:
    std::unique_ptr<std::thread> monitorThread;
    std::atomic<bool> running;
    RingBuffer<CpuMetrics, 600> metricsBuffer; // 60 seconds @ 10Hz
    std::atomic<double> lastTotalUsage;
    std::atomic<double> lastUserUsage;
    std::atomic<double> lastSystemUsage;
    
    void monitorLoop();
    CpuMetrics collectMetrics();
    
public:
    CpuMonitor();
    ~CpuMonitor();
    
    void start();
    void stop();
    bool isRunning() const;
    
    CpuMetrics getLatestMetrics();
    std::vector<CpuMetrics> getMetricsHistory();
    
    double getCurrentTotalUsage() const;
    double getCurrentUserUsage() const;
    double getCurrentSystemUsage() const;
};

#endif
