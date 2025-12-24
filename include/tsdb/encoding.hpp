#ifndef TSDB_ENCODING_HPP
#define TSDB_ENCODING_HPP

#include <vector>
#include <cstdint>
#include <tsdb/types.hpp>

namespace tsdb {

class DeltaEncoder {
public:
    static std::pair<Timestamp, std::vector<int64_t>> encode(const std::vector<Timestamp>& timestamps);
    static std::vector<Timestamp> decode(Timestamp base, const std::vector<int64_t>& deltas);
    static std::size_t encoded_size_bytes(const std::vector<Timestamp>& timestamps);
};

}  // namespace tsdb

#endif