#include <tsdb/database.hpp>
#include <iostream>

int main() {
    tsdb::Database db;
    
    db.create_series("AAPL");
    db.create_series("GOOG");
    
    db.append("AAPL", 1000, 150.0);
    db.append("AAPL", 2000, 151.0);
    db.append("AAPL", 3000, 149.0);
    
    db.append("GOOG", 1000, 2800.0);
    db.append("GOOG", 2000, 2810.0);
    
    std::cout << "Series count: " << db.series_count() << "\n";
    std::cout << "Total points: " << db.total_points() << "\n";
    
    auto points = db.range("AAPL", 1000, 2500);
    std::cout << "AAPL range query: " << points.size() << " points\n";
    
    auto agg = db.aggregate("AAPL", 1000, 3000);
    std::cout << "AAPL aggregate: avg=" << agg.avg << "\n";
    
    std::cout << "Has AAPL: " << db.has_series("AAPL") << "\n";
    std::cout << "Has MSFT: " << db.has_series("MSFT") << "\n";
    
    return 0;
}