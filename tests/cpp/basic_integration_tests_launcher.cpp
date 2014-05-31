#include "gtest/gtest.h"
#include "GNG.h"


#include "basic_integration_tests.cpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
