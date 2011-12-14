#pragma once

#include <algorithm>
#include <list>
#include "individuals\individuals.h"
#include "operators\merge.h"

template<typename SourceType>
esdl::Merger_t<typename esdl::tt::group_type<SourceType>::type, 1> best(SourceType source) {
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;

    auto pSource = source();
    pSource.evaluate();
    auto pResult = esdl::group<IndividualType, EvaluatorType>(esdl_sort::parallel_sort(*pSource, false), pSource.evalptr);
    pResult.evaluated = true;
    return esdl::merge(pResult);
}

template<typename SourceType>
esdl::Merger_t<typename esdl::tt::group_type<SourceType>::type, 1> worst(SourceType source) {
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;

    auto pSource = source();
    pSource.evaluate();
    auto pResult = esdl::group<IndividualType, EvaluatorType>(esdl_sort::parallel_sort(*pSource, true), pSource.evalptr);
    pResult.evaluated = true;
    return esdl::merge(pResult);
}
