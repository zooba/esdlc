#pragma once

#include <algorithm>
#include <list>
#include "bitonic_sort.h"
#include "individuals\individuals.h"


template<typename SourceType>
class uniform_shuffle_t
{
    SourceType source;
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

public:

    uniform_shuffle_t(SourceType source) : source(source) { }

    esdl::group<IndividualType> operator()(int count) {
        auto pSource = source();
        if (count > pSource.size()) {
            count = pSource.size();
        }
        auto pResult = esdl::make_group<IndividualType>(count);
        
        auto& src = *pSource;
        auto& dest = *pResult;
        auto _rand = random_array(count);
        auto _keys = bitonic_sort::parallel_sort_keys(*_rand);
        auto& keys = *_keys;

        parallel_for_each(src.accelerator_view, dest.extent, [&](index<1> i) restrict(amp) {
            dest[i] = src[keys[i].i];
        });

        pResult.evaluate_using(pSource);
        return pResult;
    }

    esdl::group<IndividualType> operator()() {
        return (*this)(0x7FFFFFFF);
    }
};

template<typename SourceType>
uniform_shuffle_t<SourceType> uniform_shuffle(SourceType source) {
    return uniform_shuffle_t<SourceType>(source);
}