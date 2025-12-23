#ifndef TSDB_DATABASE_HPP
#define TSDB_DATABASE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <tsdb/series.hpp>

namespace tsdb {

class Database {
public:
    void create_series(const std::string& name);
    void drop_series(const std::string& name);
    bool has_series(const std::string& name) const;
    std::vector<std::string> list_series() const;
    
    Series& get_series(const std::string& name);
    const Series& get_series(const std::string& name) const;
    
    void append(const std::string& name, Timestamp time, Value value);
    std::vector<Point> range(const std::string& name, Timestamp start, Timestamp end) const;
    AggregateResult aggregate(const std::string& name, Timestamp start, Timestamp end) const;
    
    std::size_t series_count() const;
    std::size_t total_points() const;
    std::size_t total_memory_bytes() const;

private:
    std::unordered_map<std::string, Series> series_;
};

}  // namespace tsdb

#endif