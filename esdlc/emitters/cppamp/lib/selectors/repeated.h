#pragma once

#include "individuals\individuals.h"

template<typename SourceType>
class repeated_t
{
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;
    typename esdl::tt::group_type<SourceType>::type pSource;
    int taken;
public:

    repeated_t(SourceType source) : pSource(source()), taken(0) { }

    esdl::group<IndividualType, EvaluatorType> operator()(int count) {
        if (pSource.size() == 0 || (taken % count == 0 && pSource.size() == count)) {
            return pSource;
        }

        auto pDest = esdl::make_group<IndividualType, EvaluatorType>(count);
        
        auto& src = *pSource;
        auto& dest = *pDest;
        const int have = pSource.size();
        const int _taken = taken;
        taken += count;

        parallel_for_each(esdl::acc, grid<1>(extent<1>(count)), [=, &src, &dest](index<1> i) restrict(direct3d) {
            dest[i] = src[(i + _taken) % have];
        });

        return pDest;
    }
};

template<typename SourceType>
repeated_t<SourceType> repeated(SourceType source) {
    return repeated_t<SourceType>(source);
}
