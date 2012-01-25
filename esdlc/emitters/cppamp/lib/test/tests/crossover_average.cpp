#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "operators\crossover_average.h"

void test_crossover_average() {
    test_start(L"crossover_average");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    auto s2 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(1);
    decltype(s1) _merge[] = { s1, s2 };

    auto g1 = crossover_average(esdl::merge(_merge), 0.0f, 0.0f, 0)();
    _assert(g1.size() == 1);
    auto indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f; });

    g1 = crossover_average(esdl::merge(_merge), 1.0f, 1.0f, 0)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == 0.0f; });

    g1 = crossover_average(esdl::merge(_merge), 0.5f, 1.0f, 0)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    _assert(std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == -1.0f; }) ||
        std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == 0.0f; }));

    g1 = crossover_average(esdl::merge(_merge), 1.0f, 0.0f, 0)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f; });

    g1 = crossover_average(esdl::merge(_merge), 1.0f, 0.5f, 0)();
    _assert(g1.size() == 1);
    indiv = g1.as_list().front();
    assert_all(indiv.genome, [](float x) { return x == -1.0f || x == 0.0f; });

    test_pass();
}

