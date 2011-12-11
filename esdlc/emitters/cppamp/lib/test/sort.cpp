#include "stdafx.h"

#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "sort.h"

void test_sort_numbers() {
    auto _arr = random_array(512 * 512);
    auto& arr = *_arr;
    std::vector<float> vec;
    concurrency::copy(arr, std::back_inserter(vec));

    bool allAsc = true, allDesc = true;
    float previous = vec[0];
    std::for_each(std::begin(vec), std::end(vec), [&](float f) {
        if (f < previous) allAsc = false;
        if (f > previous) allDesc = false;
        previous = f;
    });
    _assert(!allAsc && !allDesc);

    test_start(L"Sort numbers");
    auto arr2 = esdl_sort::parallel_sort(arr, true);
    vec.clear();

    concurrency::copy(*arr2, std::back_inserter(vec));

    allAsc = true, allDesc = true;
    previous = vec[0];
    std::for_each(std::begin(vec), std::end(vec), [&](float f) {
        if (f < previous) allAsc = false;
        if (f > previous) allDesc = false;
        previous = f;
    });
    _assert(!allAsc && allDesc);

    test_pass();
}

void test_sort_individuals() {
    test_start(L"Sort individuals");
    
    auto _g1 = random_real(std::integral_constant<int, 5>(), -1.0, 1.0)(10000);
    auto g1 = _g1.evaluate_using(TestEvaluator(_g1));
    g1.evaluate();
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;
    typedef esdl::tt::evaluator_type<decltype(g1)>::type Evaluator;
    
    auto g2 = esdl::group<Indiv, Evaluator>(esdl_sort::parallel_sort(*g1));
    auto g1l = g1.as_vector();
    auto g2l = g2.as_vector();

    test_pass();
}


void test_sort() {
    test_sort_numbers();
    test_sort_individuals();
}