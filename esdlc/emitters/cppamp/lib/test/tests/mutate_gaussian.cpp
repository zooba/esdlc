#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "operators\mutate_gaussian.h"

void test_mutate_gaussian_real() {
    test_start(L"Mutate Gaussian on real individuals");
    
    auto g1 = real_mid(std::integral_constant<int, 100>(), -1.0, 1.0)(100);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    int count_below = 0, count_above = 0, count_unchanged_genes = 0, count_unchanged_indivs = 0, total;

    // All individuals, 50% of genes
    auto g2 = mutate_gaussian(esdl::merge(g1), 0, 1.0f, 1.0f, 0.5f, 0)();
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return x.lowest == -1.0 && x.highest == 1.0; });
    std::for_each(begin(g2l), end(g2l), [&](const Indiv& x) {
        count_below += std::count_if(begin(x.genome), end(x.genome), [](float i) { return i < 0.0f; });
        count_above += std::count_if(begin(x.genome), end(x.genome), [](float i) { return i > 0.0f; });
            
        int unchanged = std::count_if(begin(x.genome), end(x.genome), [](float i) { return i == 0.0f; });
        count_unchanged_genes += unchanged;
        count_unchanged_indivs += (unchanged == esdl::tt::length<Indiv>::value) ? 1 : 0;
    });
    total = g2.size() * esdl::tt::length<Indiv>::value;
    _assert((total * 0.23) < count_below && count_below < (total * 0.27));
    _assert((total * 0.23) < count_above && count_above < (total * 0.27));
    _assert((total * 0.48) < count_unchanged_genes && count_unchanged_genes < (total * 0.52));
    _assert(count_unchanged_indivs < 2);

    // 50% of individuals, all genes
    count_below = 0, count_above = 0, count_unchanged_genes = 0, count_unchanged_indivs = 0;
    g2 = mutate_gaussian(esdl::merge(g1), 0, 1.0f, 0.5f, 1.0f, 0)();
    g2l = g2;
    assert_all(g2l, [](const Indiv& x) { return x.lowest == -1.0 && x.highest == 1.0; });
    std::for_each(begin(g2l), end(g2l), [&](const Indiv& x) {
        count_below += std::count_if(begin(x.genome), end(x.genome), [](float i) { return i < 0.0f; });
        count_above += std::count_if(begin(x.genome), end(x.genome), [](float i) { return i > 0.0f; });
            
        int unchanged = std::count_if(begin(x.genome), end(x.genome), [](float i) { return i == 0.0f; });
        count_unchanged_genes += unchanged;
        count_unchanged_indivs += (unchanged == esdl::tt::length<Indiv>::value) ? 1 : 0;
    });
    total = g2.size() * esdl::tt::length<Indiv>::value;

    _assert(abs(count_below - count_above) < 0.10 * total);
    _assert((total * 0.20) < count_unchanged_genes && count_unchanged_genes < (total * 0.80));
    _assert((g1.size() * 0.40) < count_unchanged_indivs && count_unchanged_indivs < (g1.size() * 0.60));

    test_pass();
}

void test_mutate_gaussian_int() {
    test_start(L"Mutate Gaussian on integer individuals");

    auto g1 = integer_mid(std::integral_constant<int, 100>(), -100, 100)(1000);
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    auto g1l = g1.as_vector();

    int count_below = 0, count_above = 0, count_unchanged_genes = 0, count_unchanged_indivs = 0, total;

    // All individuals, 50% of genes
    auto g2 = mutate_gaussian(esdl::merge(g1), 0, 50, 1.0f, 0.5f, 0)();
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return x.lowest == -100 && x.highest == 100; });
    std::for_each(begin(g2l), end(g2l), [&](const Indiv& x) {
        count_below += std::count_if(begin(x.genome), end(x.genome), [](int i) { return i < 0; });
        count_above += std::count_if(begin(x.genome), end(x.genome), [](int i) { return i > 0; });
        
        int unchanged = std::count_if(begin(x.genome), end(x.genome), [](int i) { return i == 0; });
        count_unchanged_genes += unchanged;
        count_unchanged_indivs += (unchanged == esdl::tt::length<Indiv>::value) ? 1 : 0;
    });
    total = g2.size() * esdl::tt::length<Indiv>::value;
    _assert((total * 0.23) < count_below && count_below < (total * 0.27));
    _assert((total * 0.23) < count_above && count_above < (total * 0.27));
    _assert((total * 0.48) < count_unchanged_genes && count_unchanged_genes < (total * 0.52));
    _assert(count_unchanged_indivs == 0);

    // 50% of individuals, all genes
    count_below = 0, count_above = 0, count_unchanged_genes = 0, count_unchanged_indivs = 0;
    g2 = mutate_gaussian(esdl::merge(g1), 0, 50.0f, 0.5f, 1.0f, 0)();
    g2l = g2;
    assert_all(g2l, [](const Indiv& x) { return x.lowest == -100 && x.highest == 100; });
    std::for_each(begin(g2l), end(g2l), [&](const Indiv& x) {
        count_below += std::count_if(begin(x.genome), end(x.genome), [](float i) { return i < 0; });
        count_above += std::count_if(begin(x.genome), end(x.genome), [](float i) { return i > 0; });
            
        int unchanged = std::count_if(begin(x.genome), end(x.genome), [](float i) { return i == 0; });
        count_unchanged_genes += unchanged;
        count_unchanged_indivs += (unchanged == esdl::tt::length<Indiv>::value) ? 1 : 0;
    });
    total = g2.size() * esdl::tt::length<Indiv>::value;
    
    _assert(abs(count_below - count_above) < 0.10 * total);
    _assert((total * 0.20) < count_unchanged_genes && count_unchanged_genes < (total * 0.80));
    _assert((g1.size() * 0.40) < count_unchanged_indivs && count_unchanged_indivs < (g1.size() * 0.60));

    test_pass();
}