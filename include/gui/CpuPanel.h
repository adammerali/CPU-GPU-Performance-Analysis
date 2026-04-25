#pragma once
#include "metrics/MetricsStore.h"

namespace gm {

class CpuPanel {
public:
    void render(const CpuSnapshot& cpu,
                const RamSnapshot& ram,
                const MetricsStore& store);
};

} // namespace gm
