#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "filters\unique.h"

void test_unique() {
    test_start(L"Filter unique individuals");
    
    auto g1 = real_low(std::integral_constant<int, 5>(), -1.0, 1.0)(100);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = unique(esdl::merge(g1))();
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return x.lowest == -1.0 && x.highest == 1.0; });
    _assert(g2l.size() == 1);

    auto g3 = real_high(std::integral_constant<int, 5>(), -1.0, 1.0)(100);
    g3.evaluate_using(std::make_shared<TestEvaluator>());

    decltype(g1) _merge1[] = { g1, g3 };
    g2 = unique(esdl::merge(_merge1))();
    g2l = g2;
    _assert(g2l.size() == 2);

    decltype(real_low(std::integral_constant<int, 5>(), -1.0, 1.0)(7)) _merge2[] = {
        real_low(std::integral_constant<int, 5>(), -1.0, 1.0)(7),
        real_mid(std::integral_constant<int, 5>(), -1.0, 1.0)(7),
        real_low(std::integral_constant<int, 5>(), -1.0, 1.0)(7),
        real_high(std::integral_constant<int, 5>(), -1.0, 1.0)(7), 
    };

    auto g4 = unique(esdl::merge(_merge2))(5);
    g4.evaluate_using(std::make_shared<TestEvaluator>());
    auto g4l = g4.as_vector();
    _assert(g4l.size() == 3);
    _assert(g4l.front().genome[0] < 0 && g4l.back().genome[0] > 0);

    test_pass();
}