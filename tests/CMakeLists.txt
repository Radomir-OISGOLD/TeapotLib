cmake_minimum_required(VERSION 3.16)

# Add the test executable
add_executable(device_test device_test.cpp)

# Link our test against the main library (TeapotLib), GTest, and its dependencies.
# The GTest targets are available from the parent project's FetchContent.
target_link_libraries(device_test PRIVATE
    GTest::gtest_main
    TeapotLib
)

# Add the test to CTest so it can be discovered and run.
include(GoogleTest)
gtest_discover_tests(device_test)
