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
   git clone https://github.com/Solus-QE/aerodynamics_graphics.git
   cd aerodynamics_graphics
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

3. **Recommended: build and run (works in Linux/devcontainer, no manual GLAD header fetch):**

    Use unified mode first. It is the most reliable path and does not require downloading extra GLAD headers.

    ```sh
    set -euo pipefail

    cd /workspaces/aerodynamics_graphics

    sudo apt-get update
    sudo apt-get install -y \
       build-essential cmake pkg-config \
       libglfw3-dev libglew-dev libopengl-dev libgl1-mesa-dev \
       mesa-utils xvfb

    cmake -S . -B build -DMAIN_VARIANT=unified -DSIM_VERSION=2
    cmake --build build -j"$(nproc)"

    if [ -n "${DISPLAY:-}" ]; then
       LIBGL_ALWAYS_SOFTWARE=1 MESA_GL_VERSION_OVERRIDE=3.3 ./build/aerodynamics
    else
       xvfb-run -a -s "-screen 0 1280x720x24" \
          env LIBGL_ALWAYS_SOFTWARE=1 MESA_GL_VERSION_OVERRIDE=3.3 \
          ./build/aerodynamics
    fi
    ```

4. **Optional: browser viewing with noVNC (opens network ports):**

    The simulation binary itself does not open ports. If you want to view it in a browser, start an X/VNC/noVNC stack:

    ```sh
    set -euo pipefail

    sudo apt-get update
    sudo apt-get install -y xvfb x11vnc novnc websockify fluxbox

    # X display and minimal desktop on :1
    Xvfb :1 -screen 0 1280x720x24 &
    DISPLAY=:1 fluxbox &

    # VNC server (localhost only) on 5901
    x11vnc -display :1 -forever -shared -rfbport 5901 -localhost &

    # noVNC websocket bridge on 6080
    websockify --web=/usr/share/novnc 6080 localhost:5901 &

    # Run the app on display :1
    DISPLAY=:1 LIBGL_ALWAYS_SOFTWARE=1 MESA_GL_VERSION_OVERRIDE=3.3 ./build/aerodynamics
    ```

    Verify listening ports:
    ```sh
    ss -ltnp | grep -E ':5901|:6080'
    ```

    Open in browser:
    - Local machine: `http://localhost:6080/vnc.html`
    - Codespaces/devcontainer: forward port 6080, then open the forwarded URL

5. **Exact source variants (advanced):**

    Exact Version 1 and Version 2 use `glad.c` and require local headers at `include/glad/glad.h` and `include/KHR/khrplatform.h`.

    Prepare headers:
    ```sh
    set -euo pipefail

    cd /workspaces/aerodynamics_graphics
    mkdir -p include/glad include/KHR

    # Try raw downloads first (may fail in restricted networks)
    curl -fsSL https://raw.githubusercontent.com/Dav1dde/glad/master/include/glad/glad.h \
       -o include/glad/glad.h || true
    curl -fsSL https://raw.githubusercontent.com/KhronosGroup/Khronos-Headers/main/include/KHR/khrplatform.h \
       -o include/KHR/khrplatform.h || true

    # Fallback if either file is missing
    if [ ! -s include/glad/glad.h ] || [ ! -s include/KHR/khrplatform.h ]; then
       rm -rf /tmp/glad-src /tmp/khronos-src
       git clone --depth 1 https://github.com/Dav1dde/glad.git /tmp/glad-src
       git clone --depth 1 https://github.com/KhronosGroup/Khronos-Headers.git /tmp/khronos-src
       cp /tmp/glad-src/include/glad/glad.h include/glad/glad.h
       cp /tmp/khronos-src/include/KHR/khrplatform.h include/KHR/khrplatform.h
    fi

    test -s include/glad/glad.h
    test -s include/KHR/khrplatform.h
    ```

    Build exact Version 1:
    ```sh
    cmake -S . -B build-v1 -DMAIN_VARIANT=v1_exact
    cmake --build build-v1 -j"$(nproc)"
    ./build-v1/aerodynamics
    ```

    Build exact Version 2:
    ```sh
    cmake -S . -B build-v2 -DMAIN_VARIANT=v2_exact
    cmake --build build-v2 -j"$(nproc)"
    ./build-v2/aerodynamics
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
- If `curl` to `raw.githubusercontent.com` returns `404` for known-good paths:
   - Your environment/network is likely blocking raw GitHub content
   - Use the git-clone fallback in the setup block instead of direct raw downloads
- If no browser window appears while running in a container:
  - The app itself does not publish web ports
  - Start the noVNC stack and forward port 6080
- For any other issues:
  - Make sure your compiler supports C++14
  - Ensure all dependencies are installed and discoverable by CMake
