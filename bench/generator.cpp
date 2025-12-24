#include "generator.hpp"
#include <random>
#include <algorithm>

namespace tsdb {

std::vector<Point> generate_tick_data(const GeneratorConfig& config) {
    std::vector<Point> points;
    points.reserve(config.num_points);
    
    std::mt19937_64 rng(config.seed);
    std::normal_distribution<double> interval_dist(
        static_cast<double>(config.avg_interval_ns),
        static_cast<double>(config.avg_interval_ns) * config.interval_jitter
    );
    std::normal_distribution<double> value_dist(0.0, config.volatility);
    
    Timestamp current_time = config.start_time;
    double current_value = config.start_value;
    
    for (std::size_t i = 0; i < config.num_points; i++) {
        points.push_back(Point{current_time, current_value});
        
        int64_t interval = std::max(static_cast<int64_t>(1), static_cast<int64_t>(interval_dist(rng)));
        current_time += interval;
        
        current_value *= (1.0 + value_dist(rng));
    }
    
    return points;
}

}  // namespace tsdb