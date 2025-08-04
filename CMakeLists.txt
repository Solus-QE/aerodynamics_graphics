cmake_minimum_required(VERSION 3.15)
project(aerodynamics)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use vcpkg toolchain if available
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Find OpenGL and GLFW
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Add glad
add_library(glad glad.c)
target_include_directories(glad PUBLIC ../Libraries/include)

# Add main executable
add_executable(${PROJECT_NAME}
    main.cpp
    FluidSim.cpp
    FluidSim.h
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../Libraries/include
)