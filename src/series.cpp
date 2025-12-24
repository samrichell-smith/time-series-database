#include <tsdb/series.hpp>
#include <algorithm>
#include <limits>
#include <tsdb/encoding.hpp>

namespace tsdb {

void Series::append(Timestamp time, Value value) {
    timestamps_.push_back(time);
    values_.push_back(value);
}

void Series::append_batch(const std::vector<Point>& points) {
    timestamps_.reserve(timestamps_.size() + points.size());
    values_.reserve(values_.size() + points.size());
    
    for (const auto& p : points) {
        timestamps_.push_back(p.time);
        values_.push_back(p.value);
    }
}

std::size_t Series::compressed_size_bytes() const {
    std::size_t timestamp_bytes = DeltaEncoder::encoded_size_bytes(timestamps_);
    std::size_t value_bytes = values_.size() * sizeof(Value);  // values not compressed
    return timestamp_bytes + value_bytes;
}

std::vector<Point> Series::range(Timestamp start, Timestamp end) const {
    auto start_it = std::lower_bound(timestamps_.begin(), timestamps_.end(), start);
    auto end_it = std::upper_bound(timestamps_.begin(), timestamps_.end(), end);
    
    std::size_t start_idx = start_it - timestamps_.begin();
    std::size_t end_idx = end_it - timestamps_.begin();
    
    std::vector<Point> result;
    result.reserve(end_idx - start_idx);
    
    for (std::size_t i = start_idx; i < end_idx; i++) {
        result.push_back(Point{timestamps_[i], values_[i]});
    }
    
    return result;
}

AggregateResult Series::aggregate(Timestamp start, Timestamp end) const {
    auto start_it = std::lower_bound(timestamps_.begin(), timestamps_.end(), start);
    auto end_it = std::upper_bound(timestamps_.begin(), timestamps_.end(), end);
    
    std::size_t start_idx = start_it - timestamps_.begin();
    std::size_t end_idx = end_it - timestamps_.begin();
    
    if (start_idx >= end_idx) {
        return AggregateResult{0.0, 0.0, 0.0, 0.0, 0};
    }
    
    double sum = 0.0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    
    for (std::size_t i = start_idx; i < end_idx; i++) {
        sum += values_[i];
        if (values_[i] < min) min = values_[i];
        if (values_[i] > max) max = values_[i];
    }
    
    std::size_t count = end_idx - start_idx;
    double avg = sum / static_cast<double>(count);
    
    return AggregateResult{sum, min, max, avg, count};
}

std::size_t Series::size() const {
    return timestamps_.size();
}

bool Series::empty() const {
    return timestamps_.empty();
}

Timestamp Series::first_time() const {
    return timestamps_.front();
}

Timestamp Series::last_time() const {
    return timestamps_.back();
}

std::size_t Series::memory_usage_bytes() const {
    return timestamps_.capacity() * sizeof(Timestamp) + 
           values_.capacity() * sizeof(Value);
}

}  // namespace tsdb