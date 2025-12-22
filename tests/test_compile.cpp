#include <tsdb/types.hpp>

int main() {
    tsdb::Timestamp t = 1000;
    tsdb::Value v = 99.5;
    tsdb::Point p{t, v};
    tsdb::AggregateResult r{100.0, 1.0, 10.0, 5.0, 20};
    return 0;
}