cmake_minimum_required(VERSION 3.0)
project(RayTracingByCPU)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}")
set(CMAKE_CXX_STANDARD 14)

aux_source_directory(${PROJECT_SOURCE_DIR}/SRC SRC)

include_directories(${PROJECT_SOURCE_DIR}/Include)

add_executable(RayTracingByCPU ${SRC})