#include <iostream>
#include <gtest/gtest.h>
#include "Example.hpp"

class ExampleTest : public testing::Test {};

TEST(ExampleTest, getString) {
	Example example("Hello World");
	EXPECT_EQ(example.getString(), "Hello World");
	EXPECT_NE(example.getString(), "Hello");
}
int main(int, char**){
	
	::testing::InitGoogleTest();

	std::cout << "Starting unit tests.\n";
	return RUN_ALL_TESTS();
}
