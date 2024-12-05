#include <gtest/gtest.h>

#include <iostream>

#include "Example.hpp"

class ExampleTest : public testing::Test {};

TEST(ExampleTest, get_string) {
    Example example("Hello World");
    EXPECT_EQ(example.get_string(), "Hello World");
    EXPECT_NE(example.get_string(), "Hello");
}
int main(int, char**) {
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}