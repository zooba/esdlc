#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "selectors\repeated.h"
#include "operators\unique.h"

void test_repeated() {
    test_start(L"Repeat individuals");
    
    auto g1 = random_real(std::integral_constant<int, 5>(), -1.0, 1.0)(100);
    
    auto g2 = repeated(esdl::merge(g1))(50);
    auto g3 = repeated(esdl::merge(g1))(200);
    auto g4 = repeated(esdl::merge(g1))(100);

    _assert(g2.size() == 50);
    _assert(g3.size() == 200);
    _assert(g4.size() == 100);

    auto _gen = repeated(esdl::merge(g1));
    g2 = _gen(74);
    g3 = _gen(23);
    g4 = _gen(1);
    auto g5 = _gen(200 - 74 - 23 - 1);

    decltype(g2) _merge[] = { g2, g3, g4, g5 };
    g2 = esdl::merge(_merge)();

    auto g1l = g1.as_vector();
    auto g2l = g2.as_vector();

    _assert(g2.size() == 200);
    for (unsigned int i = 0; i < g2l.size(); ++i) {
        for (unsigned int j = 0; j < 5; ++j) {
            _assert(g1l[i % g1l.size()].genome[j] == g2l[i].genome[j]);
        }
    }

    test_pass();
}