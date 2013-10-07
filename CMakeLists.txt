cmake_minimum_required(VERSION 2.6)

project(GpuRayTracer)

set(srcs
src/GPUManager.cpp
src/Image.cpp
src/Scene.cpp
)

include_directories("inc")

add_library(GRT STATIC ${srcs})

add_executable(main src/main.cpp)
target_link_libraries(main GRT)

set(test_srcs
test/test_suite.cpp
test/SceneTest.cpp
)

add_executable(TestGRT ${test_srcs})
target_link_libraries(TestGRT GRT gtest)

