#include <tsdb/types.hpp>
#include <tsdb/series.hpp>
#include <iostream>

int main() {
    tsdb::Series s;
    
    s.append(1000, 10.0);
    s.append(2000, 20.0);
    s.append(3000, 30.0);
    s.append(4000, 40.0);
    s.append(5000, 50.0);
    
    std::cout << "Size: " << s.size() << "\n";
    std::cout << "First time: " << s.first_time() << "\n";
    std::cout << "Last time: " << s.last_time() << "\n";
    
    auto points = s.range(1500, 4500);
    std::cout << "Range query returned " << points.size() << " points\n";
    
    auto agg = s.aggregate(1000, 5000);
    std::cout << "Aggregate: sum=" << agg.sum << " avg=" << agg.avg << " count=" << agg.count << "\n";
    
    return 0;
}