#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "joiners\tuples.h"
#include "operators\best_of_tuple.h"

void test_best_of_tuple_2() {
    test_start(L"best_of_tuple on pairs");

    auto s1 = random_real(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    s1.evaluate_using(std::make_shared<TestEvaluator>());
    auto s2 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    s2.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(s1)>::type Indiv;

    auto g1 = best_of_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2))()))();
    _assert(g1.size() == 10);
    auto g1l = g1.as_list();
    assert_all(g1l, [](const Indiv& indiv) { 
        return std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == 1.0f; });
    });

    test_pass();
}

void test_best_of_tuple_3() {
    test_start(L"best_of_tuple on triplets");

    auto s1 = random_real(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    s1.evaluate_using(std::make_shared<TestEvaluator>());
    auto s2 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    s2.evaluate_using(std::make_shared<TestEvaluator>());
    auto s3 = random_real(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    s3.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(s1)>::type Indiv;

    auto g1 = best_of_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))()))();
    _assert(g1.size() == 10);
    auto g1l = g1.as_list();
    assert_all(g1l, [](const Indiv& indiv) { 
        return std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == 1.0f; });
    });

    test_pass();
}


