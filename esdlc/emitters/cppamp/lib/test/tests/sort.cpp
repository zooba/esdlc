#include "stdafx.h"

#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "bitonic_sort.h"

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
    auto arr2 = bitonic_sort::parallel_sort(arr, true);
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
    
    auto g1 = random_real(std::integral_constant<int, 5>(), -1.0, 1.0)(10000);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    g1.evaluate();
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;
    
    auto g2 = esdl::group<Indiv>(bitonic_sort::parallel_sort(*g1));
    auto g1l = g1.as_vector();
    auto g2l = g2.as_vector();

    test_pass();
}


void test_sort() {
    test_sort_numbers();
    test_sort_individuals();
}