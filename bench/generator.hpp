#ifndef TSDB_BENCH_GENERATOR_HPP
#define TSDB_BENCH_GENERATOR_HPP

#include <vector>
#include <tsdb/types.hpp>

namespace tsdb {

struct GeneratorConfig {
    std::size_t num_points = 1'000'000;
    Timestamp start_time = 1'000'000'000'000'000'000LL;
    int64_t avg_interval_ns = 1'000'000;
    double interval_jitter = 0.1;
    double start_value = 100.0;
    double volatility = 0.001;
    unsigned int seed = 42;
};

std::vector<Point> generate_tick_data(const GeneratorConfig& config);

}  // namespace tsdb

#endif