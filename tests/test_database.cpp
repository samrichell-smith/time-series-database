#include <gtest/gtest.h>
#include <tsdb/database.hpp>

TEST(DatabaseTest, EmptyOnConstruction) {
    tsdb::Database db;
    EXPECT_EQ(db.series_count(), 0);
    EXPECT_EQ(db.total_points(), 0);
}

TEST(DatabaseTest, CreateSeries) {
    tsdb::Database db;
    db.create_series("AAPL");
    
    EXPECT_EQ(db.series_count(), 1);
    EXPECT_TRUE(db.has_series("AAPL"));
    EXPECT_FALSE(db.has_series("GOOG"));
}

TEST(DatabaseTest, CreateDuplicateThrows) {
    tsdb::Database db;
    db.create_series("AAPL");
    
    EXPECT_THROW(db.create_series("AAPL"), std::runtime_error);
}

TEST(DatabaseTest, DropSeries) {
    tsdb::Database db;
    db.create_series("AAPL");
    db.create_series("GOOG");
    
    db.drop_series("AAPL");
    
    EXPECT_EQ(db.series_count(), 1);
    EXPECT_FALSE(db.has_series("AAPL"));
    EXPECT_TRUE(db.has_series("GOOG"));
}

TEST(DatabaseTest, DropNonexistentThrows) {
    tsdb::Database db;
    
    EXPECT_THROW(db.drop_series("AAPL"), std::runtime_error);
}

TEST(DatabaseTest, GetSeriesNonexistentThrows) {
    tsdb::Database db;
    
    EXPECT_THROW(db.get_series("AAPL"), std::runtime_error);
}

TEST(DatabaseTest, ListSeries) {
    tsdb::Database db;
    db.create_series("AAPL");
    db.create_series("GOOG");
    db.create_series("MSFT");
    
    auto names = db.list_series();
    EXPECT_EQ(names.size(), 3);
}

TEST(DatabaseTest, AppendAndQuery) {
    tsdb::Database db;
    db.create_series("AAPL");
    
    db.append("AAPL", 1000, 150.0);
    db.append("AAPL", 2000, 151.0);
    db.append("AAPL", 3000, 149.0);
    
    EXPECT_EQ(db.total_points(), 3);
    
    auto points = db.range("AAPL", 1000, 3000);
    EXPECT_EQ(points.size(), 3);
}

TEST(DatabaseTest, Aggregate) {
    tsdb::Database db;
    db.create_series("AAPL");
    
    db.append("AAPL", 1000, 10.0);
    db.append("AAPL", 2000, 20.0);
    db.append("AAPL", 3000, 30.0);
    
    auto agg = db.aggregate("AAPL", 1000, 3000);
    EXPECT_EQ(agg.count, 3);
    EXPECT_DOUBLE_EQ(agg.avg, 20.0);
}

TEST(DatabaseTest, MultipleSeries) {
    tsdb::Database db;
    db.create_series("AAPL");
    db.create_series("GOOG");
    
    db.append("AAPL", 1000, 150.0);
    db.append("AAPL", 2000, 151.0);
    db.append("GOOG", 1000, 2800.0);
    
    EXPECT_EQ(db.total_points(), 3);
    EXPECT_EQ(db.get_series("AAPL").size(), 2);
    EXPECT_EQ(db.get_series("GOOG").size(), 1);
}