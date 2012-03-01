#pragma once

#include "..\utility.h"
#include "..\individuals\individuals.h"
#include "..\individuals\bin_fixed.h"

template<typename SourceType>
class mutate_inversion_t {
    SourceType source;
    float per_indiv_rate;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
public:

    mutate_inversion_t(SourceType source, float per_indiv_rate)
        : source(source), per_indiv_rate(per_indiv_rate)
    { }

    esdl::group<IndividualType> operator()(int count) { return mutate(source(count)); }
    esdl::group<IndividualType> operator()() { return mutate(source()); }

private:
    esdl::group<IndividualType> mutate(esdl::group<IndividualType> pSrc) {
        int count = pSrc.size();
        auto& src = *pSrc;
        auto pDest = make_group(pSrc);
        auto& dest = *pDest;

        const int length = esdl::tt::length<IndividualType>::value;
        const float _per_indiv_rate = per_indiv_rate;
        
        if (_per_indiv_rate >= 1.0f) {
            parallel_for_each(src.accelerator_view, extent<2>(count, length),
                [=, &dest](index<2> i) restrict(amp) {
                    dest(i[0]).genome[i[1]] = (dest(i[0]).genome[i[1]] == 0) ? 1 : 0;
                    dest(i[0]).fitness = 0;
            });
        } else {
            auto _rand = random_array(count, 1);
            auto& rand = *_rand;

            parallel_for_each(src.accelerator_view, extent<2>(count, length),
                [=, &dest, &rand](index<2> i) restrict(amp) {
                    if (rand(i[0], 0) < _per_indiv_rate) {
                        dest(i[0]).genome[i[1]] = (dest(i[0]).genome[i[1]] == 0) ? 1 : 0;
                        dest(i[0]).fitness = 0;
                    }
            });
        }

        return pDest;
    }
};

template<typename SourceType>
typename std::enable_if<
    esdl::tt::is_binary_individual<typename esdl::tt::individual_type<SourceType>::type>::value,
    mutate_inversion_t<SourceType>>::type
mutate_inversion(SourceType source, float per_indiv_rate) {
    return mutate_inversion_t<SourceType>(source, per_indiv_rate);
}
