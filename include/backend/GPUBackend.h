#pragma once
#include <memory>
#include <string>
#include <string_view>

namespace gm {

struct GpuSnapshot {
    float utilizationPercent = 0.f;   // 0–100
    float vramUsedMB         = 0.f;
    float vramTotalMB        = 0.f;
    float temperatureCelsius = 0.f;
    float powerDrawWatts     = 0.f;
    float coreClockMHz       = 0.f;
    float memClockMHz        = 0.f;
    std::string gpuName;
    bool valid = false;  // false = backend unavailable or SDK missing
};

class IGPUBackend {
public:
    virtual ~IGPUBackend() = default;

    // Called once after construction. Returns false if SDK init failed.
    virtual bool initialize() = 0;

    // Poll hardware and return a fresh snapshot. Must be fast (<10 ms).
    virtual GpuSnapshot poll() = 0;

    // Human-readable backend name for UI display ("NVML", "ADLX", etc.)
    virtual std::string_view backendName() const = 0;

    // Orderly shutdown (e.g. NVML requires nvmlShutdown()).
    virtual void shutdown() = 0;
};

// Factory: detects installed GPU and returns the appropriate backend.
// Returns NullBackend (never nullptr) if no vendor SDK is available.
std::unique_ptr<IGPUBackend> createGPUBackend();

} // namespace gm
