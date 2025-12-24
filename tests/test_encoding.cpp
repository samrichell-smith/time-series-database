#include <gtest/gtest.h>
#include <tsdb/encoding.hpp>

TEST(EncodingTest, EncodeEmpty) {
    std::vector<tsdb::Timestamp> timestamps;
    auto [base, deltas] = tsdb::DeltaEncoder::encode(timestamps);
    
    EXPECT_EQ(base, 0);
    EXPECT_TRUE(deltas.empty());
}

TEST(EncodingTest, EncodeSingle) {
    std::vector<tsdb::Timestamp> timestamps = {1000};
    auto [base, deltas] = tsdb::DeltaEncoder::encode(timestamps);
    
    EXPECT_EQ(base, 1000);
    EXPECT_TRUE(deltas.empty());
}

TEST(EncodingTest, EncodeMultiple) {
    std::vector<tsdb::Timestamp> timestamps = {1000, 1005, 1010, 1015};
    auto [base, deltas] = tsdb::DeltaEncoder::encode(timestamps);
    
    EXPECT_EQ(base, 1000);
    EXPECT_EQ(deltas.size(), 3);
    EXPECT_EQ(deltas[0], 5);
    EXPECT_EQ(deltas[1], 5);
    EXPECT_EQ(deltas[2], 5);
}

TEST(EncodingTest, EncodeIrregular) {
    std::vector<tsdb::Timestamp> timestamps = {1000, 1002, 1010, 1011};
    auto [base, deltas] = tsdb::DeltaEncoder::encode(timestamps);
    
    EXPECT_EQ(base, 1000);
    EXPECT_EQ(deltas[0], 2);
    EXPECT_EQ(deltas[1], 8);
    EXPECT_EQ(deltas[2], 1);
}

TEST(EncodingTest, DecodeEmpty) {
    std::vector<int64_t> deltas;
    auto timestamps = tsdb::DeltaEncoder::decode(0, deltas);
    
    EXPECT_EQ(timestamps.size(), 1);
    EXPECT_EQ(timestamps[0], 0);
}

TEST(EncodingTest, DecodeMultiple) {
    std::vector<int64_t> deltas = {5, 5, 5};
    auto timestamps = tsdb::DeltaEncoder::decode(1000, deltas);
    
    EXPECT_EQ(timestamps.size(), 4);
    EXPECT_EQ(timestamps[0], 1000);
    EXPECT_EQ(timestamps[1], 1005);
    EXPECT_EQ(timestamps[2], 1010);
    EXPECT_EQ(timestamps[3], 1015);
}

TEST(EncodingTest, RoundTrip) {
    std::vector<tsdb::Timestamp> original = {1000, 1005, 1015, 1030, 1050};
    auto [base, deltas] = tsdb::DeltaEncoder::encode(original);
    auto decoded = tsdb::DeltaEncoder::decode(base, deltas);
    
    EXPECT_EQ(original, decoded);
}

TEST(EncodingTest, EncodedSizeSmaller) {
    std::vector<tsdb::Timestamp> timestamps;
    for (int i = 0; i < 1000; i++) {
        timestamps.push_back(1000000000 + i * 1000);  // Regular 1000ns intervals
    }
    
    std::size_t raw_size = timestamps.size() * sizeof(tsdb::Timestamp);
    std::size_t encoded_size = tsdb::DeltaEncoder::encoded_size_bytes(timestamps);
    
    EXPECT_LT(encoded_size, raw_size);
}