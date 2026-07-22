## 1. Planning

- [x] 1.1 Create OpenSpec proposal, design, requirement spec, and task checklist.
- [x] 1.2 Validate the OpenSpec change in strict mode.

## 2. Core Build And Allocator

- [x] 2.1 Add Modern CMake project with C++17, warnings-as-errors, Debug AddressSanitizer, tests, and benchmarks.
- [x] 2.2 Implement `FixedBlockAllocator` using a contiguous byte buffer and embedded free list.
- [x] 2.3 Add Google-style formatting configuration.

## 3. Verification

- [x] 3.1 Add GTest coverage for allocation, deallocation, exhaustion, pointer spacing, and safety checks.
- [x] 3.2 Add Google Benchmark comparison for 10,000 allocations and deallocations.
- [x] 3.3 Build and run tests with CTest.

## 4. Documentation

- [x] 4.1 Add docs for architecture, data flow, terminology, examples, and resource links.
- [x] 4.2 Keep commits split by planning, implementation, and documentation/benchmark polish.
