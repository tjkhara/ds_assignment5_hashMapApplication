cmake_minimum_required(VERSION 3.14)
project(Assignment5 C)

set(CMAKE_C_STANDARD 99)

add_executable(Assignment5 tests.c hashMap.c hashMap.h CuTest.h CuTest.c)