#include "MemoryPool.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace memory_pool {
namespace {

std::size_t AlignUp(std::size_t value, std::size_t alignment) {
  const std::size_t remainder = value % alignment;
  if (remainder == 0) {
    return value;
  }
  return value + alignment - remainder;
}

std::size_t NormalizeBlockSize(std::size_t block_size) {
  if (block_size == 0) {
    throw std::invalid_argument("block_size must be greater than zero");
  }

  const std::size_t minimum_block_size = std::max(block_size, sizeof(void*));
  const std::size_t alignment = alignof(std::max_align_t);
  if (minimum_block_size > std::numeric_limits<std::size_t>::max() - (alignment - 1)) {
    throw std::invalid_argument("requested block size overflows size_t");
  }
  return AlignUp(minimum_block_size, alignment);
}

}  // namespace

FixedBlockAllocator::FixedBlockAllocator(std::size_t block_size, std::size_t block_capacity)
    : block_size_(NormalizeBlockSize(block_size)),
      block_capacity_(block_capacity),
      available_blocks_(block_capacity),
      buffer_(nullptr),
      free_list_head_(nullptr) {
  if (block_capacity == 0) {
    throw std::invalid_argument("block_capacity must be greater than zero");
  }
  if (block_capacity_ > std::numeric_limits<std::size_t>::max() / block_size_) {
    throw std::invalid_argument("requested pool size overflows size_t");
  }

  buffer_ = std::make_unique<unsigned char[]>(block_size_ * block_capacity_);
  InitializeFreeList();
}

FixedBlockAllocator::FixedBlockAllocator(FixedBlockAllocator&& other) noexcept
    : block_size_(std::exchange(other.block_size_, 0)),
      block_capacity_(std::exchange(other.block_capacity_, 0)),
      available_blocks_(std::exchange(other.available_blocks_, 0)),
      buffer_(std::move(other.buffer_)),
      free_list_head_(std::exchange(other.free_list_head_, nullptr)) {}

FixedBlockAllocator& FixedBlockAllocator::operator=(FixedBlockAllocator&& other) noexcept {
  if (this == &other) {
    return *this;
  }

  block_size_ = std::exchange(other.block_size_, 0);
  block_capacity_ = std::exchange(other.block_capacity_, 0);
  available_blocks_ = std::exchange(other.available_blocks_, 0);
  buffer_ = std::move(other.buffer_);
  free_list_head_ = std::exchange(other.free_list_head_, nullptr);
  return *this;
}

void* FixedBlockAllocator::Allocate() {
  if (free_list_head_ == nullptr) {
    throw std::bad_alloc();
  }

  FreeBlock* allocated_block = free_list_head_;
  free_list_head_ = free_list_head_->next;
  --available_blocks_;
  return allocated_block;
}

void FixedBlockAllocator::Deallocate(void* ptr) noexcept {
  if (ptr == nullptr) {
    return;
  }

  assert(Owns(ptr) && "pointer does not belong to this allocator");
  assert(IsAlignedBlockPointer(ptr) && "pointer is not aligned to a block boundary");
  assert(available_blocks_ < block_capacity_ && "deallocation would exceed pool capacity");

  auto* block = static_cast<FreeBlock*>(ptr);
  block->next = free_list_head_;
  free_list_head_ = block;
  ++available_blocks_;
}

bool FixedBlockAllocator::Owns(const void* ptr) const noexcept {
  if (ptr == nullptr || buffer_ == nullptr) {
    return false;
  }

  const auto* byte_ptr = static_cast<const unsigned char*>(ptr);
  const unsigned char* begin = buffer_.get();
  const unsigned char* end = begin + (block_size_ * block_capacity_);
  return byte_ptr >= begin && byte_ptr < end;
}

bool FixedBlockAllocator::IsAlignedBlockPointer(const void* ptr) const noexcept {
  if (!Owns(ptr)) {
    return false;
  }

  const auto* byte_ptr = static_cast<const unsigned char*>(ptr);
  const std::ptrdiff_t offset = byte_ptr - buffer_.get();
  return offset >= 0 && static_cast<std::size_t>(offset) % block_size_ == 0;
}

void FixedBlockAllocator::InitializeFreeList() noexcept {
  unsigned char* current = buffer_.get();
  for (std::size_t i = 0; i < block_capacity_; ++i) {
    auto* block = reinterpret_cast<FreeBlock*>(current + (i * block_size_));
    if (i + 1 < block_capacity_) {
      block->next = reinterpret_cast<FreeBlock*>(current + ((i + 1) * block_size_));
    } else {
      block->next = nullptr;
    }
  }
  free_list_head_ = reinterpret_cast<FreeBlock*>(current);
}

}  // namespace memory_pool
