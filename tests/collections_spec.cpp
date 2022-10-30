#include "collections_spec.h"

#include "../src/utility/collections.h"

void wall_e::collections_spec::vec_bench(const testing::benchmark_ctx &ctx) {

    const wall_e::vec<std::size_t> input = []{
        wall_e::vec<std::size_t> r;
        r.reserve(100000);
        for(std::size_t i = 0; i < 100000; ++i) { r.push_back(i); }
        return r;
    }();

    ctx.each("for", [&input](){
        wall_e::vec<std::size_t> output;
        output.resize(input.size());
        std::size_t i = 0;
        for(const auto& item : input) {
             output[i] = (item * 2);
             ++i;
        }
    });

    ctx.each("map", [&input](){
        auto output = input.map<std::size_t>([](auto i) { return i * 2; });

    });
}

void wall_e::collections_spec::lst_bench(const testing::benchmark_ctx &ctx) {
    const wall_e::list<std::size_t> input = []{
        wall_e::list<std::size_t> r;
        for(std::size_t i = 0; i < 100000; ++i) { r.push_back(i); }
        return r;
    }();

    ctx.each("for", [&input](){
        wall_e::list<std::size_t> output;
        std::size_t i = 0;
        for(const auto& item : input) {
             output.push_back(item * 2);
        }
    });

    ctx.each("map", [&input](){
        auto output = input.map<std::size_t>([](auto i) { return i * 2; });

    });

}
