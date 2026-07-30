#include "MemoryPool.h"

#include <cstdint>
#include <limits>
#include <new>
#include <stdexcept>
#include <vector>

#include "gtest/gtest.h"

namespace memory_pool {
namespace {

TEST(FixedBlockAllocatorTest, AllocatesAndDeallocatesMultipleBlocks) {
  FixedBlockAllocator allocator(/*block_size=*/32, /*block_capacity=*/3);

  void* first = allocator.Allocate();
  void* second = allocator.Allocate();
  void* third = allocator.Allocate();

  EXPECT_NE(first, nullptr);
  EXPECT_NE(second, nullptr);
  EXPECT_NE(third, nullptr);
  EXPECT_EQ(allocator.available_blocks(), 0U);

  allocator.Deallocate(second);
  EXPECT_EQ(allocator.available_blocks(), 1U);

  void* reused = allocator.Allocate();
  EXPECT_EQ(reused, second);
  EXPECT_EQ(allocator.available_blocks(), 0U);

  allocator.Deallocate(first);
  allocator.Deallocate(third);
  allocator.Deallocate(reused);
  EXPECT_EQ(allocator.available_blocks(), 3U);
}

TEST(FixedBlockAllocatorTest, ThrowsWhenCapacityIsExhausted) {
  FixedBlockAllocator allocator(/*block_size=*/16, /*block_capacity=*/1);

  void* block = allocator.Allocate();
  EXPECT_THROW(allocator.Allocate(), std::bad_alloc);

  allocator.Deallocate(block);
}

TEST(FixedBlockAllocatorTest, ReturnedPointersAreExactlyBlockSizeApart) {
  constexpr std::size_t kBlockSize = 64;
  FixedBlockAllocator allocator(kBlockSize, /*block_capacity=*/3);

  auto* first = static_cast<unsigned char*>(allocator.Allocate());
  auto* second = static_cast<unsigned char*>(allocator.Allocate());
  auto* third = static_cast<unsigned char*>(allocator.Allocate());

  EXPECT_EQ(second - first, static_cast<std::ptrdiff_t>(allocator.block_size()));
  EXPECT_EQ(third - second, static_cast<std::ptrdiff_t>(allocator.block_size()));
  EXPECT_EQ(allocator.block_size(), kBlockSize);

  allocator.Deallocate(third);
  allocator.Deallocate(second);
  allocator.Deallocate(first);
}

TEST(FixedBlockAllocatorTest, SmallBlockSizeIsRoundedForPointerStorage) {
  FixedBlockAllocator allocator(/*block_size=*/1, /*block_capacity=*/2);

  EXPECT_GE(allocator.block_size(), sizeof(void*));
  EXPECT_EQ(allocator.block_size() % alignof(std::max_align_t), 0U);
}

TEST(FixedBlockAllocatorTest, RejectsInvalidConstructionArguments) {
  EXPECT_THROW(FixedBlockAllocator(0, 1), std::invalid_argument);
  EXPECT_THROW(FixedBlockAllocator(1, 0), std::invalid_argument);
  EXPECT_THROW(FixedBlockAllocator(std::numeric_limits<std::size_t>::max(), 2),
               std::invalid_argument);
}

TEST(FixedBlockAllocatorTest, NullDeallocationIsNoOp) {
  FixedBlockAllocator allocator(/*block_size=*/32, /*block_capacity=*/2);

  allocator.Deallocate(nullptr);

  EXPECT_EQ(allocator.available_blocks(), 2U);
}

TEST(FixedBlockAllocatorTest, OwnsOnlyPointersInsideBuffer) {
  FixedBlockAllocator allocator(/*block_size=*/32, /*block_capacity=*/2);
  std::vector<unsigned char> outside(32);
  void* block = allocator.Allocate();

  EXPECT_TRUE(allocator.Owns(block));
  EXPECT_FALSE(allocator.Owns(outside.data()));
  EXPECT_FALSE(allocator.Owns(nullptr));

  allocator.Deallocate(block);
}

TEST(FixedBlockAllocatorDeathTest, InvalidPointerFreeFailsDebugAssertion) {
#if defined(NDEBUG)
  GTEST_SKIP() << "assertions are disabled in release builds";
#else
  FixedBlockAllocator allocator(/*block_size=*/32, /*block_capacity=*/2);
  unsigned char outside[32] = {};

  EXPECT_DEATH(allocator.Deallocate(outside), "pointer does not belong");
#endif
}

TEST(FixedBlockAllocatorDeathTest, MisalignedPointerFreeFailsDebugAssertion) {
#if defined(NDEBUG)
  GTEST_SKIP() << "assertions are disabled in release builds";
#else
  FixedBlockAllocator allocator(/*block_size=*/32, /*block_capacity=*/2);
  auto* block = static_cast<unsigned char*>(allocator.Allocate());

  EXPECT_DEATH(allocator.Deallocate(block + 1), "pointer is not aligned");

  allocator.Deallocate(block);
#endif
}

TEST(FixedBlockAllocatorDeathTest, DoubleFreeFailsDebugAssertion) {
#if defined(NDEBUG)
  GTEST_SKIP() << "assertions are disabled in release builds";
#else
  FixedBlockAllocator allocator(/*block_size=*/32, /*block_capacity=*/1);
  void* block = allocator.Allocate();

  allocator.Deallocate(block);
  EXPECT_DEATH(allocator.Deallocate(block), "deallocation would exceed");
#endif
}

}  // namespace
}  // namespace memory_pool
