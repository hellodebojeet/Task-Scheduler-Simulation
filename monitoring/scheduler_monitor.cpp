#include "scheduler_monitor.hpp"
#include <algorithm>
#include <numeric>

SchedulerMonitor::SchedulerMonitor() 
    : running(false), contextSwitchCount(0), taskDispatchCount(0), 
      wakeupCount(0), totalLatency(0.0), latencySamples(0) {}

SchedulerMonitor::~SchedulerMonitor() {
    stop();
}

void SchedulerMonitor::start() {
    if (running.load()) return;
    running.store(true);
    monitorThread = std::make_unique<std::thread>(&SchedulerMonitor::monitorLoop, this);
}

void SchedulerMonitor::stop() {
    if (!running.load()) return;
    running.store(false);
    if (monitorThread && monitorThread->joinable()) {
        monitorThread->join();
    }
}

bool SchedulerMonitor::isRunning() const {
    return running.load();
}

void SchedulerMonitor::recordContextSwitch() {
    contextSwitchCount.fetch_add(1);
}

void SchedulerMonitor::recordTaskDispatch() {
    taskDispatchCount.fetch_add(1);
}

void SchedulerMonitor::recordWakeup() {
    wakeupCount.fetch_add(1);
}

void SchedulerMonitor::recordLatency(double latency) {
    std::lock_guard<std::mutex> lock(latencyMutex);
    totalLatency += latency;
    latencySamples.fetch_add(1);
}

void SchedulerMonitor::updateQueueSize([[maybe_unused]] int size) {
}

void SchedulerMonitor::updateTaskStates([[maybe_unused]] int total,
                                        [[maybe_unused]] int running,
                                        [[maybe_unused]] int sleeping,
                                        [[maybe_unused]] int finished) {
}

void SchedulerMonitor::updateVRuntime(const std::vector<long long>& vruntimeValues) {
    VRuntimeMetrics metrics;
    metrics.vruntimeValues = vruntimeValues;
    metrics.timestamp = std::chrono::system_clock::now();
    
    if (!vruntimeValues.empty()) {
        metrics.minVRuntime = *std::min_element(vruntimeValues.begin(), vruntimeValues.end());
        metrics.avgVRuntime = std::accumulate(vruntimeValues.begin(), vruntimeValues.end(), 0LL) / vruntimeValues.size();
    }
    
    vruntimeBuffer.push(metrics);
}

SchedulerMetrics SchedulerMonitor::collectSchedulerMetrics() {
    SchedulerMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.contextSwitches = contextSwitchCount.load();
    metrics.taskDispatchRate = taskDispatchCount.load();
    metrics.wakeups = wakeupCount.load();
    
    std::lock_guard<std::mutex> lock(latencyMutex);
    if (latencySamples.load() > 0) {
        metrics.schedulingLatency = totalLatency / latencySamples.load();
    }
    
    return metrics;
}

VRuntimeMetrics SchedulerMonitor::collectVRuntimeMetrics() {
    VRuntimeMetrics metrics;
    vruntimeBuffer.peek(metrics);
    return metrics;
}

QueueMetrics SchedulerMonitor::collectQueueMetrics() {
    QueueMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.currentLength = 0; // Will be updated externally
    return metrics;
}

SystemMetrics SchedulerMonitor::collectSystemMetrics() {
    SystemMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.contextSwitchCount = contextSwitchCount.load();
    
    std::lock_guard<std::mutex> lock(latencyMutex);
    metrics.averageLatency = latencySamples.load() > 0 ? totalLatency / latencySamples.load() : 0.0;
    return metrics;
}

void SchedulerMonitor::monitorLoop() {
    while (running.load()) {
        SchedulerMetrics schedMetrics = collectSchedulerMetrics();
        metricsBuffer.push(schedMetrics);
        
        QueueMetrics queueMetrics = collectQueueMetrics();
        queueBuffer.push(queueMetrics);
        
        SystemMetrics sysMetrics = collectSystemMetrics();
        systemBuffer.push(sysMetrics);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

SchedulerMetrics SchedulerMonitor::getLatestSchedulerMetrics() {
    SchedulerMetrics metrics;
    metricsBuffer.peek(metrics);
    return metrics;
}

VRuntimeMetrics SchedulerMonitor::getLatestVRuntimeMetrics() {
    VRuntimeMetrics metrics;
    vruntimeBuffer.peek(metrics);
    return metrics;
}

QueueMetrics SchedulerMonitor::getLatestQueueMetrics() {
    QueueMetrics metrics;
    queueBuffer.peek(metrics);
    return metrics;
}

SystemMetrics SchedulerMonitor::getLatestSystemMetrics() {
    SystemMetrics metrics;
    systemBuffer.peek(metrics);
    return metrics;
}

std::vector<SchedulerMetrics> SchedulerMonitor::getSchedulerMetricsHistory() {
    return metricsBuffer.snapshot();
}

std::vector<VRuntimeMetrics> SchedulerMonitor::getVRuntimeMetricsHistory() {
    return vruntimeBuffer.snapshot();
}

std::vector<QueueMetrics> SchedulerMonitor::getQueueMetricsHistory() {
    return queueBuffer.snapshot();
}

std::vector<SystemMetrics> SchedulerMonitor::getSystemMetricsHistory() {
    return systemBuffer.snapshot();
}

void SchedulerMonitor::addTimelineEvent(const TimelineEvent& event) {
    timelineEvents.push_back(event);
}

std::vector<TimelineEvent> SchedulerMonitor::getTimelineEvents() {
    return timelineEvents;
}

void SchedulerMonitor::clearTimelineEvents() {
    timelineEvents.clear();
}
