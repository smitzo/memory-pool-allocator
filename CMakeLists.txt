cmake_minimum_required(VERSION 3.20)

project(
  MemoryPoolAllocator
  VERSION 0.1.0
  DESCRIPTION "Fixed-size memory pool allocator in C++17"
  LANGUAGES CXX)

option(MEMORY_POOL_BUILD_TESTS "Build unit tests" ON)
option(MEMORY_POOL_BUILD_BENCHMARKS "Build benchmarks" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(memory_pool src/MemoryPool.cpp)
add_library(memory_pool::memory_pool ALIAS memory_pool)

target_include_directories(
  memory_pool
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)

target_compile_features(memory_pool PUBLIC cxx_std_17)
set_target_properties(memory_pool PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
  target_compile_options(memory_pool PRIVATE /W4 /WX)
else()
  target_compile_options(memory_pool PRIVATE -Wall -Wextra -Werror)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
  target_compile_options(memory_pool PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(memory_pool PRIVATE -fsanitize=address)
endif()

include(FetchContent)

if(MEMORY_POOL_BUILD_TESTS)
  enable_testing()

  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(memory_pool_tests tests/test_memory_pool.cpp)
  target_link_libraries(memory_pool_tests PRIVATE memory_pool::memory_pool GTest::gtest_main)
  target_compile_features(memory_pool_tests PRIVATE cxx_std_17)
  set_target_properties(memory_pool_tests PROPERTIES CXX_EXTENSIONS OFF)

  if(MSVC)
    target_compile_options(memory_pool_tests PRIVATE /W4 /WX)
  else()
    target_compile_options(memory_pool_tests PRIVATE -Wall -Wextra -Werror)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(memory_pool_tests PRIVATE -fsanitize=address -fno-omit-frame-pointer)
      target_link_options(memory_pool_tests PRIVATE -fsanitize=address)
    endif()
  endif()

  include(GoogleTest)
  gtest_add_tests(TARGET memory_pool_tests)
endif()

if(MEMORY_POOL_BUILD_BENCHMARKS)
  FetchContent_Declare(
    benchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.8.5.tar.gz)
  set(BENCHMARK_ENABLE_TESTING
      OFF
      CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_INSTALL
      OFF
      CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(benchmark)

  add_executable(memory_pool_benchmark benchmarks/benchmark_memory_pool.cpp)
  target_link_libraries(memory_pool_benchmark PRIVATE memory_pool::memory_pool benchmark::benchmark_main)
  target_compile_features(memory_pool_benchmark PRIVATE cxx_std_17)
  set_target_properties(memory_pool_benchmark PROPERTIES CXX_EXTENSIONS OFF)

  if(MSVC)
    target_compile_options(memory_pool_benchmark PRIVATE /W4 /WX)
  else()
    target_compile_options(memory_pool_benchmark PRIVATE -Wall -Wextra -Werror)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(memory_pool_benchmark PRIVATE -fsanitize=address -fno-omit-frame-pointer)
      target_link_options(memory_pool_benchmark PRIVATE -fsanitize=address)
    endif()
  endif()
endif()
