#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "operators\crossover.h"

void test_crossover_one_one() {
    test_start(L"crossover_one (one_child)");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    auto s2 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    decltype(s1) _merge[] = { s1, s2 };

    auto g1 = crossover_one(esdl::merge(_merge), 1.0f, true, false)();
    _assert(g1.size() == 1);
    auto indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f || x == 1.0f; });
    _assert(std::any_of(begin(indiv.genome), end(indiv.genome), [](float x) { return x != -1.0f; }));
    _assert(std::any_of(begin(indiv.genome), end(indiv.genome), [](float x) { return x = -1.0f; }));

    test_pass();
}

void test_crossover_one_two() {
    test_start(L"crossover_one (two_children)");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    auto s2 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    decltype(s1) _merge[] = { s1, s2 };

    auto g1 = crossover_one(esdl::merge(_merge), 1.0f, true, true)();
    _assert(g1.size() == 2);
    auto indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f || x == 1.0f; });
    _assert(std::any_of(begin(indiv.genome), end(indiv.genome), [](float x) { return x != -1.0f; }));
    _assert(std::any_of(begin(indiv.genome), end(indiv.genome), [](float x) { return x = -1.0f; }));

    test_pass();
}

void test_crossover() {
    test_crossover_one_one();
    test_crossover_one_two();
}
