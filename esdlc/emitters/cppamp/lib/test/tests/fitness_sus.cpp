#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"

#include "operators\merge.h"

#include "selectors\best.h"
#include "selectors\fitness_sus.h"

void test_single_fitness_sus() {
    test_start(L"Fitness-proportional Stochastic Uniform Sampling");

    auto g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = fitness_sus(esdl::merge(g1), nullptr, 100)(100);
    auto g2l = g2.as_vector();

    assert_all(g2l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 10.0; });
    _assert(g2l.size() == 100);

    auto g3 = worst(esdl::merge(g1))(1);
    auto g4 = fitness_sus(esdl::merge(g1), g3, 100)(100);
    auto g4l = g4.as_vector();

    assert_all(g4l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 10.0; });
    _assert(g4l.size() == 100);

    test_pass();
}

void test_repeated_fitness_sus() {
    test_start(L"Fitness-proportional Stochastic Uniform Sampling (x100)");
    
    auto g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    double distribution[100] = {0};

    for (int i = 0; i < 100; ++i) {
        auto g2 = fitness_sus(esdl::merge(g1), nullptr, 100)(100);
        auto g2l = g2.as_vector();

        for (int j = 0; j < 100; ++j) {
            distribution[j] += g2l[j].fitness;
        }
    }

    for (int i = 1; i < 100; ++i) {
        distribution[i - 1] -= distribution[i];
    }
    distribution[99] = 0;

    assert_all(distribution, [](double d) { return (d + 10.0) > 0.0; });

    test_pass();
}

void test_fitness_sus() {
    test_single_fitness_sus();
    test_repeated_fitness_sus();
}