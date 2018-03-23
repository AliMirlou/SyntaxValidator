cmake_minimum_required(VERSION 3.9)
project(SyntaxValidator)

set(CMAKE_CXX_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS  "${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(SyntaxValidator
        string_utils.h
        grammar_utils.h
        parse_table_generator.h
        scanner.h
        string_utils.cpp
        grammar_utils.cpp
        parse_table_generator.cpp
        scanner.cpp
        parser.cpp)