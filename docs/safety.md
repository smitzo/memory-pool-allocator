# Safety Model

The allocator provides deterministic memory management, not automatic object lifetime management.

## Constructor Safety

The constructor rejects zero block sizes, zero capacities, and pool sizes that overflow `std::size_t`.

## Out Of Memory

When all blocks are allocated, `Allocate()` throws `std::bad_alloc`. It does not fall back to `new`, because hidden fallback allocation would break the allocator's predictable-latency contract.

## Deallocation Checks

`Deallocate()` uses debug assertions to check that a pointer:

- is not outside the owned buffer,
- starts on a block boundary,
- does not make the available count exceed capacity.

These checks catch common ownership bugs during development while keeping release overhead low.

## Double Free

The available-count assertion catches a double free after a one-block pool has already returned to full capacity. A production allocator can add stronger duplicate-free detection with debug bitmaps, but that adds memory overhead and is outside the current fixed-size hot-path scope.

## Threading

This allocator is not thread-safe. Use external locking, thread-local pools, or a future lock-free/free-list design if multiple threads must share one pool.
