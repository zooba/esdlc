#pragma once

#include <algorithm>
#include <list>
#include "sort.h"
#include "individuals\individuals.h"


template<typename SourceType>
class uniform_shuffle_t
{
    SourceType source;
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    struct _float { float f; operator float() restrict(cpu, direct3d) { return f; } };

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
        auto _keys = esdl_sort::parallel_sort_keys(*reinterpret_cast<concurrency::array<_float, 1>*>(_rand.get()));
        auto& keys = *_keys;

        parallel_for_each(src.accelerator_view, dest.grid, [&](index<1> i) restrict(direct3d) {
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