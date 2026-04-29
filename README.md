# SysMonitor

Built to monitor GPU/CPU performance on my Lenovo Yoga 7 on games so I can return it if it sucks 😂. Should work on all systems.

Shows real-time graphs for GPU utilization, VRAM, temperature, power, clocks, CPU usage, and RAM.

Works with Nvidia/AMD GPU's and with any iGPU's (tested on igpu and nvidia gpu). Also Has extra stats like CPU interrupts and GPU busy which you can't see in MSI Afterburner or Steam overlay. All graphs in one tool + csv logging.

## Build

Requires Visual Studio 2022, CMake 3.20+, Git.

```bash
git clone --recurse-submodules https://github.com/adammerali/SysMonitor.git
cd SysMonitor
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

### GPU SDK setup (optional)

- **NVIDIA:** Install the [CUDA Toolkit](https://developer.nvidia.com/cuda-downloads). `nvml.dll` is already on your system with the driver.
- **AMD:** Download the [ADLX SDK](https://github.com/GPUOpen-LibrariesAndSDKs/ADLX/releases), put `SDK/Include/*` in `third_party/adlx/include/` and `SDK/Lib/x64/amd_adlx.lib` in `third_party/adlx/lib/`.

Builds fine without any SDKs it'll just show "No GPU SDK available".
