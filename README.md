# CPU-GPU Performance Analysis Tool

Real-time performance monitor built in C++ with an ImGui overlay. Made to track how my Lenovo Yoga 7 handles games — shows live scrolling graphs for GPU utilization, VRAM usage, temperature, power draw, clock speeds, CPU usage, and RAM.

Automatically detects your GPU vendor at startup and uses the right API — no config needed. Supports NVIDIA (via NVML), AMD (via ADLX), and Intel integrated GPUs (via PDH counters).

## Build

Requires Visual Studio 2022, CMake 3.20+, Git.

```bash
git clone --recurse-submodules https://github.com/adammerali/CPU-GPU-Performance-Analysis.git
cd CPU-GPU-Performance-Analysis
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Runs without any extra SDKs — GPU metrics just won't show until you set one up below.

## GPU SDK setup

- **NVIDIA:** Install the [CUDA Toolkit](https://developer.nvidia.com/cuda-downloads). `nvml.dll` ships with your driver so nothing extra is needed at runtime.
- **AMD:** Download the [ADLX SDK](https://github.com/GPUOpen-LibrariesAndSDKs/ADLX/releases), copy `SDK/Include/*` to `third_party/adlx/include/` and `SDK/Lib/x64/amd_adlx.lib` to `third_party/adlx/lib/`, then rebuild.
- **Intel iGPU:** No SDK needed — uses Windows PDH counters built into the OS.