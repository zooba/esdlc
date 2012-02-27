#pragma once

#include "..\rng.h"
#include "..\individuals\individuals.h"

template<typename SourceType>
class clamp_t {
    SourceType source;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::element_type<IndividualType>::type ElementType;
    ElementType lowest, highest;


public:
    clamp_t(SourceType source, ElementType lowest, ElementType highest)
        : source(source), lowest(lowest), highest(highest) { }

    esdl::group<IndividualType> operator()(int count) { return mutate(source(count)); }
    esdl::group<IndividualType> operator()() { return mutate(source()); }

private:
    esdl::group<IndividualType> mutate(esdl::group<IndividualType> pSrc) {
        int count = pSrc.size();
        auto& src = *pSrc;
        auto pDest = make_group(pSrc);
        auto& dest = *pDest;

        const int length = esdl::tt::length<IndividualType>::value;
        const auto _lowest = lowest;
        const auto _highest = highest;

        parallel_for_each(dest.accelerator_view, grid<2>(extent<2>(count, length)),
            [=, &dest](index<2> i) restrict(direct3d) {
            ElementType x = dest(i[0]).genome[i[1]];
            const auto _lowest2 = (_lowest < _highest) ? _lowest : dest(i[0]).lowest;
            const auto _highest2 = (_lowest < _highest) ? _highest : dest(i[0]).highest;
            if (x < _lowest2) {
                dest(i[0]).genome[i[1]] = _lowest2;
                dest(i[0]).fitness = 0;
            } else if (x > _highest2) {
                dest(i[0]).genome[i[1]] = _highest2;
                dest(i[0]).fitness = 0;
            }
        });

        return pDest;
    }
};

template<typename SourceType, typename ElementType>
clamp_t<SourceType> clamp(SourceType source, ElementType lowest, ElementType highest) {
    return clamp_t<SourceType>(source, lowest, highest);
}
