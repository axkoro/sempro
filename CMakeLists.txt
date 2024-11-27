cmake_minimum_required(VERSION 3.17)
project(semproject CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semproject)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semproject)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/semproject)

option(BUILD_TESTS "Build tests" OFF)

if (BUILD_TESTS) # Instructions for building C++ library and googletest testing suite
	add_subdirectory(extlibs/gtest)
	set(CMAKE_BUILD_TYPE "Debug")
	add_executable(VectorTest src/main_testing.cpp src/DenseVector.cpp)
	target_include_directories(VectorTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(VectorTest PRIVATE gtest)
	target_compile_options(VectorTest PRIVATE -coverage)
	target_link_options(VectorTest PRIVATE -coverage)
else() # Build Python bindings as libraries
	add_subdirectory(extlibs/pybind11)
	pybind11_add_module(_lin_alg src/py11_lin_alg.cpp src/DenseVector.cpp)
	target_include_directories(_lin_alg PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(_lin_alg PROPERTIES LINKER_LANGUAGE CXX)
endif()
 
set(CMAKE_CLEAN_TARGET "clean")