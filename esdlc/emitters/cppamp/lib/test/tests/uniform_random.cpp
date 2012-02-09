#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"

#include "operators\merge.h"

#include "selectors\uniform_random.h"
#include "selectors\best.h"

void test_uniform_random() {
    test_start(L"Uniform random selection");
    
    auto g1 = random_real(std::integral_constant<int, 10>(), 0.0, 10.0)(100);
    g1.evaluate_using(std::make_shared<TestEvaluator>());
    typedef esdl::tt::individual_type<decltype(g1)>::type Indiv;

    g1 = best(esdl::merge(g1))();

    auto g2 = uniform_random(esdl::merge(g1))(100);
    auto g2l = g2.as_vector();
    assert_all(g2l, [](const Indiv& x) { return x.lowest == 0.0 && x.highest == 10.0; });
    _assert(g2l.size() == 100);

    bool inOrder = true;
    for (int i = 1; i < 100; ++i) {
        if (g2l[i-1].fitness < g2l[i].fitness)
            inOrder = false;
    }

    _assert(!inOrder);

    test_pass();
}