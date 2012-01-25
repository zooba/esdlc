#include "stdafx.h"


#include "individuals\bin_fixed.h"
#include "individuals\bin_fixed_generators.h"

void test_bin_generators_fixed() {
    test_start(L"Fixed=length binary generators");

    auto g1 = random_binary(std::integral_constant<int, 10>(), 0.5f)(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 0 || i == 1; }); });
    
    int distribution[2] = {0};
    std::for_each(begin(g1l), end(g1l), [&](const Indiv& x) {
        std::for_each(begin(x.genome), end(x.genome), [&](int i) {
            distribution[i] += 1;
        });
    });
    
    _assert(450 <= distribution[0] && distribution[0] <= 550);
    _assert(450 <= distribution[1] && distribution[1] <= 550);

    g1 = binary_true(std::integral_constant<int, 10>())(100);
    g1l = g1;
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 1; }); });

    g1 = binary_false(std::integral_constant<int, 10>())(100);
    g1l = g1;
    assert_all(g1l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 0; }); });

    test_pass();
}
