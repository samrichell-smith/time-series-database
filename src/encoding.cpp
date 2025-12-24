#include <tsdb/encoding.hpp>
#include <utility>

namespace tsdb {

std::pair<Timestamp, std::vector<int64_t>> DeltaEncoder::encode(const std::vector<Timestamp>& timestamps) {
    if (timestamps.empty()) {
        return {0, {}};
    }
    
    Timestamp base = timestamps[0];
    std::vector<int64_t> deltas;
    deltas.reserve(timestamps.size() - 1);
    
    for (std::size_t i = 1; i < timestamps.size(); i++) {
        deltas.push_back(timestamps[i] - timestamps[i - 1]);
    }
    
    return {base, deltas};
}

std::vector<Timestamp> DeltaEncoder::decode(Timestamp base, const std::vector<int64_t>& deltas) {
    std::vector<Timestamp> timestamps;
    timestamps.reserve(deltas.size() + 1);
    timestamps.push_back(base);
    
    for (int64_t delta : deltas) {
        timestamps.push_back(timestamps.back() + delta);
    }
    
    return timestamps;
}

std::size_t DeltaEncoder::encoded_size_bytes(const std::vector<Timestamp>& timestamps) {
    if (timestamps.empty()) {
        return 0;
    }
    
    std::size_t bytes = sizeof(Timestamp);  // base timestamp
    
    Timestamp prev = timestamps[0];
    for (std::size_t i = 1; i < timestamps.size(); i++) {
        int64_t delta = timestamps[i] - prev;
        prev = timestamps[i];
        
        // Variable-length encoding: count bytes needed
        if (delta == 0) {
            bytes += 1;
        } else {
            int64_t abs_delta = delta < 0 ? -delta : delta;
            if (abs_delta < 128) bytes += 1;
            else if (abs_delta < 32768) bytes += 2;
            else if (abs_delta < 8388608) bytes += 3;
            else if (abs_delta < 2147483648) bytes += 4;
            else bytes += 8;
        }
    }
    
    return bytes;
}

}  // namespace tsdb