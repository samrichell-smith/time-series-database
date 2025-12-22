#ifndef TSDB_SERIES_HPP
#define TSDB_SERIES_HPP

#include <vector>
#include <tsdb/types.hpp>

namespace tsdb {

  class Series {

    public:
      void append(Timestamp time, Value value);
      std::size_t size() const;
      bool empty() const;
      Timestamp first_time() const;
      Timestamp last_time() const;

    private:
      std::vector<Timestamp> timestamps_;
      std::vector<Value> values_;

  };

} // namespace tsdb

#endif