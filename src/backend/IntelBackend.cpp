#ifdef GM_HAS_INTEL
#include "backend/IntelBackend.h"
#include <dxgi.h>
#include <string>
#include <algorithm>

// PDH GPU engine utilization counter path template.
// Works on Windows 10 1903+ regardless of GPU vendor, but Intel iGPUs
// typically only appear if NVML/ADLX are absent (factory tries those first).
static constexpr wchar_t k_counterPath[] =
    L"\\GPU Engine(*)\\Utilization Percentage";

namespace gm {

bool IntelBackend::initialize()
{
    // Find the Intel adapter via DXGI to confirm iGPU presence.
    IDXGIFactory* factory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
        return false;

    IDXGIAdapter* adapter = nullptr;
    bool found = false;
    for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC desc{};
        adapter->GetDesc(&desc);
        std::wstring name(desc.Description);
        // Intel iGPUs report "Intel" in their description.
        if (name.find(L"Intel") != std::wstring::npos) {
            m_adapterName = name;
            found = true;
        }
        adapter->Release();
        if (found) break;
    }
    factory->Release();

    if (!found)
        return false;

    // Set up PDH query for GPU engine utilization.
    if (PdhOpenQuery(nullptr, 0, &m_query) != ERROR_SUCCESS)
        return false;

    if (PdhAddEnglishCounterW(m_query, k_counterPath, 0, &m_utilCounter) != ERROR_SUCCESS) {
        PdhCloseQuery(m_query);
        m_query = nullptr;
        return false;
    }

    // Prime the first sample (PDH needs two collections to compute a value).
    PdhCollectQueryData(m_query);

    m_initialized = true;
    return true;
}

GpuSnapshot IntelBackend::poll()
{
    if (!m_initialized)
        return {};

    GpuSnapshot s;

    // Convert adapter name for display
    std::string name(m_adapterName.begin(), m_adapterName.end());
    s.gpuName = name;

    PdhCollectQueryData(m_query);

    PDH_FMT_COUNTERVALUE val{};
    DWORD type = 0;
    if (PdhGetFormattedCounterValue(m_utilCounter, PDH_FMT_DOUBLE, &type, &val) == ERROR_SUCCESS) {
        s.utilizationPercent = static_cast<float>(val.doubleValue);
        // Clamp: wildcard counter sums across engines and can exceed 100 on multi-engine GPUs.
        s.utilizationPercent = std::clamp(s.utilizationPercent, 0.f, 100.f);
    }

    // PDH does not expose VRAM, temp, power, or clocks for iGPUs.
    // Leave those at 0 and mark the snapshot valid for utilization display.
    s.valid = true;
    return s;
}

void IntelBackend::shutdown()
{
    if (m_query) {
        PdhCloseQuery(m_query);
        m_query = nullptr;
    }
    m_initialized = false;
}

} // namespace gm
#endif // GM_HAS_INTEL
