#ifndef TSDB_BENCH_NAIVE_SERIES_HPP
#define TSDB_BENCH_NAIVE_SERIES_HPP

#include <vector>
#include <tsdb/types.hpp>

namespace naive {

class Series {
public:
    void append(tsdb::Timestamp time, tsdb::Value value) {
        data_.push_back(tsdb::Point{time, value});
    }
    
    void append_batch(const std::vector<tsdb::Point>& points) {
        for (const auto& p : points) {
            data_.push_back(p);
        }
    }
    
    std::vector<tsdb::Point> range(tsdb::Timestamp start, tsdb::Timestamp end) const {
        std::vector<tsdb::Point> result;
        for (const auto& p : data_) {
            if (p.time >= start && p.time <= end) {
                result.push_back(p);
            }
        }
        return result;
    }
    
    tsdb::AggregateResult aggregate(tsdb::Timestamp start, tsdb::Timestamp end) const {
        double sum = 0.0;
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::lowest();
        std::size_t count = 0;
        
        for (const auto& p : data_) {
            if (p.time >= start && p.time <= end) {
                sum += p.value;
                if (p.value < min) min = p.value;
                if (p.value > max) max = p.value;
                count++;
            }
        }
        
        double avg = count > 0 ? sum / static_cast<double>(count) : 0.0;
        return tsdb::AggregateResult{sum, min, max, avg, count};
    }
    
    std::size_t size() const { return data_.size(); }
    
    std::size_t memory_usage_bytes() const {
        return data_.capacity() * sizeof(tsdb::Point);
    }

private:
    std::vector<tsdb::Point> data_;
};

}  // namespace naive

#endif