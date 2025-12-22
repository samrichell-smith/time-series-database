#include <tsdb/series.hpp>

namespace tsdb {

  void Series::append(Timestamp time, Value value) {
    timestamps_.push_back(time);
    values_.push_back(value);
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

} // namespace tsdb