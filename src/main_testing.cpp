#include <iostream>

#include "DenseVector.hpp"

#include <gtest/gtest.h>

class VectorGTest : public testing::Test {};

TEST(VectorGTest, testScaling) {
	DenseVector v{1,2,3,4};
   	auto u = v * 3;
	
	EXPECT_NE(u,v);
	EXPECT_EQ(v,v);
}

int main(int, char**){
	
	::testing::InitGoogleTest();

	std::cout << "Starting unit tests.\n";
	return RUN_ALL_TESTS();
}
