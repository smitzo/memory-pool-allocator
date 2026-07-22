## Why

General-purpose heap allocators trade flexibility for unpredictable latency and fragmentation. This project needs a fixed-size allocator with deterministic O(1) allocation and deallocation for embedded and systems workloads.

## What Changes

- Add a C++17 `FixedBlockAllocator` with a contiguous raw byte buffer and an embedded singly-linked free list.
- Provide a Modern CMake build with warnings-as-errors, Debug AddressSanitizer, Google Test, and Google Benchmark targets.
- Add unit tests for allocation/deallocation, exhaustion, pointer spacing, ownership checks, and invalid construction.
- Add benchmarks comparing repeated `new`/`delete` against allocator-backed fixed-block allocation.
- Add docs that explain architecture, data flow, terminology, examples, and resource links.

## Capabilities

### New Capabilities

- `fixed-block-allocator`: Fixed-size block allocation with deterministic free-list operations, safety checks, and C++17 build/test/benchmark support.

### Modified Capabilities

- None.

## Impact

- New public API in `include/MemoryPool.h`.
- New implementation in `src/MemoryPool.cpp`.
- New test and benchmark dependencies fetched by CMake.
- New documentation under `docs/`.
