#pragma once

#include <tuple>
#include <type_traits>
#include "..\utility.h"
#include "..\individuals\joined_individuals.h"
#include "..\operators\merge.h"
#include "..\operators\repeated.h"
#include "..\selectors\uniform_random.h"

template<int Count, bool distinct, typename S1, typename S2, typename S3=esdl::_nil>
class random_tuples_t;

template<typename Source1, typename Source2, bool distinct>
class random_tuples_t<2, distinct, Source1, Source2>  {
    Source1 source1;
    Source2 source2;

public:
    typedef esdl::joinedgroup<2, void,
        typename esdl::tt::group_type<Source1>::type,
        typename esdl::tt::group_type<Source2>::type
        > GroupType;
    
    random_tuples_t(Source1 source1, Source2 source2)
        : source1(source1), source2(source2) { }

    GroupType operator()() {
        auto pSrc1 = source1();
        auto pSrc2 = uniform_random(source2)(pSrc1.size());

        return GroupType(pSrc1, pSrc2);
    }

    GroupType operator()(int count) {
        auto pSrc1 = source1(count);
        auto pSrc2 = uniform_random(source2)(pSrc1.size());

        return GroupType(pSrc1, pSrc2);
    }
};


template<typename Source1, typename Source2, typename Source3, bool distinct>
class random_tuples_t<3, distinct, Source1, Source2, Source3>  {
    Source1 source1;
    Source2 source2;
    Source3 source3;

public:
    typedef esdl::joinedgroup<3, void,
        typename esdl::tt::group_type<Source1>::type, 
        typename esdl::tt::group_type<Source2>::type,
        typename esdl::tt::group_type<Source3>::type
        > GroupType;
    
    random_tuples_t(Source1 source1, Source2 source2, Source3 source3)
        : source1(source1), source2(source2), source3(source3) { }

    GroupType operator()() {
        auto pSrc1 = source1();
        auto pSrc2 = uniform_random(source2)(pSrc1.size());
        auto pSrc3 = uniform_random(source3)(pSrc1.size());

        return GroupType(pSrc1, pSrc2, pSrc3);
    }

    GroupType operator()(int count) {
        auto pSrc1 = source1(count);
        auto pSrc2 = uniform_random(source2)(pSrc1.size());
        auto pSrc3 = uniform_random(source3)(pSrc1.size());

        return GroupType(pSrc1, pSrc2, pSrc3);
    }
};



template<typename Source1, typename Source2, typename distinct>
random_tuples_t<2, distinct::value, Source1, Source2>
random_tuples(Source1 source1, Source2 source2, distinct) {
    return random_tuples_t<2, distinct::value, Source1, Source2>(source1, source2);
}

template<typename Source1, typename Source2, typename Source3, typename distinct>
random_tuples_t<3, distinct::value, Source1, Source2, Source3>
random_tuples(Source1 source1, Source2 source2, Source3 source3, distinct) {
    return random_tuples_t<3, distinct::value, Source1, Source2, Source3>(source1, source2, source3);
}
