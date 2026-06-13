#ifndef SCHEDULER_MONITOR_HPP
#define SCHEDULER_MONITOR_HPP

#include "metrics.hpp"
#include "ring_buffer.hpp"
#include <thread>
#include <atomic>
#include <memory>
#include <vector>

class SchedulerMonitor {
private:
    std::unique_ptr<std::thread> monitorThread;
    std::atomic<bool> running;
    RingBuffer<SchedulerMetrics, 600> metricsBuffer;
    RingBuffer<VRuntimeMetrics, 600> vruntimeBuffer;
    RingBuffer<QueueMetrics, 600> queueBuffer;
    RingBuffer<SystemMetrics, 600> systemBuffer;
    std::vector<TimelineEvent> timelineEvents;
    
    std::atomic<int> contextSwitchCount;
    std::atomic<int> taskDispatchCount;
    std::atomic<int> wakeupCount;
    double totalLatency;
    std::atomic<int> latencySamples;
    std::mutex latencyMutex;
    
    void monitorLoop();
    SchedulerMetrics collectSchedulerMetrics();
    VRuntimeMetrics collectVRuntimeMetrics();
    QueueMetrics collectQueueMetrics();
    SystemMetrics collectSystemMetrics();
    
public:
    SchedulerMonitor();
    ~SchedulerMonitor();
    
    void start();
    void stop();
    bool isRunning() const;
    
    void recordContextSwitch();
    void recordTaskDispatch();
    void recordWakeup();
    void recordLatency(double latency);
    
    void updateQueueSize(int size);
    void updateTaskStates(int total, int running, int sleeping, int finished);
    void updateVRuntime(const std::vector<long long>& vruntimeValues);
    
    SchedulerMetrics getLatestSchedulerMetrics();
    VRuntimeMetrics getLatestVRuntimeMetrics();
    QueueMetrics getLatestQueueMetrics();
    SystemMetrics getLatestSystemMetrics();
    
    std::vector<SchedulerMetrics> getSchedulerMetricsHistory();
    std::vector<VRuntimeMetrics> getVRuntimeMetricsHistory();
    std::vector<QueueMetrics> getQueueMetricsHistory();
    std::vector<SystemMetrics> getSystemMetricsHistory();
    
    void addTimelineEvent(const TimelineEvent& event);
    std::vector<TimelineEvent> getTimelineEvents();
    void clearTimelineEvents();
};

#endif
