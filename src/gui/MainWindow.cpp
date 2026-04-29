#include "gui/MainWindow.h"
#include "imgui.h"

namespace gm {

void MainWindow::render(const MetricsStore::Frame& frame,
                         const MetricsStore& store,
                         std::string_view gpuBackendName,
                         std::atomic<int>& updateIntervalMs)
{
    // Full-screen dockspace window
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowBgAlpha(1.f);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("##root", nullptr, flags);

    // Title bar
    ImGui::Text("GPU/CPU Performance Monitor");
    if (ImGui::SmallButton("Settings")) m_showSettings = !m_showSettings;
    ImGui::Separator();

    // Two-column layout: GPU left, CPU right
    float panelW = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
    float panelH = ImGui::GetContentRegionAvail().y;

    ImGui::BeginChild("LeftCol", ImVec2(panelW, panelH), false);
    m_gpuPanel.render(frame.gpu, store, gpuBackendName);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightCol", ImVec2(panelW, panelH), false);
    m_cpuPanel.render(frame.cpu, frame.ram, store);
    ImGui::EndChild();

    ImGui::End();

    // Settings popup
    if (m_showSettings) {
        ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Always);
        if (ImGui::Begin("Settings", &m_showSettings)) {
            int interval = updateIntervalMs.load(std::memory_order_relaxed);
            if (ImGui::SliderInt("Update interval (ms)", &interval, 100, 5000))
                updateIntervalMs.store(interval, std::memory_order_relaxed);
        }
        ImGui::End();
    }
}

} // namespace gm
