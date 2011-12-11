#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_fixed_generators.h"

void test_real_generators_fixed() {
    test_start(L"Fixed-length real-valued generators");

    auto g1 = random_real(std::integral_constant<int, 5>(), 0.0, 1.0)(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();
    assert_all(g1l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 1.0; });
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](float i) { return 0.0 <= i && i <= 1.0; }); });
    assert_all(g1l, [](const Indiv& x) { return std::any_of(begin(x.genome), end(x.genome), [](float i) { return 0.0 < i && i < 1.0; }); });

    g1 = real_low(std::integral_constant<int, 5>(), 0.0, 1.0)(100);
    g1l = g1;
    assert_all(g1l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 1.0; });
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](float i) { return 0.0 <= i && i <= 1.0; }); });
    assert_all(g1l, [](const Indiv& x) { return std::any_of(begin(x.genome), end(x.genome), [](float i) { return i == 0.0; }); });

    g1 = real_high(std::integral_constant<int, 5>(), 0.0, 1.0)(100);
    g1l = g1;
    assert_all(g1l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 1.0; });
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](float i) { return i == 1.0; }); });

    g1 = real_mid(std::integral_constant<int, 5>(), -1.0, 1.0)(100);
    g1l = g1;
    assert_all(g1l, [](const Indiv& x) { return x.lowest == -1.0 && x.highest == 1.0; });
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](float i) { return i == 0.0; }); });

    test_pass();
}