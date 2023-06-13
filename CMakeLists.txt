cmake_minimum_required(VERSION 3.12)
project(GameSolver)

set(CMAKE_CXX_STANDARD 11)

# Add the executable
add_executable(GameSolver solver.cpp)

# Include header files
target_include_directories(GameSolver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})