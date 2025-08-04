# Aerodynamics Simulation

A real-time fluid dynamics simulation that demonstrates aerodynamic principles using OpenGL.

## Quick Start Guide (Cross-Platform, No Visual Studio Required)

### Prerequisites

- C++14 compatible compiler (GCC, Clang, MSVC, or MinGW)
- [CMake](https://cmake.org/) 3.15 or newer
- [GLFW](https://www.glfw.org/) and [GLAD](https://glad.dav1d.de/) (see below for setup)

### Building the Project

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
     ./vcpkg install glfw3
     export VCPKG_ROOT=$(pwd) # or set VCPKG_ROOT=%cd% on Windows
     cd ..
     ```
   - **Or install GLFW and OpenGL using your system package manager.**

3. **Make sure the `glad` loader files are present:**
   - Place `glad.c` in your project root or `glad/` directory.
   - Place `glad.h` in `glad/include/glad/`.

4. **Build with CMake:**
   ```sh
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

5. **Run the simulation:**
   ```sh
   ./aerodynamics   # or aerodynamics.exe on Windows
   ```

## Project Structure

- `main.cpp` - Main application code
- `FluidSim.h/cpp` - Fluid simulation implementation
- `glad/` - OpenGL loader (C and header files)

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
- If you get `glad.h not found`:
  - Ensure the glad folder is in your project directory as described above
  - Check that the include paths are set correctly in CMakeLists.txt
- For any other issues:
  - Make sure your compiler supports C++14
  - Ensure all dependencies are installed and discoverable by CMake