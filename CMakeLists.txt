cmake_minimum_required(VERSION 3.17)
project(semproject CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build tests" ON)

if (BUILD_TESTS) # Instructions for building C++ library and googletest testing suite
	add_subdirectory(extlibs/gtest)
	set(CMAKE_BUILD_TYPE "Debug")

	add_executable(ExampleTest src/tests/ExampleTest.cpp src/Example.cpp)
	target_include_directories(ExampleTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(ExampleTest PRIVATE gtest)
	target_compile_options(ExampleTest PRIVATE -coverage)
	target_link_options(ExampleTest PRIVATE -coverage)
	
	add_executable(GraphTest src/tests/GraphTest.cpp src/Graph.cpp)
	target_include_directories(GraphTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(GraphTest PRIVATE gtest)
	target_compile_options(GraphTest PRIVATE -coverage)
	target_link_options(GraphTest PRIVATE -coverage)
else() # Build Python bindings as libraries
	add_subdirectory(extlibs/pybind11)
	pybind11_add_module(_example src/Example.cpp src/py11_example.cpp)
	target_include_directories(_example PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(_example PROPERTIES LINKER_LANGUAGE CXX)
	pybind11_add_module(_graph src/Graph.cpp src/py11_graph.cpp)
	target_include_directories(_graph PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(_graph PROPERTIES LINKER_LANGUAGE CXX)
endif()
 
set(CMAKE_CLEAN_TARGET "clean")