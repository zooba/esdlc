#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "operators\mutate_delta.h"

void test_mutate_delta_real() {
    test_start(L"Mutate delta on real individuals");
    
    auto g1 = real_mid(std::integral_constant<int, 5>(), -1.0, 1.0)(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = mutate_delta(esdl::merge(g1), 1.0f, 0.0f, 1.0f, 0.5f, 0)();
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return x.lowest == -1.0 && x.highest == 1.0; });

    test_pass();
}

void test_mutate_delta_int() {
    test_start(L"Mutate delta on integer individuals");

    auto g1 = integer_mid(std::integral_constant<int, 10>(), -100, 100)(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = mutate_delta(esdl::merge(g1), 1, 0.5, 1.0f, 0.5f, 0)();
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return x.lowest == -100 && x.highest == 100; });

    test_pass();
}
