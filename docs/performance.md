# Performance Notes

## Why A Fixed Pool Is Fast

General-purpose allocation must handle many sizes, alignment needs, fragmentation patterns, and synchronization cases. A fixed-block pool removes most of that work by knowing every block has the same size.

The hot path is:

```text
Allocate:   old = head; head = old->next; return old
Deallocate: ptr->next = head; head = ptr
```

No size-class lookup, no free-space search, and no per-block heap metadata allocation are needed.

## Benchmark Shape

The benchmark allocates and deallocates 10,000 `BenchmarkObject` instances per iteration using:

- standard `new` and `delete`,
- `FixedBlockAllocator` plus placement new and explicit destruction.

The custom allocator should usually win on this fixed-size workload because the pool does not need to search for memory or call the general heap for each object.

## Interpreting Results

Benchmarks should be run in Release mode for final performance numbers:

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target memory_pool_benchmark
./build-release/memory_pool_benchmark
```

Debug AddressSanitizer builds are for correctness checks, not final speed comparisons.
