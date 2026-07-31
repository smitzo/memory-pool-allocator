# Fixed-Size Memory Pool Allocator

A production-oriented C++17 fixed-block allocator for predictable allocation latency in embedded and systems software.

The allocator pre-allocates one contiguous byte buffer, divides it into equal blocks, and stores the free-list pointer inside each free block. Allocation and deallocation are O(1) pointer operations.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Debug builds enable AddressSanitizer on non-MSVC compilers.

## Benchmarks

```bash
cmake --build build --target memory_pool_benchmark
./build/memory_pool_benchmark
```

Benchmarks compare 10,000 `new`/`delete` operations against 10,000 `FixedBlockAllocator` allocations and deallocations. Exact timings vary by hardware, compiler, build type, and the platform allocator.

## Project Layout

```text
CMakeLists.txt
include/MemoryPool.h
src/MemoryPool.cpp
tests/test_memory_pool.cpp
benchmarks/benchmark_memory_pool.cpp
docs/
openspec/
.clang-format
```

## Planning

The implementation is tracked by OpenSpec change `build-fixed-block-allocator` under `openspec/changes/`.
