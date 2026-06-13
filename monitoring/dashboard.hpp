#ifndef DASHBOARD_HPP
#define DASHBOARD_HPP

#include "cpu_monitor.hpp"
#include "scheduler_monitor.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <atomic>
#include <vector>

class Dashboard {
private:
    GLFWwindow* window;
    std::unique_ptr<CpuMonitor> cpuMonitor;
    std::unique_ptr<SchedulerMonitor> schedulerMonitor;
    
    std::atomic<bool> running;
    std::atomic<bool> paused;
    std::atomic<bool> showTimeline;
    std::atomic<bool> showHeatmap;
    
    // Theme colors
    ImVec4 colorCPU;
    ImVec4 colorGPU;
    ImVec4 colorScheduler;
    ImVec4 colorMemory;
    
    // Window dimensions
    int windowWidth;
    int windowHeight;
    
    bool initGLFW();
    bool initImGui();
    void cleanup();
    
    void renderMainMenuBar();
    void renderCPUUsageGraph();
    void renderSchedulerActivityGraph();
    void renderVRuntimeVisualization();
    void renderQueueMonitoringGraph();
    void renderTaskExecutionTimeline();
    void renderActiveTasksTable();
    void renderSystemStatisticsPanel();
    void renderHeatmapVisualization();
    
    void exportMetricsCSV();
    void exportMetricsJSON();
    
public:
    Dashboard();
    ~Dashboard();
    
    bool initialize();
    void run();
    void stop();
    
    void setSchedulerMonitor(std::unique_ptr<SchedulerMonitor> monitor);
    
    bool isRunning() const;
    void pause();
    void resume();
    bool isPaused() const;
};

#endif
