#ifndef MEMORY_POOL_ALLOCATOR_INCLUDE_MEMORYPOOL_H_
#define MEMORY_POOL_ALLOCATOR_INCLUDE_MEMORYPOOL_H_

#include <cstddef>
#include <memory>

namespace memory_pool {

class FixedBlockAllocator {
 public:
  FixedBlockAllocator(std::size_t block_size, std::size_t block_capacity);
  ~FixedBlockAllocator() = default;

  FixedBlockAllocator(const FixedBlockAllocator&) = delete;
  FixedBlockAllocator& operator=(const FixedBlockAllocator&) = delete;

  FixedBlockAllocator(FixedBlockAllocator&& other) noexcept;
  FixedBlockAllocator& operator=(FixedBlockAllocator&& other) noexcept;

  void* Allocate();
  void Deallocate(void* ptr) noexcept;

  [[nodiscard]] std::size_t block_size() const noexcept { return block_size_; }
  [[nodiscard]] std::size_t block_capacity() const noexcept { return block_capacity_; }
  [[nodiscard]] std::size_t available_blocks() const noexcept { return available_blocks_; }
  [[nodiscard]] bool Owns(const void* ptr) const noexcept;

 private:
  struct FreeBlock {
    FreeBlock* next;
  };

  [[nodiscard]] bool IsAlignedBlockPointer(const void* ptr) const noexcept;
  void InitializeFreeList() noexcept;

  std::size_t block_size_ = 0;
  std::size_t block_capacity_ = 0;
  std::size_t available_blocks_ = 0;
  std::unique_ptr<unsigned char[]> buffer_;
  FreeBlock* free_list_head_ = nullptr;
};

}  // namespace memory_pool

#endif  // MEMORY_POOL_ALLOCATOR_INCLUDE_MEMORYPOOL_H_
