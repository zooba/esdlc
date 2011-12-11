#include "stdafx.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"

#include "individuals\real_fixed_generators.h"
#include "individuals\int_fixed_generators.h"

#include "operators\merge.h"

#include "joiners\tuples.h"
#include "joiners\random_tuples.h"

void test_tuples_2() {
    test_start(L"Tuples joiner (pairs)");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    auto s2 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(4);

    auto g1 = tuples(esdl::merge(s1), esdl::merge(s2))(5);
    _assert(g1.size() == 5);
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 0>::type>::type Indiv1;
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 1>::type>::type Indiv2;

    static_assert(std::is_same<Indiv1, Indiv2>::value, "Type mismatch");
    auto g1l = g1.group1.as_list();
    assert_all(g1l, [](const Indiv1& x) { return x.genome[0] == -1.0f; });

    g1l = g1.group2;
    assert_all(g1l, [](const Indiv2& x) { return x.genome[0] == 1.0f; });

    g1 = tuples(esdl::merge(s1), esdl::merge(s2))();
    _assert(g1.size() == s1.size());

    test_pass();
}

void test_tuples_3() {
    test_start(L"Tuples joiner (triplets)");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    auto s2 = real_mid(std::integral_constant<int, 5>(), -1.0f, 1.0f)(2);
    auto s3 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(12);

    auto g1 = tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))(5);
    _assert(g1.size() == 5);
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 0>::type>::type Indiv1;
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 1>::type>::type Indiv2;
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 2>::type>::type Indiv3;

    static_assert(std::is_same<Indiv1, Indiv2>::value, "Type mismatch");
    static_assert(std::is_same<Indiv1, Indiv3>::value, "Type mismatch");
    auto g1l = g1.group1.as_list();
    assert_all(g1l, [](const Indiv1& x) { return x.genome[0] == -1.0f; });

    g1l = g1.group2;
    assert_all(g1l, [](const Indiv2& x) { return x.genome[0] == 0.0f; });

    g1l = g1.group3;
    assert_all(g1l, [](const Indiv3& x) { return x.genome[0] == 1.0f; });

    g1 = tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3))();
    _assert(g1.size() == s1.size());

    test_pass();
}


void test_random_tuples_2() {
    test_start(L"Random tuples joiner (pairs)");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    auto s2 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(4);

    auto g1 = random_tuples(esdl::merge(s1), esdl::merge(s2), std::false_type())(5);
    _assert(g1.size() == 5);
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 0>::type>::type Indiv1;
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 1>::type>::type Indiv2;

    static_assert(std::is_same<Indiv1, Indiv2>::value, "Type mismatch");
    auto g1l = g1.group1.as_list();
    assert_all(g1l, [](const Indiv1& x) { return x.genome[0] == -1.0f; });

    g1l = g1.group2;
    assert_all(g1l, [](const Indiv2& x) { return x.genome[0] == 1.0f; });

    g1 = random_tuples(esdl::merge(s1), esdl::merge(s2), std::false_type())();
    _assert(g1.size() == s1.size());

    test_pass();
}

void test_random_tuples_3() {
    test_start(L"Random tuples joiner (triplets)");

    auto s1 = real_low(std::integral_constant<int, 5>(), -1.0f, 1.0f)(10);
    auto s2 = real_mid(std::integral_constant<int, 5>(), -1.0f, 1.0f)(2);
    auto s3 = real_high(std::integral_constant<int, 5>(), -1.0f, 1.0f)(12);

    auto g1 = random_tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3), std::false_type())(5);
    _assert(g1.size() == 5);
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 0>::type>::type Indiv1;
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 1>::type>::type Indiv2;
    typedef esdl::tt::individual_type<typename esdl::tt::joined_group_type<decltype(g1), 2>::type>::type Indiv3;

    static_assert(std::is_same<Indiv1, Indiv2>::value, "Type mismatch");
    static_assert(std::is_same<Indiv1, Indiv3>::value, "Type mismatch");
    auto g1l = g1.group1.as_list();
    assert_all(g1l, [](const Indiv1& x) { return x.genome[0] == -1.0f; });

    g1l = g1.group2;
    assert_all(g1l, [](const Indiv2& x) { return x.genome[0] == 0.0f; });

    g1l = g1.group3;
    assert_all(g1l, [](const Indiv3& x) { return x.genome[0] == 1.0f; });

    g1 = random_tuples(esdl::merge(s1), esdl::merge(s2), esdl::merge(s3), std::false_type())();
    _assert(g1.size() == s1.size());

    test_pass();
}

