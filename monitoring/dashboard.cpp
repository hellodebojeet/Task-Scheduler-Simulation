#include "dashboard.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

Dashboard::Dashboard() 
    : window(nullptr), running(false), paused(false), 
      showTimeline(true), showHeatmap(false),
      windowWidth(1280), windowHeight(720) {
    
    // Initialize theme colors (dark theme)
    colorCPU = ImVec4(0.3f, 0.6f, 1.0f, 1.0f);    // Blue
    colorGPU = ImVec4(0.3f, 1.0f, 0.5f, 1.0f);    // Green
    colorScheduler = ImVec4(1.0f, 0.6f, 0.2f, 1.0f); // Orange
    colorMemory = ImVec4(0.7f, 0.3f, 1.0f, 1.0f);  // Purple
    
    cpuMonitor = std::make_unique<CpuMonitor>();
    schedulerMonitor = std::make_unique<SchedulerMonitor>();
}

Dashboard::~Dashboard() {
    cleanup();
}

bool Dashboard::initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(windowWidth, windowHeight, "CFS Scheduler Performance Dashboard", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    return true;
}

bool Dashboard::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup dark theme
    ImGui::StyleColorsDark();
    
    // Setup ImPlot style
    ImPlot::StyleColorsDark();
    
    // Setup platform/renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
        return false;
    }
    
    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        std::cerr << "Failed to initialize ImGui OpenGL3 backend" << std::endl;
        return false;
    }
    
    return true;
}

bool Dashboard::initialize() {
    if (!initGLFW()) return false;
    if (!initImGui()) return false;
    
    return true;
}

void Dashboard::cleanup() {
    if (window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

void Dashboard::run() {
    running.store(true);
    cpuMonitor->start();
    schedulerMonitor->start();
    
    while (running.load() && !glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        if (!paused.load()) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            // Main menu bar
            renderMainMenuBar();
            
            // Create main window layout
            ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight - 20), ImGuiCond_FirstUseEver);
            ImGui::Begin("CFS Scheduler Performance Dashboard", nullptr, 
                        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            
            // CPU Usage Graph
            if (ImGui::BeginChild("CPU Usage", ImVec2(0, 150), true)) {
                renderCPUUsageGraph();
            }
            ImGui::EndChild();
            
            // Scheduler Activity Graph
            if (ImGui::BeginChild("Scheduler Activity", ImVec2(0, 150), true)) {
                renderSchedulerActivityGraph();
            }
            ImGui::EndChild();
            
            // Split view for vruntime and queue
            ImGui::Columns(2, "MetricsColumns", false);
            ImGui::SetColumnWidth(0, windowWidth / 2 - 10);
            
            // Virtual Runtime Visualization
            if (ImGui::BeginChild("Virtual Runtime", ImVec2(0, 150), true)) {
                renderVRuntimeVisualization();
            }
            ImGui::EndChild();
            
            ImGui::NextColumn();
            
            // Queue Monitoring Graph
            if (ImGui::BeginChild("Queue Monitoring", ImVec2(0, 150), true)) {
                renderQueueMonitoringGraph();
            }
            ImGui::EndChild();
            
            ImGui::Columns(1);
            
            // Task Execution Timeline
            if (showTimeline.load()) {
                if (ImGui::BeginChild("Task Timeline", ImVec2(0, 150), true)) {
                    renderTaskExecutionTimeline();
                }
                ImGui::EndChild();
            }
            
            // Active Tasks Table
            if (ImGui::BeginChild("Active Tasks", ImVec2(0, 200), true)) {
                renderActiveTasksTable();
            }
            ImGui::EndChild();
            
            // System Statistics Panel
            if (ImGui::BeginChild("System Statistics", ImVec2(0, 100), true)) {
                renderSystemStatisticsPanel();
            }
            ImGui::EndChild();
            
            ImGui::End();
            
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            glfwSwapBuffers(window);
        }
    }
    
    cpuMonitor->stop();
    schedulerMonitor->stop();
}

void Dashboard::stop() {
    running.store(false);
}

void Dashboard::setSchedulerMonitor(std::unique_ptr<SchedulerMonitor> monitor) {
    schedulerMonitor = std::move(monitor);
}

bool Dashboard::isRunning() const {
    return running.load();
}

void Dashboard::pause() {
    paused.store(true);
}

void Dashboard::resume() {
    paused.store(false);
}

bool Dashboard::isPaused() const {
    return paused.load();
}

