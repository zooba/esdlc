#include "stdafx.h"


#include "individuals\int_fixed.h"
#include "individuals\int_fixed_generators.h"

void test_int_generators_fixed() {
    test_start(L"Fixed=length integer generators");

    auto g1 = random_integer(std::integral_constant<int, 10>(), -100, 100)(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();
    assert_all(g1l, [](const Indiv& x) { return x.lowest == -100 && x.highest == 100; });
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return -100 <= i && i <= 100; }); });
    assert_all(g1l, [](const Indiv& x) { return std::any_of(begin(x.genome), end(x.genome), [](int i) { return -100 < i && i < 100; }); });

    g1 = integer_low(std::integral_constant<int, 10>(), -100, 100)(100);
    g1l = g1;
    assert_all(g1l, [](const Indiv& x) { return x.lowest == -100 && x.highest == 100; });
    assert_all(g1l, [](const Indiv& x) { return std::any_of(begin(x.genome), end(x.genome), [](int i) { return i == -100; }); });

    g1 = integer_high(std::integral_constant<int, 10>(), -100, 100)(100);
    g1l = g1;
    assert_all(g1l, [](const Indiv& x) { return x.lowest == -100 && x.highest == 100; });
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 100; }); });

    g1 = integer_mid(std::integral_constant<int, 10>(), -100, 100)(100);
    g1l = g1;
    assert_all(g1l, [](const Indiv& x) { return x.lowest == -100 && x.highest == 100; });
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 0; }); });

    test_pass();
}
