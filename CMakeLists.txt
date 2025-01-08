cmake_minimum_required(VERSION 3.17)
project(semproject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

option(BUILD_TESTS "Build tests" ON)

find_package(OpenMP REQUIRED)

add_library(GraphLib
    src/Graph.cpp
    src/GraphDouble.cpp
    src/GraphInt.cpp
    src/GraphBool.cpp
)
target_include_directories(GraphLib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(GraphLib PRIVATE OpenMP::OpenMP_CXX)

if (BUILD_TESTS) # Instructions for building C++ library and googletest testing suite
	add_subdirectory(extlibs/gtest)
	set(CMAKE_BUILD_TYPE "Debug")
	
	add_executable(GraphTest src/tests/GraphTest.cpp)
	target_include_directories(GraphTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(GraphTest PRIVATE gtest GraphLib OpenMP::OpenMP_CXX)
	target_compile_options(GraphTest PRIVATE -coverage)
	target_link_options(GraphTest PRIVATE -coverage)

	# add_executable(KNNImputerTest src/tests/KNNImputerTest.cpp src/KNNImputer.cpp)
	# target_include_directories(KNNImputerTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	# target_link_libraries(KNNImputerTest PRIVATE gtest GraphLib OpenMP::OpenMP_CXX)
	# target_compile_options(KNNImputerTest PRIVATE -coverage)
	# target_link_options(KNNImputerTest PRIVATE -coverage)
else() # Build Python bindings as libraries
	add_subdirectory(extlibs/pybind11)

	pybind11_add_module(_graph src/py11_graph.cpp)
	target_link_libraries(_graph PRIVATE GraphLib OpenMP::OpenMP_CXX)
	target_include_directories(_graph PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(_graph PROPERTIES LINKER_LANGUAGE CXX)
	
	# pybind11_add_module(_strats src/KNNImputer.cpp src/py11_strats.cpp)
	# target_link_libraries(_strats PRIVATE GraphLib OpenMP::OpenMP_CXX)
	# target_include_directories(_strats PUBLIC ${PROJECT_SOURCE_DIR}/include)
	# set_target_properties(_strats PROPERTIES LINKER_LANGUAGE CXX)
endif()
 
set(CMAKE_CLEAN_TARGET "clean")