#include "stdafx.h"


#include "individuals\bin_fixed.h"
#include "individuals\bin_variable.h"

#include "individuals\bin_fixed_generators.h"

#include "operators\merge.h"

#include "operators\mutate_inversion.h"

void test_mutate_inversion() {
    test_start(L"Mutate inversion on binary individuals");
    
    auto g1 = binary_false(std::integral_constant<int, 10>())(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = mutate_inversion(esdl::merge(g1), 1.0f)();
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 1; }); });

    g2 = mutate_inversion(esdl::merge(g1), 0.0f)();
    g2l = g2;
    assert_all(g2l, [](const Indiv& x) { return std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 0; }); });

    g2 = mutate_inversion(esdl::merge(g1), 0.5f)();
    g2l = g2;
    assert_all(g2l, [](const Indiv& x) { 
        return std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 1; }) || 
            std::all_of(begin(x.genome), end(x.genome), [](int i) { return i == 0; });
    });

    test_pass();
}
