#ifndef TSDB_SERIES_HPP
#define TSDB_SERIES_HPP

#include <vector>
#include <tsdb/types.hpp>

namespace tsdb {

class Series {
public:
    void append(Timestamp time, Value value);
    void append_batch(const std::vector<Point>& points);
    
    std::vector<Point> range(Timestamp start, Timestamp end) const;
    AggregateResult aggregate(Timestamp start, Timestamp end) const;
    
    std::size_t size() const;
    bool empty() const;
    Timestamp first_time() const;
    Timestamp last_time() const;
    
    std::size_t memory_usage_bytes() const;

    std::size_t compressed_size_bytes() const;

private:
    std::vector<Timestamp> timestamps_;
    std::vector<Value> values_;
};

}  // namespace tsdb

#endif