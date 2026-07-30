#include "MemoryPool.h"

#include <array>
#include <cstddef>

#include "benchmark/benchmark.h"

namespace {

struct alignas(16) BenchmarkObject {
  std::array<std::byte, 64> payload{};
};

constexpr int kObjectsPerIteration = 10000;

void BM_NewDelete(benchmark::State& state) {
  for (auto _ : state) {
    std::array<BenchmarkObject*, kObjectsPerIteration> objects{};
    for (BenchmarkObject*& object : objects) {
      object = new BenchmarkObject();
      benchmark::DoNotOptimize(object);
    }
    for (BenchmarkObject* object : objects) {
      benchmark::DoNotOptimize(object);
      delete object;
    }
  }
  state.SetItemsProcessed(state.iterations() * kObjectsPerIteration);
}

void BM_FixedBlockAllocator(benchmark::State& state) {
  memory_pool::FixedBlockAllocator allocator(sizeof(BenchmarkObject), kObjectsPerIteration);

  for (auto _ : state) {
    std::array<BenchmarkObject*, kObjectsPerIteration> objects{};
    for (BenchmarkObject*& object : objects) {
      void* storage = allocator.Allocate();
      object = new (storage) BenchmarkObject();
      benchmark::DoNotOptimize(object);
    }
    for (BenchmarkObject* object : objects) {
      benchmark::DoNotOptimize(object);
      object->~BenchmarkObject();
      allocator.Deallocate(object);
    }
  }
  state.SetItemsProcessed(state.iterations() * kObjectsPerIteration);
}

BENCHMARK(BM_NewDelete);
BENCHMARK(BM_FixedBlockAllocator);

}  // namespace
