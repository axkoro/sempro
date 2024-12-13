cmake_minimum_required(VERSION 3.17)
project(semproject CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build tests" ON)

if (BUILD_TESTS) # Instructions for building C++ library and googletest testing suite
	add_subdirectory(extlibs/gtest)
	set(CMAKE_BUILD_TYPE "Debug")
	
	add_executable(GraphTest src/tests/GraphTest.cpp src/Graph.cpp)
	target_include_directories(GraphTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(GraphTest PRIVATE gtest)
	target_compile_options(GraphTest PRIVATE -coverage)
	target_link_options(GraphTest PRIVATE -coverage)

	add_executable(KNNImputerTest src/tests/KNNImputerTest.cpp src/KNNImputer.cpp src/Graph.cpp)
	target_include_directories(KNNImputerTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(KNNImputerTest PRIVATE gtest)
	target_compile_options(KNNImputerTest PRIVATE -coverage)
	target_link_options(KNNImputerTest PRIVATE -coverage)
else() # Build Python bindings as libraries
	add_subdirectory(extlibs/pybind11)

	add_library(graph_lib src/Graph.cpp)
	target_include_directories(graph_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

	pybind11_add_module(_graph src/Graph.cpp src/py11_graph.cpp)
	target_include_directories(_graph PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(_graph PROPERTIES LINKER_LANGUAGE CXX)

	pybind11_add_module(_strats src/KNNImputer.cpp src/py11_strats.cpp)
	target_link_libraries(_strats PRIVATE graph_lib)
	target_include_directories(_strats PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(_strats PROPERTIES LINKER_LANGUAGE CXX)
endif()
 
set(CMAKE_CLEAN_TARGET "clean")