# Examples

## Raw Block Usage

```cpp
#include "MemoryPool.h"

memory_pool::FixedBlockAllocator allocator(64, 128);

void* first = allocator.Allocate();
void* second = allocator.Allocate();

allocator.Deallocate(second);
allocator.Deallocate(first);
```

## Object Usage

```cpp
#include "MemoryPool.h"

struct Packet {
  int id;
  char payload[60];
};

memory_pool::FixedBlockAllocator allocator(sizeof(Packet), 1024);

void* storage = allocator.Allocate();
auto* packet = new (storage) Packet{42, {}};

packet->~Packet();
allocator.Deallocate(packet);
```

## Exhaustion Handling

```cpp
try {
  memory_pool::FixedBlockAllocator allocator(sizeof(Packet), 1);
  void* a = allocator.Allocate();
  void* b = allocator.Allocate();  // throws std::bad_alloc
  allocator.Deallocate(b);
  allocator.Deallocate(a);
} catch (const std::bad_alloc&) {
  // Pool exhausted. Caller can drop work, retry later, or use another pool.
}
```
