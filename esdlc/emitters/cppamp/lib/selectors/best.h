#pragma once

#include <algorithm>
#include <list>
#include "bitonic_sort.h"
#include "individuals\individuals.h"
#include "operators\merge.h"

template<typename SourceType>
esdl::Merger_t<typename esdl::tt::group_type<SourceType>::type, 1> best(SourceType source) {
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

    auto pSource = source();
    pSource.evaluate();
    auto pResult = esdl::group<IndividualType>(bitonic_sort::parallel_sort(*pSource, false));
    pResult.evaluate_using(pSource);
    pResult.evaluated = true;
    return esdl::merge(pResult);
}

template<typename SourceType>
esdl::Merger_t<typename esdl::tt::group_type<SourceType>::type, 1> worst(SourceType source) {
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

    auto pSource = source();
    pSource.evaluate();
    auto pResult = esdl::group<IndividualType>(bitonic_sort::parallel_sort(*pSource, true));
    pResult.evaluate_using(pSource);
    pResult.evaluated = true;
    return esdl::merge(pResult);
}
