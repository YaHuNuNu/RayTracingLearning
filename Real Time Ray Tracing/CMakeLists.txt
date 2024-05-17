cmake_minimum_required(VERSION 3.2)
project(RealTimeRayTracing)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}")
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#set(CMAKE_INSTALL_RPATH "${PROJECT_SOURCE_DIR}/LIB")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

file(GLOB LIB_FILES "${PROJECT_SOURCE_DIR}/LIB/*.lib")
aux_source_directory(${PROJECT_SOURCE_DIR}/Code SRC)

include_directories(${PROJECT_SOURCE_DIR}/Include)
link_directories(${PROJECT_SOURCE_DIR}/LIB)

add_executable(RealTimeRayTracing ${SRC})

add_custom_command(TARGET RealTimeRayTracing POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/Assets
                   $<TARGET_FILE_DIR:RealTimeRayTracing>/Assets)

add_custom_command(TARGET RealTimeRayTracing POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/DLL
                   $<TARGET_FILE_DIR:RealTimeRayTracing>)

target_link_libraries(RealTimeRayTracing ${LIB_FILES})
