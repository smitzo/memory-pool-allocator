# Build And Test

## Debug Validation

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Debug builds enable AddressSanitizer on non-MSVC compilers. This helps detect leaks, invalid accesses, and use-after-free bugs in tests.

## Release Benchmarking

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target memory_pool_benchmark
./build-release/memory_pool_benchmark
```

## Configurable CMake Options

- `MEMORY_POOL_BUILD_TESTS=ON|OFF`
- `MEMORY_POOL_BUILD_BENCHMARKS=ON|OFF`

Disabling benchmarks is useful for offline or dependency-restricted environments.
