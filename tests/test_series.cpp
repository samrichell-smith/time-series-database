#include <gtest/gtest.h>
#include <tsdb/series.hpp>

TEST(SeriesTest, EmptyOnConstruction) {
    tsdb::Series s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
}

TEST(SeriesTest, AppendIncreasesSize) {
    tsdb::Series s;
    s.append(1000, 10.0);
    EXPECT_EQ(s.size(), 1);
    EXPECT_FALSE(s.empty());
    
    s.append(2000, 20.0);
    EXPECT_EQ(s.size(), 2);
}

TEST(SeriesTest, FirstAndLastTime) {
    tsdb::Series s;
    s.append(1000, 10.0);
    s.append(2000, 20.0);
    s.append(3000, 30.0);
    
    EXPECT_EQ(s.first_time(), 1000);
    EXPECT_EQ(s.last_time(), 3000);
}

TEST(SeriesTest, AppendBatch) {
    tsdb::Series s;
    std::vector<tsdb::Point> points = {
        {1000, 10.0},
        {2000, 20.0},
        {3000, 30.0}
    };
    s.append_batch(points);
    
    EXPECT_EQ(s.size(), 3);
    EXPECT_EQ(s.first_time(), 1000);
    EXPECT_EQ(s.last_time(), 3000);
}

TEST(SeriesTest, RangeQueryAll) {
    tsdb::Series s;
    s.append(1000, 10.0);
    s.append(2000, 20.0);
    s.append(3000, 30.0);
    
    auto results = s.range(0, 5000);
    EXPECT_EQ(results.size(), 3);
}

TEST(SeriesTest, RangeQuerySubset) {
    tsdb::Series s;
    s.append(1000, 10.0);
    s.append(2000, 20.0);
    s.append(3000, 30.0);
    s.append(4000, 40.0);
    s.append(5000, 50.0);
    
    auto results = s.range(1500, 3500);
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(results[0].time, 2000);
    EXPECT_EQ(results[1].time, 3000);
}

TEST(SeriesTest, RangeQueryEmpty) {
    tsdb::Series s;
    s.append(1000, 10.0);
    s.append(2000, 20.0);
    
    auto results = s.range(5000, 6000);
    EXPECT_EQ(results.size(), 0);
}

TEST(SeriesTest, RangeQueryInclusive) {
    tsdb::Series s;
    s.append(1000, 10.0);
    s.append(2000, 20.0);
    s.append(3000, 30.0);
    
    auto results = s.range(1000, 3000);
    EXPECT_EQ(results.size(), 3);
}

TEST(SeriesTest, AggregateBasic) {
    tsdb::Series s;
    s.append(1000, 10.0);
    s.append(2000, 20.0);
    s.append(3000, 30.0);
    s.append(4000, 40.0);
    
    auto agg = s.aggregate(1000, 4000);
    EXPECT_EQ(agg.count, 4);
    EXPECT_DOUBLE_EQ(agg.sum, 100.0);
    EXPECT_DOUBLE_EQ(agg.avg, 25.0);
    EXPECT_DOUBLE_EQ(agg.min, 10.0);
    EXPECT_DOUBLE_EQ(agg.max, 40.0);
}

TEST(SeriesTest, AggregateSubset) {
    tsdb::Series s;
    s.append(1000, 10.0);
    s.append(2000, 20.0);
    s.append(3000, 30.0);
    s.append(4000, 40.0);
    
    auto agg = s.aggregate(1500, 3500);
    EXPECT_EQ(agg.count, 2);
    EXPECT_DOUBLE_EQ(agg.sum, 50.0);
    EXPECT_DOUBLE_EQ(agg.avg, 25.0);
}

TEST(SeriesTest, AggregateEmpty) {
    tsdb::Series s;
    s.append(1000, 10.0);
    
    auto agg = s.aggregate(5000, 6000);
    EXPECT_EQ(agg.count, 0);
}

TEST(SeriesTest, MemoryUsage) {
    tsdb::Series s;
    s.append(1000, 10.0);
    
    EXPECT_GT(s.memory_usage_bytes(), 0);
}

TEST(SeriesTest, AppendOutOfOrderThrows) {
    tsdb::Series s;
    s.append(2000, 20.0);
    
    EXPECT_THROW(s.append(1000, 10.0), std::invalid_argument);
}

TEST(SeriesTest, AppendDuplicateTimestampThrows) {
    tsdb::Series s;
    s.append(1000, 10.0);
    
    EXPECT_THROW(s.append(1000, 20.0), std::invalid_argument);
}

TEST(SeriesTest, FirstTimeEmptyThrows) {
    tsdb::Series s;
    
    EXPECT_THROW(s.first_time(), std::runtime_error);
}

TEST(SeriesTest, LastTimeEmptyThrows) {
    tsdb::Series s;
    
    EXPECT_THROW(s.last_time(), std::runtime_error);
}