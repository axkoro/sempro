cmake_minimum_required(VERSION 3.17)
project(sempro CXX)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(extlibs/gtest)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(Example src/main.cpp src/Example.cpp)
target_include_directories(Example PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(ExampleTest src/main_testing.cpp src/Example.cpp)
target_link_libraries(ExampleTest PUBLIC gtest)