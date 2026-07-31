# Data Flow

This allocator has two hot paths: allocation and deallocation.

## Construction Flow

1. Validate `block_size > 0` and `block_capacity > 0`.
2. Round `block_size` up to the effective block size.
3. Check that `block_size * block_capacity` fits in `std::size_t`.
4. Allocate one byte buffer.
5. Walk the buffer and link every block to the next block.

For capacity 4, the initial free list looks like this:

```text
free_list_head_
      |
      v
+---------+    +---------+    +---------+    +---------+
| block 0 | -> | block 1 | -> | block 2 | -> | block 3 | -> null
+---------+    +---------+    +---------+    +---------+
```

## Allocation Flow

`Allocate()` reads the current head, moves the head to `head->next`, decrements `available_blocks_`, and returns the old head as raw storage.

```text
before: head -> block 0 -> block 1 -> block 2
return: block 0
after:  head -> block 1 -> block 2
```

If the head is null, no blocks remain and the function throws `std::bad_alloc`.

## Deallocation Flow

`Deallocate(ptr)` asserts that the pointer belongs to the buffer and starts at a valid block boundary. It then writes the old head into `ptr->next` and makes `ptr` the new head.

```text
before: head -> block 1 -> block 2
free:   block 0
after:  head -> block 0 -> block 1 -> block 2
```

Passing null is a no-op, matching the behavior programmers expect from `delete`.
