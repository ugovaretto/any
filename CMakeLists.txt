cmake_minimum_required(VERSION 3.6)
project(any)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
    include/Any.h
    include/AnyPolicies.h
    include/Operators.h
    test/any-test.cpp include/utility.h)

add_executable(any-test ${SOURCE_FILES})
target_include_directories(any-test PRIVATE include)