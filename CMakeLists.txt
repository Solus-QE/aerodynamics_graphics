cmake_minimum_required(VERSION 3.15)
project(aerodynamics)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MAIN_VARIANT "unified" CACHE STRING "Main source variant: unified, v1_exact, v2_exact")
set_property(CACHE MAIN_VARIANT PROPERTY STRINGS unified v1_exact v2_exact)

if(NOT MAIN_VARIANT STREQUAL "unified" AND NOT MAIN_VARIANT STREQUAL "v1_exact" AND NOT MAIN_VARIANT STREQUAL "v2_exact")
    message(FATAL_ERROR "MAIN_VARIANT must be one of: unified, v1_exact, v2_exact")
endif()

set(SIM_VERSION "2" CACHE STRING "Simulation render path version for unified main.cpp: 1 or 2")
set_property(CACHE SIM_VERSION PROPERTY STRINGS 1 2)

if(NOT SIM_VERSION STREQUAL "1" AND NOT SIM_VERSION STREQUAL "2")
    message(FATAL_ERROR "SIM_VERSION must be 1 or 2")
endif()

# Use vcpkg toolchain if available
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG QUIET)
if(NOT glfw3_FOUND)
    find_package(glfw3 REQUIRED)
endif()

if(TARGET glfw)
    set(GLFW_TARGET glfw)
elseif(TARGET glfw3::glfw)
    set(GLFW_TARGET glfw3::glfw)
else()
    message(FATAL_ERROR "Found glfw3 package, but no supported target (glfw or glfw3::glfw) is available")
endif()

if(MAIN_VARIANT STREQUAL "unified")
    set(MAIN_SOURCE main.cpp)

    find_package(GLEW REQUIRED)
    if(TARGET GLEW::GLEW)
        set(LOADER_TARGET GLEW::GLEW)
    elseif(TARGET GLEW::glew)
        set(LOADER_TARGET GLEW::glew)
    else()
        message(FATAL_ERROR "Could not find a supported GLEW target")
    endif()
else()
    if(MAIN_VARIANT STREQUAL "v1_exact")
        set(MAIN_SOURCE main_v1_exact.cpp)
    else()
        set(MAIN_SOURCE main_v2_exact.cpp)
    endif()

    find_path(GLAD_INCLUDE_DIR glad/glad.h HINTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
    if(NOT GLAD_INCLUDE_DIR)
        message(FATAL_ERROR "Could not find glad/glad.h for exact variants. Add headers under include/glad/glad.h and include/KHR/khrplatform.h.")
    endif()

    set(LOADER_SOURCES glad.c)
    if(UNIX AND NOT APPLE)
        set(LOADER_TARGET dl)
    endif()
endif()

add_executable(${PROJECT_NAME}
    ${MAIN_SOURCE}
    FluidSim.cpp
    FluidSim.h
    ${LOADER_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    ${GLFW_TARGET}
    ${LOADER_TARGET}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLAD_INCLUDE_DIR}
)

if(MAIN_VARIANT STREQUAL "unified")
    target_compile_definitions(${PROJECT_NAME} PRIVATE SIM_VERSION=${SIM_VERSION})
endif()
