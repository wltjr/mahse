################################################################################
# Set minimum required version of cmake, project name and compile options
################################################################################
cmake_minimum_required(VERSION 3.20)
project(cot6405)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-g -Wall -Wextra -Wpedantic)
endif()


################################################################################
# Find packages and libraries dependencies
################################################################################
#find_package(MPI REQUIRED)


################################################################################
# Build
################################################################################
include_directories(src)

# executable binaries
add_executable(grape src/main.cpp src/agent.cpp src/task.cpp)

# link executable binaries to shared object libraries
#target_link_libraries(some.exe PUBLIC MPI::MPI_CXX)
