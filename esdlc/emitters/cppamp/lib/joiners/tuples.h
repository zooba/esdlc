#pragma once

#include <tuple>
#include <type_traits>
#include "..\utility.h"
#include "..\individuals\joined_individuals.h"
#include "..\operators\merge.h"
#include "..\selectors\repeated.h"

template<int Count, typename S1, typename S2, typename S3 = esdl::_nil>
class tuples_t;

template<typename Source1, typename Source2>
class tuples_t<2, Source1, Source2> {
    Source1 source1;
    Source2 source2;

    typedef esdl::joinedgroup<
        2, void,
        esdl::group<typename esdl::tt::individual_type<Source1>::type, typename esdl::tt::evaluator_type<Source1>::type>,
        esdl::group<typename esdl::tt::individual_type<Source2>::type, typename esdl::tt::evaluator_type<Source2>::type>
        > GroupType;
public:
    
    tuples_t(Source1 source1, Source2 source2)
        : source1(source1), source2(source2) { }

    GroupType operator()() {
        auto pSrc1 = source1();
        auto pSrc2 = repeated(source2)(pSrc1.size());

        return GroupType(pSrc1, pSrc2);
    }

    GroupType operator()(int count) {
        auto pSrc1 = source1(count);
        auto pSrc2 = repeated(source2)(pSrc1.size());

        return GroupType(pSrc1, pSrc2);
    }
};


template<typename Source1, typename Source2, typename Source3>
class tuples_t<3, Source1, Source2, Source3> {
    Source1 source1;
    Source2 source2;
    Source3 source3;

    typedef esdl::joinedgroup<3, void,
        esdl::group<typename esdl::tt::individual_type<Source1>::type, typename esdl::tt::evaluator_type<Source1>::type>,
        esdl::group<typename esdl::tt::individual_type<Source2>::type, typename esdl::tt::evaluator_type<Source2>::type>,
        esdl::group<typename esdl::tt::individual_type<Source3>::type, typename esdl::tt::evaluator_type<Source3>::type>
        > GroupType;
public:
    
    tuples_t(Source1 source1, Source2 source2, Source3 source3)
        : source1(source1), source2(source2), source3(source3) { }

    GroupType operator()() {
        auto pSrc1 = source1();
        auto pSrc2 = repeated(source2)(pSrc1.size());
        auto pSrc3 = repeated(source3)(pSrc1.size());

        return GroupType(pSrc1, pSrc2, pSrc3);
    }

    GroupType operator()(int count) {
        auto pSrc1 = source1(count);
        auto pSrc2 = repeated(source2)(pSrc1.size());
        auto pSrc3 = repeated(source3)(pSrc1.size());

        return GroupType(pSrc1, pSrc2, pSrc3);
    }
};



template<typename Source1, typename Source2>
tuples_t<2, Source1, Source2> tuples(Source1 source1, Source2 source2) {
    return tuples_t<2, Source1, Source2>(source1, source2);
}

template<typename Source1, typename Source2, typename Source3>
tuples_t<3, Source1, Source2, Source3> tuples(Source1 source1, Source2 source2, Source3 source3) {
    return tuples_t<3, Source1, Source2, Source3>(source1, source2, source3);
}
