#include <tsdb/database.hpp>
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>

std::vector<tsdb::Point> generate_data(std::size_t count, unsigned int seed) {
    std::vector<tsdb::Point> points;
    points.reserve(count);
    
    std::mt19937_64 rng(seed);
    std::normal_distribution<double> interval_dist(1'000'000, 100'000);
    std::normal_distribution<double> value_dist(0.0, 0.001);
    
    tsdb::Timestamp time = 1'000'000'000'000'000'000LL;
    double value = 100.0 + (seed % 100);
    
    for (std::size_t i = 0; i < count; i++) {
        points.push_back(tsdb::Point{time, value});
        time += static_cast<int64_t>(std::max(1.0, interval_dist(rng)));
        value *= (1.0 + value_dist(rng));
    }
    
    return points;
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         High-Performance Time-Series Database Demo           ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

    tsdb::Database db;
    const std::size_t points_per_series = 10'000'000;
    
    // Create series
    std::cout << "Creating series: AAPL, GOOG, MSFT\n";
    db.create_series("AAPL");
    db.create_series("GOOG");
    db.create_series("MSFT");
    
    // Ingest data
    std::cout << "Generating and ingesting " << points_per_series / 1'000'000 << "M points per series...\n\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& name : {"AAPL", "GOOG", "MSFT"}) {
        auto data = generate_data(points_per_series, std::hash<std::string>{}(name));
        db.get_series(name).append_batch(data);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::size_t total_points = db.total_points();
    double seconds = duration.count() / 1000.0;
    double points_per_sec = total_points / seconds;
    
    std::cout << "┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ INGESTION PERFORMANCE                                       │\n";
    std::cout << "├─────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Total points:     " << std::setw(12) << total_points << "                          │\n";
    std::cout << "│ Time:             " << std::setw(12) << std::fixed << std::setprecision(2) << seconds << " sec                       │\n";
    std::cout << "│ Throughput:       " << std::setw(12) << std::fixed << std::setprecision(1) << points_per_sec / 1'000'000 << " M points/sec               │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n\n";
    
    // Memory stats
    std::size_t raw_size = db.total_memory_bytes();
    std::size_t compressed_size = 0;
    for (const auto& name : {"AAPL", "GOOG", "MSFT"}) {
        compressed_size += db.get_series(name).compressed_size_bytes();
    }
    
    std::cout << "┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ MEMORY USAGE                                                │\n";
    std::cout << "├─────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Raw size:         " << std::setw(12) << raw_size / 1024 / 1024 << " MB                         │\n";
    std::cout << "│ Compressed size:  " << std::setw(12) << compressed_size / 1024 / 1024 << " MB                         │\n";
    std::cout << "│ Compression:      " << std::setw(12) << std::fixed << std::setprecision(1) << static_cast<double>(raw_size) / compressed_size << "x                          │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n\n";
    
    // Range query benchmark
    auto& aapl = db.get_series("AAPL");
    tsdb::Timestamp query_start = aapl.first_time() + (aapl.last_time() - aapl.first_time()) / 2;
    tsdb::Timestamp query_end = query_start + 1'000'000'000LL;  // 1 second range
    
    start = std::chrono::high_resolution_clock::now();
    auto results = aapl.range(query_start, query_end);
    end = std::chrono::high_resolution_clock::now();
    auto query_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    std::cout << "┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ RANGE QUERY PERFORMANCE                                     │\n";
    std::cout << "├─────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Query range:      1 second window                           │\n";
    std::cout << "│ Points returned:  " << std::setw(12) << results.size() << "                          │\n";
    std::cout << "│ Query time:       " << std::setw(12) << std::fixed << std::setprecision(3) << query_ns / 1000.0 << " µs                        │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n\n";
    
    // Aggregation benchmark
    start = std::chrono::high_resolution_clock::now();
    auto agg = aapl.aggregate(aapl.first_time(), aapl.last_time());
    end = std::chrono::high_resolution_clock::now();
    auto agg_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "┌─────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ AGGREGATION PERFORMANCE (full series)                       │\n";
    std::cout << "├─────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ Points scanned:   " << std::setw(12) << agg.count << "                          │\n";
    std::cout << "│ Sum:              " << std::setw(12) << std::scientific << std::setprecision(4) << agg.sum << "                          │\n";
    std::cout << "│ Avg:              " << std::setw(12) << std::fixed << std::setprecision(4) << agg.avg << "                          │\n";
    std::cout << "│ Min:              " << std::setw(12) << std::fixed << std::setprecision(4) << agg.min << "                          │\n";
    std::cout << "│ Max:              " << std::setw(12) << std::fixed << std::setprecision(4) << agg.max << "                          │\n";
    std::cout << "│ Query time:       " << std::setw(12) << std::fixed << std::setprecision(2) << agg_us / 1000.0 << " ms                        │\n";
    std::cout << "└─────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "Demo complete.\n";
    
    return 0;
}