#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "operators\mutate_random.h"

void test_mutate_random_real() {
    test_start(L"Mutate random on real individuals");
    
    auto g1 = random_real(std::integral_constant<int, 5>(), 0.0, 1.0)(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = mutate_random(esdl::merge(g1), 1.0f, 0.5f, 0)();
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 1.0; });
    assert_all(g2l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](float i) { return 0.0 <= i && i <= 1.0; }); });
    assert_all(g2l, [](const Indiv& x) { return std::any_of(begin(x.genome), end(x.genome), [](float i) { return 0.0 < i && i < 1.0; }); });

    test_pass();
}

void test_mutate_random_int() {
    test_start(L"Mutate random on integer individuals");

    auto g1 = random_integer(std::integral_constant<int, 10>(), -100, 100)(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = mutate_random(esdl::merge(g1), 1.0f, 0.5f, 0)();
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return x.lowest == -100 && x.highest == 100; });
    assert_all(g2l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return -100 <= i && i <= 100; }); });
    assert_all(g2l, [](const Indiv& x) { return std::any_of(begin(x.genome), end(x.genome), [](int i) { return -100 < i && i < 100; }); });

    test_pass();
}