void Dashboard::renderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Controls")) {
            if (ImGui::MenuItem("Pause", nullptr, paused.load())) {
                if (paused.load()) {
                    resume();
                } else {
                    pause();
                }
            }
            if (ImGui::MenuItem("Reset Statistics")) {
                // Reset logic here
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                stop();
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Show Timeline", nullptr, &showTimeline);
            ImGui::MenuItem("Show Heatmap", nullptr, &showHeatmap);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Export")) {
            if (ImGui::MenuItem("Export CSV")) {
                exportMetricsCSV();
            }
            if (ImGui::MenuItem("Export JSON")) {
                exportMetricsJSON();
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void Dashboard::renderCPUUsageGraph() {
    ImGui::Text("CPU Usage");
    
    auto cpuHistory = cpuMonitor->getMetricsHistory();
    
    std::vector<double> totalUsage, userUsage, systemUsage, idleUsage;
    std::vector<double> timestamps;
    
    for (const auto& metrics : cpuHistory) {
        totalUsage.push_back(metrics.totalUsage);
        userUsage.push_back(metrics.userUsage);
        systemUsage.push_back(metrics.systemUsage);
        idleUsage.push_back(metrics.idleUsage);
        
        auto time = std::chrono::system_clock::to_time_t(metrics.timestamp);
        timestamps.push_back(static_cast<double>(time));
    }
    
    if (!totalUsage.empty()) {
        if (ImPlot::BeginPlot("CPU Usage History", ImVec2(-1, -1))) {
            ImPlot::PlotLine("Total", timestamps.data(), totalUsage.data(), totalUsage.size());
            ImPlot::PlotLine("User", timestamps.data(), userUsage.data(), userUsage.size());
            ImPlot::PlotLine("System", timestamps.data(), systemUsage.data(), systemUsage.size());
            ImPlot::PlotLine("Idle", timestamps.data(), idleUsage.data(), idleUsage.size());
            ImPlot::EndPlot();
        }
    }
}

void Dashboard::renderSchedulerActivityGraph() {
    ImGui::Text("Scheduler Activity");
    
    auto schedHistory = schedulerMonitor->getSchedulerMetricsHistory();
    
    std::vector<double> contextSwitches, dispatchRate, wakeups;
    std::vector<double> timestamps;
    
    for (const auto& metrics : schedHistory) {
        contextSwitches.push_back(static_cast<double>(metrics.contextSwitches));
        dispatchRate.push_back(static_cast<double>(metrics.taskDispatchRate));
        wakeups.push_back(static_cast<double>(metrics.wakeups));
        
        auto time = std::chrono::system_clock::to_time_t(metrics.timestamp);
        timestamps.push_back(static_cast<double>(time));
    }
    
    if (!contextSwitches.empty()) {
        if (ImPlot::BeginPlot("Scheduler Activity History", ImVec2(-1, -1))) {
            ImPlot::PlotLine("Context Switches", timestamps.data(), contextSwitches.data(), contextSwitches.size());
            ImPlot::PlotLine("Dispatch Rate", timestamps.data(), dispatchRate.data(), dispatchRate.size());
            ImPlot::PlotLine("Wakeups", timestamps.data(), wakeups.data(), wakeups.size());
            ImPlot::EndPlot();
        }
    }
}

void Dashboard::renderVRuntimeVisualization() {
    ImGui::Text("Virtual Runtime");
    
    auto vruntimeMetrics = schedulerMonitor->getLatestVRuntimeMetrics();
    
    if (!vruntimeMetrics.vruntimeValues.empty()) {
        std::vector<int> taskIds;
        std::vector<double> vruntimeValues;
        
        for (size_t i = 0; i < vruntimeMetrics.vruntimeValues.size(); ++i) {
            taskIds.push_back(static_cast<int>(i));
            vruntimeValues.push_back(static_cast<double>(vruntimeMetrics.vruntimeValues[i]));
        }
        
        if (ImPlot::BeginPlot("Virtual Runtime by Task", ImVec2(-1, -1))) {
            ImPlot::PlotBars("vruntime", taskIds.data(), vruntimeValues.data(), taskIds.size());
            ImPlot::EndPlot();
        }
        
        ImGui::Text("Min vruntime: %lld", vruntimeMetrics.minVRuntime);
        ImGui::Text("Avg vruntime: %lld", vruntimeMetrics.avgVRuntime);
    }
}

void Dashboard::renderQueueMonitoringGraph() {
    ImGui::Text("Run Queue Monitoring");
    
    auto queueHistory = schedulerMonitor->getQueueMetricsHistory();
    
    std::vector<double> queueLengths;
    std::vector<double> timestamps;
    
    for (const auto& metrics : queueHistory) {
        queueLengths.push_back(static_cast<double>(metrics.currentLength));
        
        auto time = std::chrono::system_clock::to_time_t(metrics.timestamp);
        timestamps.push_back(static_cast<double>(time));
    }
    
    if (!queueLengths.empty()) {
        if (ImPlot::BeginPlot("Queue Length History", ImVec2(-1, -1))) {
            ImPlot::PlotLine("Queue Length", timestamps.data(), queueLengths.data(), queueLengths.size());
            ImPlot::EndPlot();
        }
    }
}

void Dashboard::renderTaskExecutionTimeline() {
    ImGui::Text("Task Execution Timeline");
    
    auto events = schedulerMonitor->getTimelineEvents();
    
    if (!events.empty()) {
        ImGui::Text("Timeline Events: %zu", events.size());
        // Simplified timeline display
        for (const auto& event : events) {
            ImGui::Text("PID %d: %s (%lld - %lld) [%s]", 
                       event.pid, event.taskName.c_str(), 
                       event.startTime, event.endTime, event.state.c_str());
        }
    }
}

void Dashboard::renderActiveTasksTable() {
    ImGui::Text("Active Tasks");
    
    if (ImGui::BeginTable("TasksTable", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable)) {
        ImGui::TableSetupColumn("PID");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Priority");
        ImGui::TableSetupColumn("Weight");
        ImGui::TableSetupColumn("vruntime");
        ImGui::TableSetupColumn("State");
        ImGui::TableHeadersRow();
        
        // Sample data - would be populated from actual scheduler
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("1");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("Process 1");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("1");
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("1024");
        ImGui::TableSetColumnIndex(4);
        ImGui::Text("1000");
        ImGui::TableSetColumnIndex(5);
        ImGui::Text("running");
        
        ImGui::EndTable();
    }
}

void Dashboard::renderSystemStatisticsPanel() {
    ImGui::Text("System Statistics");
    
    auto sysMetrics = schedulerMonitor->getLatestSystemMetrics();
    
    ImGui::Columns(4, "StatsColumns", false);
    ImGui::Text("Total Tasks: %d", sysMetrics.totalTasks);
    ImGui::NextColumn();
    ImGui::Text("Running: %d", sysMetrics.runningTasks);
    ImGui::NextColumn();
    ImGui::Text("Sleeping: %d", sysMetrics.sleepingTasks);
    ImGui::NextColumn();
    ImGui::Text("Finished: %d", sysMetrics.finishedTasks);
    ImGui::Columns(1);
    
    ImGui::Separator();
    
    ImGui::Columns(4, "PerfColumns", false);
    ImGui::Text("Context Switches: %lld", sysMetrics.contextSwitchCount);
    ImGui::NextColumn();
    ImGui::Text("Avg Latency: %.2f", sysMetrics.averageLatency);
    ImGui::NextColumn();
    ImGui::Text("Tick Rate: %.2f", sysMetrics.schedulerTickRate);
    ImGui::NextColumn();
    ImGui::Text("Throughput: %.2f", sysMetrics.throughput);
    ImGui::Columns(1);
}

void Dashboard::renderHeatmapVisualization() {
    ImGui::Text("CPU Core Utilization Heatmap");
    ImGui::Text("Heatmap visualization not yet implemented");
}

void Dashboard::exportMetricsCSV() {
    std::ofstream file("metrics_export.csv");
    if (file.is_open()) {
        file << "Timestamp,TotalCPU,UserCPU,SystemCPU,IdleCPU,ContextSwitches,DispatchRate,Wakeups\n";
        
        auto cpuHistory = cpuMonitor->getMetricsHistory();
        auto schedHistory = schedulerMonitor->getSchedulerMetricsHistory();
        
        size_t minSize = std::min(cpuHistory.size(), schedHistory.size());
        for (size_t i = 0; i < minSize; ++i) {
            auto time = std::chrono::system_clock::to_time_t(cpuHistory[i].timestamp);
            file << time << ","
                << cpuHistory[i].totalUsage << ","
                << cpuHistory[i].userUsage << ","
                << cpuHistory[i].systemUsage << ","
                << cpuHistory[i].idleUsage << ","
                << schedHistory[i].contextSwitches << ","
                << schedHistory[i].taskDispatchRate << ","
                << schedHistory[i].wakeups << "\n";
        }
        
        file.close();
        std::cout << "Metrics exported to metrics_export.csv" << std::endl;
    }
}

void Dashboard::exportMetricsJSON() {
    std::ofstream file("metrics_export.json");
    if (file.is_open()) {
        file << "{\n";
        file << "  \"cpu_metrics\": [\n";
        
        auto cpuHistory = cpuMonitor->getMetricsHistory();
        for (size_t i = 0; i < cpuHistory.size(); ++i) {
            auto time = std::chrono::system_clock::to_time_t(cpuHistory[i].timestamp);
            file << "    {\"timestamp\": " << time 
                 << ", \"total\": " << cpuHistory[i].totalUsage
                 << ", \"user\": " << cpuHistory[i].userUsage
                 << ", \"system\": " << cpuHistory[i].systemUsage
                 << ", \"idle\": " << cpuHistory[i].idleUsage << "}";
            if (i < cpuHistory.size() - 1) file << ",";
            file << "\n";
        }
        
        file << "  ],\n";
        file << "  \"scheduler_metrics\": [\n";
        
        auto schedHistory = schedulerMonitor->getSchedulerMetricsHistory();
        for (size_t i = 0; i < schedHistory.size(); ++i) {
            auto time = std::chrono::system_clock::to_time_t(schedHistory[i].timestamp);
            file << "    {\"timestamp\": " << time 
                 << ", \"context_switches\": " << schedHistory[i].contextSwitches
                 << ", \"dispatch_rate\": " << schedHistory[i].taskDispatchRate
                 << ", \"wakeups\": " << schedHistory[i].wakeups << "}";
            if (i < schedHistory.size() - 1) file << ",";
            file << "\n";
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
        std::cout << "Metrics exported to metrics_export.json" << std::endl;
    }
}
