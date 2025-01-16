cmake_minimum_required(VERSION 3.17)
project(semproject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

option(BUILD_TESTS "Build tests" ON)

find_package(OpenMP REQUIRED)

if (BUILD_TESTS) # Instructions for building C++ library and googletest testing suite
	add_subdirectory(extlibs/gtest)
	set(CMAKE_BUILD_TYPE "Debug")
	
	add_executable(GraphTest src/tests/GraphTest.cpp src/Graph.cpp)
	target_include_directories(GraphTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(GraphTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(GraphTest PRIVATE -coverage)
	target_link_options(GraphTest PRIVATE -coverage)

	add_executable(KNNImputerTest src/tests/KNNImputerTest.cpp src/KNNImputer.cpp src/Graph.cpp)
	target_include_directories(KNNImputerTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(KNNImputerTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(KNNImputerTest PRIVATE -coverage)
	target_link_options(KNNImputerTest PRIVATE -coverage)

	add_executable(LouvainClusteringTest src/tests/LouvainClusteringTest.cpp src/LouvainClustering.cpp src/Graph.cpp)
	target_include_directories(LouvainClusteringTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(LouvainClusteringTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(LouvainClusteringTest PRIVATE -coverage)
	target_link_options(LouvainClusteringTest PRIVATE -coverage)

	add_executable(LouvainImputerTest src/tests/LouvainImputerTest.cpp src/LouvainImputer.cpp src/Graph.cpp)
	target_include_directories(LouvainImputerTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(LouvainImputerTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(LouvainImputerTest PRIVATE -coverage)
	target_link_options(LouvainImputerTest PRIVATE -coverage)
else() # Build Python bindings as libraries
	add_subdirectory(extlibs/pybind11)

	pybind11_add_module(_graph src/Graph.cpp src/py11_graph.cpp)
	target_link_libraries(_graph PRIVATE OpenMP::OpenMP_CXX)
	target_include_directories(_graph PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(_graph PROPERTIES LINKER_LANGUAGE CXX)

	add_library(graph_lib src/Graph.cpp)
	target_include_directories(graph_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
	
	pybind11_add_module(_strats src/KNNImputer.cpp src/py11_strats.cpp)
	target_link_libraries(_strats PRIVATE graph_lib OpenMP::OpenMP_CXX)
	target_include_directories(_strats PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(_strats PROPERTIES LINKER_LANGUAGE CXX)
endif()
 
set(CMAKE_CLEAN_TARGET "clean")