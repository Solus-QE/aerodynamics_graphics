@echo off
echo Setting up development environment...

REM Check if vcpkg exists in the parent directory, if not clone it
if not exist ..\vcpkg (
    echo Cloning vcpkg...
    cd ..
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    call bootstrap-vcpkg.bat
    cd ..\aerodynamics
)

REM Install required packages using vcpkg
echo Installing dependencies...
..\vcpkg\vcpkg install glfw3:x64-windows

REM Create build directory
if not exist build mkdir build

REM Generate project files using CMake
echo Generating project files...
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
cd ..

echo Setup complete! You can now build the project by:
echo 1. cd build
echo 2. cmake --build . --config Release
pause