cmake_minimum_required(VERSION 3.17)
project(sempro CXX)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(extlibs/gtest)

add_executable(VectorExample src/main.cpp src/DenseVector.cpp)
target_include_directories(VectorExample PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(VectorTest src/main_testing.cpp src/DenseVector.cpp)
target_link_libraries(VectorTest PUBLIC gtest)