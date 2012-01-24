#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"

#include "operators\merge.h"

#include "selectors\best.h"

void test_best() {
    test_start(L"Best selection");
    
    auto _g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    auto g1 = _g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;
    
    auto g2 = best(esdl::merge(g1))();
    auto g2l = g2.as_vector();

    bool inOrder = true;
    for (int i = 1; i < 100; ++i) {
        if (g2l[i-1].fitness < g2l[i].fitness)
            inOrder = false;
    }
    _assert(inOrder);

    auto _gen = best(esdl::merge(g1));
    g2 = _gen(10);
    auto rest = _gen();

    g2l = g2;
    auto restl = rest.as_vector();

    inOrder = true;
    for (int i = 1; i < 10; ++i) {
        if (g2l[i-1].fitness < g2l[i].fitness)
            inOrder = false;
    }
    _assert(inOrder);
    assert_all(restl, [&](const Indiv& indiv) {
        return !std::all_of(std::begin(g2l), std::end(g2l), [&](const Indiv& j) { return indiv.fitness > j.fitness; });
    });

    test_pass();
}

void test_worst() {
    test_start(L"Worst selection");
    
    auto _g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    auto g1 = _g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;
    auto g1l = g1.as_vector();

    auto g2 = worst(esdl::merge(g1))();
    auto g2l = g2.as_vector();

    bool inOrder = true;
    for (int i = 1; i < 100; ++i) {
        if (g2l[i-1].fitness > g2l[i].fitness)
            inOrder = false;
    }
    _assert(inOrder);

    auto _gen = worst(esdl::merge(g1));
    g2 = _gen(10);
    auto rest = _gen();

    g2l = g2;
    auto restl = rest.as_vector();

    inOrder = true;
    for (int i = 1; i < 10; ++i) {
        if (g2l[i-1].fitness > g2l[i].fitness)
            inOrder = false;
    }
    _assert(inOrder);
    assert_all(restl, [&](const Indiv& indiv) {
        return !std::all_of(std::begin(g2l), std::end(g2l), [&](const Indiv& j) { return indiv.fitness < j.fitness; });
    });

    test_pass();
}
