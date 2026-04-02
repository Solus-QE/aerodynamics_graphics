# Aerodynamics Simulation

A real-time fluid dynamics simulation that demonstrates aerodynamic principles using OpenGL.

## Start Guide

### Prerequisites

- C++14 compatible compiler (GCC, Clang, MSVC, or MinGW)
- [CMake](https://cmake.org/) 3.15 or newer
- [GLFW](https://www.glfw.org/) and [GLEW](https://glew.sourceforge.net/)

### Build and Run

1. **Clone this repository:**
   ```sh
   git clone https://github.com/Solus-QE/aerodynamics_graphics_.git
   cd aerodynamics_graphics_
   ```

2. **Install dependencies:**
   - **With vcpkg (recommended, cross-platform):**
     ```sh
     git clone https://github.com/Microsoft/vcpkg.git
     cd vcpkg
     ./bootstrap-vcpkg.sh # or .\bootstrap-vcpkg.bat on Windows
       ./vcpkg install glfw3 glew
     export VCPKG_ROOT=$(pwd) # or set VCPKG_ROOT=%cd% on Windows
     cd ..
     ```
   - **Or install GLFW/OpenGL/GLEW from your system package manager (Ubuntu/Debian):**
       ```sh
       sudo apt-get update
       sudo apt-get install -y libglfw3-dev libglew-dev libopengl-dev libgl1-mesa-dev mesa-common-dev
       ```

3. **Choose one run mode:**

    - **Unified mode (`main.cpp`)**
       Uses the refactored entry point and lets you pick simulation behavior via `SIM_VERSION=1|2`.

       ```sh
       cmake -S . -B build -DMAIN_VARIANT=unified -DSIM_VERSION=2
       cmake --build build -j
       ./build/aerodynamics
       ```

    - **Exact Version 1 (`main_v1_exact.cpp`)**
    - **Exact Version 2 (`main_v2_exact.cpp`)**

       These two are the preserved source variants. They use GLAD + `glad.c`.
       If you use exact variants, ensure headers exist:

       ```sh
       mkdir -p include/glad include/KHR
       curl -fsSL https://raw.githubusercontent.com/Dav1dde/glad/v0.1.36/include/glad/glad.h -o include/glad/glad.h
       curl -fsSL https://raw.githubusercontent.com/KhronosGroup/Khronos-Headers/main/include/KHR/khrplatform.h -o include/KHR/khrplatform.h
       ```

       Version 1:
       ```sh
       cmake -S . -B build-v1 -DMAIN_VARIANT=v1_exact
       cmake --build build-v1 -j
       ./build-v1/aerodynamics
       ```

       Version 2:
       ```sh
       cmake -S . -B build-v2 -DMAIN_VARIANT=v2_exact
       cmake --build build-v2 -j
       ./build-v2/aerodynamics
       ```

4. **Headless/container run (if no desktop display):**
   ```sh
    DISPLAY=:1 LIBGL_ALWAYS_SOFTWARE=1 MESA_GL_VERSION_OVERRIDE=3.3 ./build/aerodynamics
   ```

## Project Structure

- `main.cpp` - Unified entry point (select behavior with `SIM_VERSION=1|2`)
- `main_v1_exact.cpp` - Preserved exact Version 1 source path
- `main_v2_exact.cpp` - Preserved exact Version 2 source path
- `FluidSim.h/cpp` - Fluid simulation implementation
- Unified mode uses GLEW; exact modes use GLAD (`glad.c` + headers)

## Controls

The simulation starts automatically and shows:
- Red particles representing fluid flow
- Orange rectangle representing the obstacle
- Fluid flows from left to right
- The simulation demonstrates how fluid (air) flows around the obstacle

## Parameters

You can modify these parameters in `main.cpp`:
- `simSize`: Grid size (default: 128)
- `obsSize`: Obstacle size (default: simSize/8)
- Point size: Modify `gl_PointSize` in shaders (default: 12.0)
- Fluid parameters in FluidSim constructor:
  - Time step: 0.00001f
  - Viscosity: 0.0000001f
  - Diffusion rate: 0.2f

## Troubleshooting

- If you get `GLFW/glfw3.h not found`:
  - Make sure you've installed GLFW (via vcpkg or your system package manager)
  - Ensure CMake can find the GLFW package
- If you get `GL/glew.h not found`:
   - Install `libglew-dev` (Linux) or `glew` via vcpkg
- If you build exact variants and get `glad/glad.h not found`:
   - Add `include/glad/glad.h` and `include/KHR/khrplatform.h` as shown above
- For any other issues:
  - Make sure your compiler supports C++14
  - Ensure all dependencies are installed and discoverable by CMake
