#include "collections_spec.h"

#include "../src/utility/collections.h"
#include "../src/utility/box.h"
#include <iostream>
#include <wall_e/src/utility/defer.h>

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

void wall_e::collections_spec::box_test() {

    bool deleted = false;
    {
        const auto box = wall_e::box<defer>::make([&deleted](){
            deleted = true;
        });
        wall_e_should_equal(deleted, false);
    }
    wall_e_should_equal(deleted, true);

}

void wall_e::collections_spec::box_list_test() {


    bool deleted0 = false;
    bool deleted1 = false;

    {
        wall_e::box_list<defer> bl;
        {
            bl.make([&deleted0](){
                deleted0 = true;
            });
            wall_e_should_equal(deleted0, false);
        }
        wall_e_should_equal(deleted0, false);

        bl.make([&deleted1](){
            deleted1 = true;
        });
        wall_e_should_equal(deleted1, false);
    }

    wall_e_should_equal(deleted0, true);
    wall_e_should_equal(deleted1, true);


}

void wall_e::collections_spec::defer_test() {
    {
        bool defered = false;
        {
            wall_e::defer d([&defered]{
                defered = true;
            });
            wall_e_should_equal(defered, false)
        }
        wall_e_should_equal(defered, true)
    } {
        bool defered = false;
        {
            wall_e::defer([&defered]{
                defered = true;
            });
            wall_e_should_equal(defered, true)
        }
        wall_e_should_equal(defered, true)
    }
}
