#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "joiners\tuples.h"
#include "operators\crossover_tuple.h"

void test_crossover_tuple_2() {
    test_start(L"crossover_tuple on pairs");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    auto s2 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);

    auto g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2))(1)), 0.0f, 0.5f)();
    _assert(g1.size() == 1);
    auto indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f; });

    g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2))(1)), 1.0f, 1.0f)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f; });

    g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2))(1)), 0.5f, 1.0f)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    _assert(std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == -1.0f; }) ||
        std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == 1.0f; }));

    g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2))(1)), 1.0f, 0.0f)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == 1.0f; });

    g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2))(1)), 1.0f, 0.5f)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f || x == 1.0f; });

    test_pass();
}

void test_crossover_tuple_3() {
    test_start(L"crossover_tuple on triplets");
    test_notimpl();

    /*auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    auto s2 = real_mid(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    auto s3 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);

    auto g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))(1)), 0.0f, 0.5f)();
    _assert(g1.size() == 1);
    auto indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f; });

    g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))(1)), 1.0f, 1.0f)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == 1.0f; });

    g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))(1)), 0.5f, 1.0f)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    _assert(std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == -1.0f; }) ||
        std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == 1.0f; }));

    g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))(1)), 1.0f, 0.0f)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f; });

    g1 = crossover_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))(1)), 1.0f, 0.5f)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f || x == 1.0f; });

    test_pass();*/
}


