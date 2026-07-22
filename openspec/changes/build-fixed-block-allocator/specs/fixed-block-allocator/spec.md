## ADDED Requirements

### Requirement: Fixed-size pool construction

The allocator SHALL allocate one contiguous raw byte buffer during construction and SHALL divide it into `block_capacity` blocks.

#### Scenario: Valid construction

- **WHEN** `FixedBlockAllocator` is constructed with positive `block_size` and `block_capacity`
- **THEN** it exposes the requested capacity
- **AND** no per-block heap container is required to track free blocks.

#### Scenario: Invalid construction

- **WHEN** `block_size` is zero or `block_capacity` is zero
- **THEN** construction SHALL throw `std::invalid_argument`.

### Requirement: O(1) allocation

`Allocate()` SHALL pop one block from the embedded free-list head in constant time.

#### Scenario: Space remains

- **WHEN** at least one block is free
- **THEN** `Allocate()` SHALL return a non-null block pointer
- **AND** reduce available capacity by one.

#### Scenario: Pool exhausted

- **WHEN** no blocks are free
- **THEN** `Allocate()` SHALL throw `std::bad_alloc`.

### Requirement: O(1) deallocation

`Deallocate(void* ptr)` SHALL push the returned block onto the embedded free-list head in constant time.

#### Scenario: Valid pointer

- **WHEN** `ptr` is a block pointer previously returned by this allocator
- **THEN** `Deallocate(ptr)` SHALL increase available capacity by one.

#### Scenario: Null pointer

- **WHEN** `ptr` is null
- **THEN** `Deallocate(ptr)` SHALL do nothing.

#### Scenario: Invalid pointer in debug builds

- **WHEN** `ptr` does not belong to the allocator buffer or is not aligned to a block boundary
- **THEN** debug assertions SHALL fail.

### Requirement: Build, test, and benchmark

The project SHALL provide CMake targets for the allocator library, Google Test unit tests, and Google Benchmark benchmarks.

#### Scenario: Unit validation

- **WHEN** tests are enabled
- **THEN** CTest SHALL execute allocation, deallocation, exhaustion, pointer arithmetic, and safety tests.

#### Scenario: Performance comparison

- **WHEN** benchmarks are enabled
- **THEN** benchmarks SHALL compare 10,000 fixed-size object allocations using standard `new`/`delete` and `FixedBlockAllocator`.
