#pragma once

#include <algorithm>
#include <list>
#include "individuals\individuals.h"

template<typename SourceType>
class uniform_random_t
{
    SourceType source;
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

    typename esdl::tt::group_type<SourceType>::type pSource;
public:

    uniform_random_t(SourceType source) : pSource(source()) { }

    esdl::group<IndividualType> operator()(int count) {
        auto pResult = esdl::make_group<IndividualType>(count);
        
        auto& src = *pSource;
        auto& dest = *pResult;
        auto _rand = random_array(count);
        auto& rand = *_rand;
        const int size = pSource.size();

        parallel_for_each(src.accelerator_view, dest.extent, [&, size](index<1> i) restrict(amp) {
            dest[i] = src[(int)(rand[i] * size)];
        });

        pResult.evaluate_using(pSource);
        pResult.evaluated = pSource.evaluated;
        return pResult;
    }
};

template<typename SourceType>
uniform_random_t<SourceType> uniform_random(SourceType source) {
    return uniform_random_t<SourceType>(source);
}