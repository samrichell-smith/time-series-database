#ifndef TSDB_TYPES_HPP
#define TSDB_TYPES_HPP

#include <cstdint>
#include <cstddef>

namespace tsdb {

  using Timestamp = int64_t; 
  using Value = double; 

  struct Point {
    Timestamp time;
    Value value;
  };

  struct AggregateResult {
    double sum;
    double min;
    double max;
    double avg;
    size_t count;
  };

} // namespace tsdb


#endif