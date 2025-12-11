cmake_minimum_required(VERSION 3.14)
project(BigUIntLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_library(LAB1 BigUInt.cpp)

add_executable(LAB1_app main.cpp)
target_link_libraries(LAB1_app PRIVATE LAB1)

enable_testing()

add_executable(bigint_tests test_biguint.cpp)
target_link_libraries(bigint_tests PRIVATE LAB1 GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(bigint_tests)