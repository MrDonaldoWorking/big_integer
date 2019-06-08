cmake_minimum_required(VERSION 3.12)

project(BIGINT)

include_directories(${BIGINT_SOURCE_DIR}
                    gtest)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -O3")
endif()

add_library(big_int_lib STATIC
            big_integer.h big_integer.cpp
            opt_vector.h opt_vector.cpp
            gtest/gtest-all.cc
            gtest/gtest.h
            gtest/gtest_main.cc)

add_executable(big_integer_testing big_integer_testing.cpp)

target_link_libraries(big_integer_testing big_int_lib -lpthread)