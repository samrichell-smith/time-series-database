#include <benchmark/benchmark.h>
#include <tsdb/series.hpp>
#include "naive_series.hpp"
#include "generator.hpp"

static std::vector<tsdb::Point> test_data;
static tsdb::Timestamp query_start;
static tsdb::Timestamp query_end;

static void ensure_test_data() {
    if (test_data.empty()) {
        tsdb::GeneratorConfig config;
        config.num_points = 10'000'000;
        test_data = tsdb::generate_tick_data(config);
        
        std::size_t start_idx = test_data.size() * 0.495;
        std::size_t end_idx = test_data.size() * 0.505;
        query_start = test_data[start_idx].time;
        query_end = test_data[end_idx].time;
    }
}

static void BM_Ingestion_Optimised(benchmark::State& state) {
    for (auto _ : state) {
        tsdb::Series series;
        for (const auto& p : test_data) {
            series.append(p.time, p.value);
        }
        benchmark::DoNotOptimize(series);
    }
    state.SetItemsProcessed(state.iterations() * test_data.size());
}

static void BM_Ingestion_Naive(benchmark::State& state) {
    for (auto _ : state) {
        naive::Series series;
        for (const auto& p : test_data) {
            series.append(p.time, p.value);
        }
        benchmark::DoNotOptimize(series);
    }
    state.SetItemsProcessed(state.iterations() * test_data.size());
}

static void BM_RangeQuery_Optimised(benchmark::State& state) {
    ensure_test_data();
    
    tsdb::Series series;
    series.append_batch(test_data);
    
    for (auto _ : state) {
        auto result = series.range(query_start, query_end);
        benchmark::DoNotOptimize(result);
    }
}

static void BM_RangeQuery_Naive(benchmark::State& state) {
    ensure_test_data();
    
    naive::Series series;
    series.append_batch(test_data);
    
    for (auto _ : state) {
        auto result = series.range(query_start, query_end);
        benchmark::DoNotOptimize(result);
    }
}

static void BM_Aggregate_Optimised(benchmark::State& state) {
    ensure_test_data();
    
    tsdb::Series series;
    series.append_batch(test_data);
    
    for (auto _ : state) {
        auto result = series.aggregate(query_start, query_end);
        benchmark::DoNotOptimize(result);
    }
}

static void BM_Aggregate_Naive(benchmark::State& state) {
    ensure_test_data();
    
    naive::Series series;
    series.append_batch(test_data);
    
    for (auto _ : state) {
        auto result = series.aggregate(query_start, query_end);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_Ingestion_Optimised);
BENCHMARK(BM_Ingestion_Naive);
BENCHMARK(BM_RangeQuery_Optimised);
BENCHMARK(BM_RangeQuery_Naive);
BENCHMARK(BM_Aggregate_Optimised);
BENCHMARK(BM_Aggregate_Naive);

BENCHMARK_MAIN();