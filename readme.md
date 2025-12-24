# High-Performance Time-Series Database

![CI](https://github.com/samrichell-smith/time-series-database/actions/workflows/ci.yml/badge.svg)

A fast, memory-efficient time-series storage engine written in C++17. Designed for financial tick data and high-throughput workloads where latency matters.

## Features

- **Columnar storage** — timestamps and values stored separately for cache-efficient sequential access
- **Delta encoding** — timestamp compression exploiting the regularity of tick data intervals
- **Binary search indexing** — O(log n) range queries over sorted time-series data
- **Sub-millisecond latency** — 40µs to query 1,000 points from 10 million

## Performance

Benchmarked on Apple M-series CPU with 10 million data points:

| Metric | Result |
|--------|--------|
| Ingestion throughput | 22M points/sec |
| Range query (1K points from 10M) | 40 µs |
| Full aggregation (10M points) | 22 ms |
| Timestamp compression | 1.5x |

Compared against naive row-oriented storage with linear scan:

| Operation | Optimized | Naive | Speedup |
|-----------|-----------|-------|---------|
| Range query | 0.35 ms | 6.1 ms | **17x** |
| Aggregation | 0.18 ms | 5.6 ms | **30x** |

## Building
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Running
```bash
./demo                    # Demo with performance output
./bench/bench_tsdb        # Google Benchmark suite
./tests/tests             # Google Test unit tests
```

## Usage
```cpp
#include <tsdb/database.hpp>

int main() {
    tsdb::Database db;
    db.create_series("AAPL");
    
    // Append tick data (timestamp in nanoseconds, price)
    db.append("AAPL", 1000000000, 150.25);
    db.append("AAPL", 1000001000, 150.30);
    db.append("AAPL", 1000002000, 150.28);
    
    // Range query
    auto points = db.range("AAPL", 1000000000, 1000002000);
    
    // Aggregation
    auto stats = db.aggregate("AAPL", 1000000000, 1000002000);
    // stats.sum, stats.avg, stats.min, stats.max, stats.count
}
```

## Architecture
```
┌─────────────────────────────────────────────────────────────┐
│                        Database                             │
│           std::unordered_map<string, Series>                │
└─────────────────────────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────┐
│                         Series                              │
│                                                             │
│  timestamps_: [t₀, t₁, t₂, t₃, ...]  ← contiguous          │
│  values_:     [v₀, v₁, v₂, v₃, ...]  ← contiguous          │
│                                                             │
│  • Binary search for O(log n) range lookups                │
│  • Delta encoding for timestamp compression                │
└─────────────────────────────────────────────────────────────┘
```

## Design Decisions

**Columnar storage**: When scanning timestamps to find a range, every byte loaded into cache is useful. Row-oriented layout wastes half the cache on values we don't need yet.

**Delta encoding**: Tick data arrives at regular intervals. Storing deltas instead of absolute timestamps reduces storage—a 1ms interval fits in 2 bytes vs 8 for the full timestamp.

**Binary search**: For 10M points, linear scan checks ~5M timestamps on average. Binary search checks 24. That's the difference between 6ms and 40µs.

## Project Structure
```
├── include/tsdb/       # Headers: types, series, database, encoding
├── src/                # Implementation
├── bench/              # Google Benchmark performance tests
├── tests/              # Google Test unit tests
└── examples/           # Demo application
```

## Future Work

- XOR compression for float values (Gorilla-style)
- Time-based segmentation
- Concurrent read/write support
- Disk persistence

## License

MIT