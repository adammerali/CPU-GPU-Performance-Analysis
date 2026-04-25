#pragma once
#include "backend/GPUBackend.h"
#include "metrics/MetricsStore.h"
#include <string_view>

namespace gm {

class GpuPanel {
public:
    void render(const GpuSnapshot& latest,
                const MetricsStore& store,
                std::string_view backendName);
private:
    void renderGraph(const char* label,
                     const RingBuffer<>& rb,
                     float yMin, float yMax,
                     const char* unit,
                     float currentValue);
};

} // namespace gm
