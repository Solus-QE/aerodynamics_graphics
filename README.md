# Aerodynamics Simulation

A real-time fluid dynamics simulation that demonstrates aerodynamic principles using OpenGL.

## Project Overview

### What Was Implemented

This project implements a **real-time 2D fluid dynamics (aerodynamics) simulation** rendered with OpenGL. The core simulation is based on Jos Stam's "Stable Fluids" algorithm, which solves the incompressible Navier-Stokes equations on a discrete grid. Key components include:

- **FluidSim (C++ class):** A grid-based fluid solver (`FluidSim.h` / `FluidSim.cpp`) that operates on a 128×128 cell grid and supports:
  - **Diffusion** — spreads velocity and density using a Gauss-Seidel iterative solver.
  - **Advection** — moves density and velocity along the current velocity field using semi-Lagrangian back-tracing.
  - **Projection** — enforces incompressibility (divergence-free velocity) via a pressure-correction step, also solved with Gauss-Seidel iteration.
  - **Obstacle support** — individual grid cells can be marked as solid, enabling wind-tunnel walls and internal obstacles.

- **Wind tunnel setup (`main.cpp`):**
  - Top and bottom walls are set as solid boundary cells, forming a closed channel.
  - A rectangular obstacle is placed in the center of the tunnel to represent an object in the flow.
  - Fluid (density + rightward velocity) is continuously injected from the left side of the grid, simulating an oncoming airstream.

- **OpenGL rendering:**
  - Fluid density is visualized as a grid of red points whose brightness scales with local density, giving a clear picture of the flow field.
  - The solid obstacle is drawn as an orange-filled rectangle using a separate shader program.
  - The simulation runs in real time inside a standard GLFW render loop, updating the GPU vertex buffer every frame.

### Results

Running the simulation produces the following observable aerodynamic effects:

- **Laminar upstream flow** — fluid enters from the left and travels smoothly toward the obstacle.
- **Flow deflection and separation** — as the fluid encounters the rectangular obstacle, it is forced to go around it, clearly showing flow splitting above and below the body.
- **Wake formation** — a low-density shadow region (wake) forms immediately downstream of the obstacle, reflecting the reduced flow behind a bluff body.
- **Wall effects** — the top and bottom solid walls accelerate the flow in the narrow passages between the obstacle and the walls, consistent with the Venturi effect.

The combination of the diffusion, advection, and projection steps keeps the simulation numerically stable at the chosen parameters (time step 0.00001, viscosity 0.0000001, diffusion rate 0.2), allowing the flow pattern to evolve smoothly without blowing up over time.

## Start Guide

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
