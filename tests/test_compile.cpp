#include <tsdb/database.hpp>
#include <tsdb/encoding.hpp>
#include <iostream>
#include <random>

int main() {
    tsdb::Database db;
    db.create_series("AAPL");
    
    // Generate realistic tick data
    std::mt19937_64 rng(42);
    std::normal_distribution<double> interval_dist(1'000'000, 100'000);  // ~1ms intervals
    std::normal_distribution<double> value_dist(0.0, 0.001);
    
    tsdb::Timestamp time = 1'000'000'000'000'000'000LL;
    double value = 150.0;
    
    for (int i = 0; i < 10'000'000; i++) {
        db.append("AAPL", time, value);
        time += static_cast<int64_t>(std::max(1.0, interval_dist(rng)));
        value *= (1.0 + value_dist(rng));
    }
    
    auto& series = db.get_series("AAPL");
    
    std::size_t raw_size = series.memory_usage_bytes();
    std::size_t compressed_size = series.compressed_size_bytes();
    
    std::cout << "Points: " << series.size() << "\n";
    std::cout << "Raw size: " << raw_size / 1024 / 1024 << " MB\n";
    std::cout << "Compressed size: " << compressed_size / 1024 / 1024 << " MB\n";
    std::cout << "Compression ratio: " << static_cast<double>(raw_size) / compressed_size << "x\n";
    
    // Query performance check
    auto points = series.range(series.first_time(), series.first_time() + 1'000'000'000LL);
    std::cout << "Range query returned: " << points.size() << " points\n";
    
    auto agg = series.aggregate(series.first_time(), series.last_time());
    std::cout << "Average value: " << agg.avg << "\n";
    
    return 0;
}