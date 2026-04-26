#ifdef GM_HAS_ADLX
#include "backend/AmdBackend.h"

// Pull in ADLX SDK headers
#include "ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "Include/IPerformanceMonitoring.h"
#include "Include/IGPUAutoTuning.h"

static ADLXHelper g_adlxHelper;

namespace gm {

bool AmdBackend::initialize()
{
    ADLX_RESULT res = g_adlxHelper.Initialize();
    if (ADLX_FAILED(res))
        return false;

    adlx::IADLXSystem* sys = g_adlxHelper.GetSystemServices();
    if (!sys)
        return false;

    adlx::IADLXGPUList* gpuList = nullptr;
    if (ADLX_FAILED(sys->GetGPUs(&gpuList)) || !gpuList)
        return false;

    adlx::IADLXGPU* gpu = nullptr;
    if (ADLX_FAILED(gpuList->At(gpuList->Begin(), &gpu)) || !gpu) {
        gpuList->Release();
        return false;
    }
    gpuList->Release();

    adlx::IADLXPerformanceMonitoringServices* perfSvc = nullptr;
    if (ADLX_FAILED(sys->GetPerformanceMonitoringServices(&perfSvc)) || !perfSvc) {
        gpu->Release();
        return false;
    }

    m_system  = sys;
    m_gpu     = gpu;
    m_perfSvc = perfSvc;
    return true;
}

GpuSnapshot AmdBackend::poll()
{
    if (!m_gpu || !m_perfSvc)
        return {};

    GpuSnapshot s;
    s.valid = true;

    // GPU name
    const char* name = nullptr;
    if (ADLX_SUCCEEDED(m_gpu->Name(&name)) && name)
        s.gpuName = name;

    // Collect current GPU metrics
    adlx::IADLXGPUMetrics* metrics = nullptr;
    if (ADLX_SUCCEEDED(m_perfSvc->GetCurrentGPUMetrics(m_gpu, &metrics)) && metrics) {
        adlx_double val = 0;

        if (ADLX_SUCCEEDED(metrics->GPUUsage(&val)))
            s.utilizationPercent = static_cast<float>(val);

        if (ADLX_SUCCEEDED(metrics->GPUTemperature(&val)))
            s.temperatureCelsius = static_cast<float>(val);

        if (ADLX_SUCCEEDED(metrics->GPUClockSpeed(&val)))
            s.coreClockMHz = static_cast<float>(val);

        if (ADLX_SUCCEEDED(metrics->GPUVRAMClockSpeed(&val)))
            s.memClockMHz = static_cast<float>(val);

        adlx_int vramUsed = 0, vramTotal = 0;
        if (ADLX_SUCCEEDED(metrics->GPUVRAM(&vramUsed)))
            s.vramUsedMB = static_cast<float>(vramUsed);

        // IADLXGPUMetrics1 exposes power
        adlx::IADLXGPUMetrics1* metrics1 = nullptr;
        if (ADLX_SUCCEEDED(metrics->QueryInterface(adlx::IADLXGPUMetrics1::IID(), (void**)&metrics1)) && metrics1) {
            if (ADLX_SUCCEEDED(metrics1->GPUTotalBoardPower(&val)))
                s.powerDrawWatts = static_cast<float>(val);
            metrics1->Release();
        }

        metrics->Release();
    }

    // VRAM total from GPU interface
    adlx_uint totalVRAM = 0;
    if (ADLX_SUCCEEDED(m_gpu->TotalVRAM(&totalVRAM)))
        s.vramTotalMB = static_cast<float>(totalVRAM);

    return s;
}

void AmdBackend::shutdown()
{
    if (m_perfSvc) { m_perfSvc->Release(); m_perfSvc = nullptr; }
    if (m_gpu)     { m_gpu->Release();     m_gpu     = nullptr; }
    m_system = nullptr;
    g_adlxHelper.Terminate();
}

} // namespace gm
#endif // GM_HAS_ADLX
