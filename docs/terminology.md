# Terminology

## Allocator

An object responsible for handing out and reclaiming raw memory. This project exposes `FixedBlockAllocator`.

## Pool

The complete pre-allocated memory region owned by the allocator.

## Block

One fixed-size slice of the pool. If a pool has capacity 100 and an effective block size of 64 bytes, the buffer contains 100 blocks and consumes 6,400 bytes.

## Free List

A linked list of blocks that are available for reuse. This allocator embeds the link pointer inside the free block itself, so it does not allocate separate list nodes.

## O(1)

Constant-time complexity. `Allocate()` and `Deallocate()` do not search through the pool; they only update the head of the free list.

## Placement New

The C++ syntax for constructing an object into raw storage:

```cpp
void* storage = allocator.Allocate();
auto* object = new (storage) MyObject();
object->~MyObject();
allocator.Deallocate(object);
```

The allocator only manages memory. Object construction and destruction remain the caller's responsibility.
