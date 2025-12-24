# High-Performance Time-Series Database

A fast, memory-efficient time-series storage engine written in C++17. Designed for financial tick data and similar high-throughput time-series workloads.

## Features

- **Columnar storage** for cache-efficient queries
- **Delta encoding** for timestamp compression
- **Binary search indexing** for O(log n) range queries
- **Sub-millisecond query latency** over millions of data points

## Performance

Benchmarked on Apple M-series CPU:

| Metric | Result |
|--------|--------|
| Ingestion throughput | 22M points/sec |
| Range query (1K points from 10M) | 40 µs |
| Full aggregation (10M points) | 22 ms |
| Timestamp compression | 1.5x |

Compared to naive linear-scan baseline:
- **17x faster** range queries
- **30x faster** aggregations

## Building
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Usage
```cpp
#include <tsdb/database.hpp>

int main() {
    tsdb::Database db;
    
    // Create a series
    db.create_series("AAPL");
    
    // Append data points
    db.append("AAPL", 1000000000, 150.25);
    db.append("AAPL", 1000001000, 150.30);
    db.append("AAPL", 1000002000, 150.28);
    
    // Range query
    auto points = db.range("AAPL", 1000000000, 1000002000);
    
    // Aggregation
    auto stats = db.aggregate("AAPL", 1000000000, 1000002000);
    // stats.sum, stats.avg, stats.min, stats.max, stats.count
    
    return 0;
}
```

## Running the Demo
```bash
./demo
```

## Running Benchmarks
```bash
./bench/bench_tsdb
```

## Architecture
```
┌─────────────────────────────────────────────────────────┐
│                      Database                           │
│         std::unordered_map<string, Series>              │
└─────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────┐
│                       Series                            │
│  ┌─────────────────────────────────────────────────┐   │
│  │  Columnar Storage                                │   │
│  │  timestamps_: [t1, t2, t3, ...]                  │   │
│  │  values_:     [v1, v2, v3, ...]                  │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  • Binary search for O(log n) range lookups            │
│  • Delta encoding for timestamp compression            │
└─────────────────────────────────────────────────────────┘
```

## Design Decisions

**Why columnar storage?**  
Time-series queries typically scan timestamps first to find a range, then access values. Columnar layout keeps timestamps contiguous in memory, maximizing cache efficiency during scans.

**Why delta encoding?**  
Timestamps in time-series data are often regular (e.g., every millisecond). Storing differences between consecutive timestamps instead of absolute values reduces storage significantly.

**Why binary search?**  
Data is append-only and sorted by time. Binary search finds range boundaries in O(log n) vs O(n) for linear scan—critical when querying millions of points.

## Future Improvements

- Value compression (XOR encoding for floats)
- Time-based segmentation for memory management
- Concurrent read/write support
- Disk persistence

## License

MIT