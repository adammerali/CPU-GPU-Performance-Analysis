#include "gui/CpuPanel.h"
#include "imgui.h"
#include "implot.h"
#include <cstdio>

namespace gm {

static float RingBufferGetter(void* data, int idx)
{
    auto* rb = static_cast<const RingBuffer<>*>(data);
    int   sz  = static_cast<int>(rb->data.size());
    int   pos = (rb->offset - rb->count + idx + sz) % sz;
    return rb->data[static_cast<std::size_t>(pos)];
}

void CpuPanel::render(const CpuSnapshot& cpu,
                       const RamSnapshot& ram,
                       const MetricsStore& store)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
    ImGui::BeginChild("CPU", ImVec2(0, 0), true);

    ImGui::Text("CPU & Memory");
    ImGui::Separator();

    // CPU utilization graph
    ImGui::Text("CPU Usage");
    if (ImPlot::BeginPlot("CPU##plot", ImVec2(-1, 100), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr,
                          ImPlotAxisFlags_NoDecorations,
                          ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 100.0, ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0,
            static_cast<double>(store.cpuUtil.data.size()), ImPlotCond_Always);
        ImPlot::PlotLineG("CPU", RingBufferGetter,
            const_cast<RingBuffer<>*>(&store.cpuUtil), store.cpuUtil.count);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImGui::Text("%.1f %%", cpu.totalUsagePercent);

    ImGui::Spacing();

    // RAM progress bar
    float ramFrac = (ram.totalMB > 0.f) ? (ram.usedMB / ram.totalMB) : 0.f;
    char ramLabel[64];
    std::snprintf(ramLabel, sizeof(ramLabel),
                  "%.0f MB / %.0f MB", ram.usedMB, ram.totalMB);
    ImGui::Text("RAM");
    ImGui::ProgressBar(ramFrac, ImVec2(-1, 20), ramLabel);

    // RAM history graph
    if (ImPlot::BeginPlot("RAM##plot", ImVec2(-1, 100), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr,
                          ImPlotAxisFlags_NoDecorations,
                          ImPlotAxisFlags_NoDecorations);
        float ramMax = ram.totalMB > 0.f ? ram.totalMB : 32768.f;
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, static_cast<double>(ramMax), ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0,
            static_cast<double>(store.ramUsed.data.size()), ImPlotCond_Always);
        ImPlot::PlotLineG("RAM", RingBufferGetter,
            const_cast<RingBuffer<>*>(&store.ramUsed), store.ramUsed.count);
        ImPlot::EndPlot();
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
}

} // namespace gm
