#include <tsdb/database.hpp>
#include <stdexcept>

namespace tsdb {

void Database::create_series(const std::string& name) {
    if (series_.find(name) != series_.end()) {
        throw std::runtime_error("Series already exists: " + name);
    }
    series_[name] = Series{};
}

void Database::drop_series(const std::string& name) {
    auto it = series_.find(name);
    if (it == series_.end()) {
        throw std::runtime_error("Series not found: " + name);
    }
    series_.erase(it);
}

bool Database::has_series(const std::string& name) const {
    return series_.find(name) != series_.end();
}

std::vector<std::string> Database::list_series() const {
    std::vector<std::string> names;
    names.reserve(series_.size());
    for (const auto& pair : series_) {
        names.push_back(pair.first);
    }
    return names;
}

Series& Database::get_series(const std::string& name) {
    auto it = series_.find(name);
    if (it == series_.end()) {
        throw std::runtime_error("Series not found: " + name);
    }
    return it->second;
}

const Series& Database::get_series(const std::string& name) const {
    auto it = series_.find(name);
    if (it == series_.end()) {
        throw std::runtime_error("Series not found: " + name);
    }
    return it->second;
}

void Database::append(const std::string& name, Timestamp time, Value value) {
    get_series(name).append(time, value);
}

std::vector<Point> Database::range(const std::string& name, Timestamp start, Timestamp end) const {
    return get_series(name).range(start, end);
}

AggregateResult Database::aggregate(const std::string& name, Timestamp start, Timestamp end) const {
    return get_series(name).aggregate(start, end);
}

std::size_t Database::series_count() const {
    return series_.size();
}

std::size_t Database::total_points() const {
    std::size_t total = 0;
    for (const auto& pair : series_) {
        total += pair.second.size();
    }
    return total;
}

std::size_t Database::total_memory_bytes() const {
    std::size_t total = 0;
    for (const auto& pair : series_) {
        total += pair.second.memory_usage_bytes();
    }
    return total;
}

}  // namespace tsdb