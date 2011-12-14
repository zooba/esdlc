#pragma once

#include <algorithm>
#include <list>
#include "individuals\individuals.h"

template<typename SourceType>
class uniform_random_t
{
    SourceType source;
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;
public:

    uniform_random_t(SourceType source) : source(source) { }

    esdl::group<IndividualType, EvaluatorType> operator()(int count) {
        auto pSrc = source();
        auto pResult = esdl::make_group<IndividualType, EvaluatorType>(count, pSrc.evalptr);
        
        auto& src = *pSrc;
        auto& dest = *pResult;
        auto _rand = random_array(count);
        auto& rand = *_rand;
        const int size = pSrc.size();

        parallel_for_each(esdl::acc, dest.grid, [&, size](index<1> i) restrict(direct3d) {
            dest[i] = src[(int)(rand[i] * size)];
        });

        return pResult;
    }
};

template<typename SourceType>
uniform_random_t<SourceType> uniform_random(SourceType source) {
    return uniform_random_t<SourceType>(source);
}