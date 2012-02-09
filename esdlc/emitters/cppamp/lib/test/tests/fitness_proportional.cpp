#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"

#include "operators\merge.h"

#include "selectors\best.h"
#include "selectors\fitness_proportional.h"

void test_single_fitness_proportional_replace() {
    test_start(L"Fitness-proportional selection (with replacement)");

    auto g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = fitness_proportional(esdl::merge(g1), nullptr, std::true_type(), std::false_type())(100);
    auto g2l = g2.as_vector();

    assert_all(g2l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 10.0; });
    _assert(g2l.size() == 100);

    auto g3 = worst(esdl::merge(g1))(1);
    auto g4 = fitness_proportional(esdl::merge(g1), g3, std::true_type(), std::false_type())(100);
    auto g4l = g4.as_vector();

    assert_all(g4l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 10.0; });
    _assert(g4l.size() == 100);

    test_pass();
}

void test_repeated_fitness_proportional_replace() {
    test_start(L"Fitness-proportional selection (x100, with replacement)");
    
    auto g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    double distribution[100] = {0};

    for (int i = 0; i < 100; ++i) {
        auto g2 = fitness_proportional(esdl::merge(g1), nullptr, std::true_type(), std::false_type())(100);
        auto g2l = g2.as_vector();

        for (int j = 0; j < 100; ++j) {
            distribution[j] += g2l[j].fitness;
        }
    }

    for (int i = 1; i < 100; ++i) {
        distribution[i - 1] -= distribution[i];
    }
    distribution[99] = 0;

    assert_all(distribution, [](double d) { return (d + 500.0) > 0.0; });

    test_pass();
}

void test_single_fitness_proportional_no_replace() {
    test_start(L"Fitness-proportional selection (without replacement)");

    auto g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    auto g2 = fitness_proportional(esdl::merge(g1), nullptr, std::false_type(), std::false_type())();
    auto g3 = fitness_proportional(esdl::merge(g1), nullptr, std::true_type(), std::true_type())();
    auto g2l = g2.as_vector();

    assert_all(g2l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 10.0; });
    _assert(g2l.size() == 100);

    auto g4 = worst(esdl::merge(g1))(1);
    auto g5 = fitness_proportional(esdl::merge(g1), g4, std::true_type(), std::false_type())(100);
    auto g5l = g5.as_vector();

    assert_all(g5l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 10.0; });
    _assert(g5l.size() == 100);

    test_pass();
}

void test_repeated_fitness_proportional_no_replace() {
    test_start(L"Fitness-proportional selection (x100, without replacement)");
    
    auto g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    double distribution[100] = {0};

    for (int i = 0; i < 100; ++i) {
        auto g2 = fitness_proportional(esdl::merge(g1), nullptr, std::false_type(), std::false_type())();
        auto g3 = fitness_proportional(esdl::merge(g1), nullptr, std::true_type(), std::true_type())();
        auto g2l = g2.as_vector();

        for (int j = 0; j < 100; ++j) {
            distribution[j] += g2l[j].fitness;
        }
    }

    for (int i = 1; i < 100; ++i) {
        distribution[i - 1] -= distribution[i];
    }
    distribution[99] = 0;

    assert_all(distribution, [](double d) { return (d + 2500.0) > 0.0; });

    test_pass();
}



void test_fitness_proportional() {
    test_single_fitness_proportional_replace();
    test_repeated_fitness_proportional_replace();
    test_single_fitness_proportional_no_replace();
    test_repeated_fitness_proportional_no_replace();
}