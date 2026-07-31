# Architecture

`FixedBlockAllocator` is a single-owner memory pool. It is designed for workloads where every allocation has the same maximum size and predictable latency is more important than arbitrary allocation sizes.

## Core Components

- `buffer_`: the only heap allocation made by the pool. It is a contiguous `unsigned char[]` region sized as `block_size * block_capacity`.
- `block_size_`: the effective block size. The constructor rounds the requested size up so every block can hold a `FreeBlock*` and satisfy `alignof(std::max_align_t)`.
- `free_list_head_`: a pointer to the first free block.
- `available_blocks_`: an O(1) counter for remaining free blocks.

## Invariants

- Every returned pointer is inside `buffer_`.
- Every returned pointer starts at a block boundary.
- Every free block stores its next pointer in the first bytes of the block.
- Allocating pops exactly one free-list node.
- Deallocating pushes exactly one free-list node.
- No per-block container allocation occurs after construction.

## Complexity

Construction is O(n) because the allocator links every block once. `Allocate()` and `Deallocate()` are O(1) because each operation updates a constant number of pointers and counters.

## Tradeoffs

The allocator is intentionally fixed-size. That keeps allocation deterministic, but callers must choose a block size large enough for the objects stored in the pool. It is not thread-safe; a caller that shares one allocator across threads must add external synchronization or use one allocator per thread.
