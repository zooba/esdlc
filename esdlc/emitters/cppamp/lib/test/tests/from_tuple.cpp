#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\bin_fixed_generators.h"
#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "joiners\tuples.h"
#include "operators\from_tuple.h"

void test_from_tuple_2() {
    test_start(L"From tuple on pairs");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    auto s2 = integer_low(std::integral_constant<int, 10>(), -100, 100)(10);
    typedef esdl::tt::individual_type<decltype(s1)>::type Indiv1;
    typedef esdl::tt::individual_type<decltype(s2)>::type Indiv2;

    auto g1 = from_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2))()), std::integral_constant<int, 1>)();
    static_assert(std::is_same<typename esdl::tt::individual_type<decltype(g1)>::type, Indiv1>::value)
    _assert(g1.size() == 10);
    auto g1l = g1.as_list();
    assert_all(g1l, [](const Indiv1& indiv) { 
        return std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == -1.0f; });
    });

    auto g2 = from_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2))()), std::integral_constant<int, 2>)();
    static_assert(std::is_same<typename esdl::tt::individual_type<decltype(g2)>::type, Indiv2>::value)
    _assert(g2.size() == 10);
    auto g2l = g2.as_list();
    assert_all(g2l, [](const Indiv2& indiv) { 
        return std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](int x) { return x == -100; });
    });

    test_pass();
}

void test_from_tuple_3() {
    test_start(L"From tuple on triplets");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    auto s2 = integer_low(std::integral_constant<int, 10>(), -100, 100)(10);
    auto s3 = binary_true(std::integral_constant<int, 20>())(10);
    typedef esdl::tt::individual_type<decltype(s1)>::type Indiv1;
    typedef esdl::tt::individual_type<decltype(s2)>::type Indiv2;
    typedef esdl::tt::individual_type<decltype(s3)>::type Indiv3;

    auto g1 = from_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))()), std::integral_constant<int, 1>)();
    static_assert(std::is_same<typename esdl::tt::individual_type<decltype(g1)>::type, Indiv1>::value)
    _assert(g1.size() == 10);
    auto g1l = g1.as_list();
    assert_all(g1l, [](const Indiv1& indiv) { 
        return std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](float x) { return x == -1.0f; });
    });

    auto g2 = from_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))()), std::integral_constant<int, 2>)();
    static_assert(std::is_same<typename esdl::tt::individual_type<decltype(g2)>::type, Indiv2>::value)
    _assert(g2.size() == 10);
    auto g2l = g2.as_list();
    assert_all(g2l, [](const Indiv2& indiv) { 
        return std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](int x) { return x == -100; });
    });

    auto g3 = from_tuple(esdl::merge(tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))()), std::integral_constant<int, 3>)();
    static_assert(std::is_same<typename esdl::tt::individual_type<decltype(g3)>::type, Indiv3>::value)
    _assert(g3.size() == 10);
    auto g3l = g3.as_list();
    assert_all(g3l, [](const Indiv3& indiv) { 
        return std::all_of(std::begin(indiv.genome), std::end(indiv.genome), [](int x) { return x == 1; });
    });

    test_pass();
}


