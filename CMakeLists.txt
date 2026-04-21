cmake_minimum_required(VERSION 3.20)
project(GpuMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Options ────────────────────────────────────────────────────────────────
option(GM_ENABLE_NVIDIA "Build NVIDIA NVML backend"  ON)
option(GM_ENABLE_AMD    "Build AMD ADLX backend"     ON)
option(GM_ENABLE_INTEL  "Build Intel WMI/PDH backend" ON)

# ── Compiler options ───────────────────────────────────────────────────────
include(cmake/CompilerOptions.cmake)

# ── External: ImGui ────────────────────────────────────────────────────────
add_library(imgui STATIC
    extern/imgui/imgui.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/backends/imgui_impl_dx11.cpp
    extern/imgui/backends/imgui_impl_win32.cpp
)
target_include_directories(imgui PUBLIC
    extern/imgui
    extern/imgui/backends
)
target_compile_definitions(imgui PUBLIC
    IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
)

# ── External: ImPlot ───────────────────────────────────────────────────────
add_library(implot STATIC
    extern/implot/implot.cpp
    extern/implot/implot_items.cpp
)
target_include_directories(implot PUBLIC extern/implot)
target_link_libraries(implot PUBLIC imgui)

# ── Optional: NVML ────────────────────────────────────────────────────────
set(NVML_FOUND FALSE)
if(GM_ENABLE_NVIDIA)
    include(cmake/FindNVML.cmake)
    if(NVML_FOUND)
        message(STATUS "NVML found — NVIDIA backend enabled")
    else()
        message(STATUS "NVML not found — NVIDIA backend disabled")
    endif()
endif()

# ── Optional: ADLX ────────────────────────────────────────────────────────
set(ADLX_FOUND FALSE)
if(GM_ENABLE_AMD)
    include(cmake/FindADLX.cmake)
    if(ADLX_FOUND)
        message(STATUS "ADLX found — AMD backend enabled")
    else()
        message(STATUS "ADLX SDK not found — AMD backend disabled")
    endif()
endif()

# ── Application sources ────────────────────────────────────────────────────
file(GLOB_RECURSE APP_SOURCES
    src/app/*.cpp
    src/backend/*.cpp
    src/gui/*.cpp
    src/metrics/*.cpp
)

add_executable(GpuMonitor WIN32 ${APP_SOURCES})

target_include_directories(GpuMonitor PRIVATE include)

target_link_libraries(GpuMonitor PRIVATE
    imgui
    implot
    d3d11
    dxgi
    dxguid
    pdh
    wbemuuid
    ole32
    oleaut32
)

# ── Conditional vendor linkage ─────────────────────────────────────────────
if(NVML_FOUND)
    target_compile_definitions(GpuMonitor PRIVATE GM_HAS_NVML)
    target_include_directories(GpuMonitor PRIVATE "${NVML_INCLUDE_DIR}")
    target_link_libraries(GpuMonitor PRIVATE "${NVML_LIBRARY}")
endif()

if(ADLX_FOUND)
    target_compile_definitions(GpuMonitor PRIVATE GM_HAS_ADLX)
    target_include_directories(GpuMonitor PRIVATE "${ADLX_INCLUDE_DIR}")
    target_link_libraries(GpuMonitor PRIVATE "${ADLX_LIBRARY}")
endif()

if(GM_ENABLE_INTEL)
    target_compile_definitions(GpuMonitor PRIVATE GM_HAS_INTEL)
endif()
