cmake_minimum_required(VERSION 3.21)
project(huffman)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)

# NOTE: huffman functional must be implemented as library and linked to the executables
# NOTE: tool target name must be huffman-tool

include_directories(huffman_lib)

add_executable(tests unit-tests/tests.cpp)

set(lib_huffman
        huffman_lib/bstream.cpp
        huffman_lib/bstream.h
        huffman_lib/huffman.h
        huffman_lib/huffman_code.cpp
        huffman_lib/huffman_code.h
        huffman_lib/huffman_tree.h
        huffman_lib/huffman_codes_examples/ct_string.cpp
        huffman_lib/huffman_codes_examples/ct_vector_bool.cpp
        huffman_lib/huffman_codes_examples/ct_vector_int.cpp
        )

add_executable(huffman-tool ${lib_huffman} huffman-tool.cpp)

# don't forget to use same flags at your library
if (NOT MSVC)
  target_compile_options(tests PRIVATE -Wall -Wno-sign-compare -pedantic)
endif()

option(USE_SANITIZERS "Enable to build with undefined,leak and address sanitizers" OFF)
if (USE_SANITIZERS)
  target_compile_options(tests PUBLIC -fsanitize=address,undefined,leak -fno-sanitize-recover=all)
  target_link_options(tests PUBLIC -fsanitize=address,undefined,leak)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#  target_compile_options(tests PUBLIC -stdlib=libc++)
  target_compile_options(tests PUBLIC -std=c++2a)
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  target_compile_options(tests PUBLIC -D_GLIBCXX_DEBUG)
endif()

target_link_libraries(tests GTest::gtest GTest::gtest_main)
