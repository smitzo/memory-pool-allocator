## Overview

`FixedBlockAllocator` owns one contiguous `unsigned char[]` allocation. The constructor divides that buffer into equal-sized blocks and stores the free-list next pointer inside each free block. Allocation pops the head pointer; deallocation pushes a block pointer back to the head. Both paths touch a constant number of pointers.

## Key Decisions

- Use `std::unique_ptr<unsigned char[]>` for single-buffer ownership while exposing only `void*` allocation results.
- Round `block_size` up to at least `sizeof(void*)` and `alignof(std::max_align_t)` so a free block can safely store a next pointer and satisfy normal object alignment.
- Keep the implementation non-copyable and movable. Moving transfers the buffer and current free-list state without copying blocks.
- Reject invalid constructor inputs with `std::invalid_argument`.
- Throw `std::bad_alloc` on exhausted capacity.
- Use `assert` for invalid deallocation checks, matching the requested debug safety model without adding release-build overhead.

## Data Flow

1. Construction allocates `block_size * block_capacity` bytes.
2. Construction walks the buffer once and links block `i` to block `i + 1`.
3. `Allocate()` returns the current free-list head and advances the head to the embedded next pointer.
4. `Deallocate(ptr)` checks that `ptr` belongs to the buffer and is block-aligned, then writes the previous head into `ptr` and makes `ptr` the new head.

## Constraints

- No per-block `std::vector`, `std::list`, or heap allocations after construction.
- Allocation and deallocation must remain O(1).
- The allocator is not thread-safe in this change.
- Returned memory is raw storage; callers construct and destroy objects separately if needed.

## Validation

- Build with C++17 and warnings-as-errors.
- Run Google Test through CTest.
- Run Debug builds under AddressSanitizer.
- Run Google Benchmark for comparative timing. The benchmark demonstrates the intended performance shape, but exact results depend on compiler, CPU, allocator, and build type.
