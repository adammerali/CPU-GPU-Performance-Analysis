# CPU-GPU Performance Analysis Tool

Built to monitor GPU/CPU performance on my Lenovo Yoga 7 while gaming. Shows real-time graphs for GPU utilization, VRAM, temperature, power, clocks, CPU usage, and RAM.

Works with NVIDIA, AMD, and Intel integrated GPUs.

## Build

Requires Visual Studio 2022, CMake 3.20+, Git.

```bash
git clone --recurse-submodules https://github.com/adammerali/CPU-GPU-Performance-Analysis.git
cd CPU-GPU-Performance-Analysis
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

### GPU SDK setup (optional)

- **NVIDIA:** Install the [CUDA Toolkit](https://developer.nvidia.com/cuda-downloads). `nvml.dll` is already on your system with the driver.
- **AMD:** Download the [ADLX SDK](https://github.com/GPUOpen-LibrariesAndSDKs/ADLX/releases), put `SDK/Include/*` in `third_party/adlx/include/` and `SDK/Lib/x64/amd_adlx.lib` in `third_party/adlx/lib/`.

Builds fine without any SDKs — just shows "No GPU SDK available".