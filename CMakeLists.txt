cmake_minimum_required(VERSION 3.17)
project(semproject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" ON)

find_package(OpenMP REQUIRED)

if (BUILD_TESTS) # Instructions for building C++ library and googletest testing suite
	add_subdirectory(extlibs/gtest)
	set(CMAKE_BUILD_TYPE Debug)
	
	add_executable(GraphTest
		src/tests/GraphTest.cpp
		src/Graph.cpp
		src/GraphEdgeWeights.cpp)
	target_include_directories(GraphTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(GraphTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(GraphTest PRIVATE -coverage)
	target_link_options(GraphTest PRIVATE -coverage)

	add_executable(KNNImputerTest
		src/tests/KNNImputerTest.cpp
		src/Graph.cpp)
	target_include_directories(KNNImputerTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(KNNImputerTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(KNNImputerTest PRIVATE -coverage)
	target_link_options(KNNImputerTest PRIVATE -coverage)

	add_executable(LouvainTest
		src/tests/LouvainTest.cpp
		src/Louvain.cpp
		src/Graph.cpp
		src/DynamicGraph.cpp)
	target_include_directories(LouvainTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(LouvainTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(LouvainTest PRIVATE -coverage)
	target_link_options(LouvainTest PRIVATE -coverage)

	add_executable(CommunityImputerTest
		src/tests/CommunityImputerTest.cpp
		src/Graph.cpp)
	target_include_directories(CommunityImputerTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(CommunityImputerTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(CommunityImputerTest PRIVATE -coverage)
	target_link_options(CommunityImputerTest PRIVATE -coverage)

	add_executable(DeepWalkTest
		src/tests/DeepWalkTest.cpp
		src/RandomWalkGenerator.cpp
		src/SkipGram.cpp
		src/NegativeSampler.cpp
		src/Matrix.cpp
		src/Vector.cpp
		src/Graph.cpp
		src/GraphEdgeWeights.cpp)
	target_include_directories(DeepWalkTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(DeepWalkTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(DeepWalkTest PRIVATE -coverage)
	target_link_options(DeepWalkTest PRIVATE -coverage)

	add_executable(LinearAlgebraTest
		src/tests/LinearAlgebraTest.cpp
		src/Vector.cpp
		src/Matrix.cpp)
	target_include_directories(LinearAlgebraTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(LinearAlgebraTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(LinearAlgebraTest PRIVATE -coverage)
	target_link_options(LinearAlgebraTest PRIVATE -coverage)

	add_executable(NegativeSamplerTest
		src/tests/NegativeSamplerTest.cpp
		src/NegativeSampler.cpp
		src/Graph.cpp)
	target_include_directories(NegativeSamplerTest PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(NegativeSamplerTest PRIVATE gtest OpenMP::OpenMP_CXX)
	target_compile_options(NegativeSamplerTest PRIVATE -coverage)
	target_link_options(NegativeSamplerTest PRIVATE -coverage)

else() # Build Python bindings as libraries
	set(CMAKE_BUILD_TYPE Release)
	add_subdirectory(extlibs/pybind11)

	pybind11_add_module(cpp_graph_module
		src/pybind/py11_graph.cpp
		src/Graph.cpp)
	target_link_libraries(cpp_graph_module PRIVATE OpenMP::OpenMP_CXX)
	target_include_directories(cpp_graph_module PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(cpp_graph_module PROPERTIES LINKER_LANGUAGE CXX)
	
	pybind11_add_module(cpp_imputation_module
		src/pybind/py11_imputation.cpp
		src/Graph.cpp)
	target_link_libraries(cpp_imputation_module PRIVATE OpenMP::OpenMP_CXX)
	target_include_directories(cpp_imputation_module PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(cpp_imputation_module PROPERTIES LINKER_LANGUAGE CXX)

	pybind11_add_module(cpp_louvain_module
		src/pybind/py11_louvain.cpp
		src/Louvain.cpp
		src/DynamicGraph.cpp
		src/Graph.cpp)
	target_link_libraries(cpp_louvain_module PRIVATE OpenMP::OpenMP_CXX)
	target_include_directories(cpp_louvain_module PUBLIC ${PROJECT_SOURCE_DIR}/include)
	set_target_properties(cpp_louvain_module PROPERTIES LINKER_LANGUAGE CXX)

endif()
