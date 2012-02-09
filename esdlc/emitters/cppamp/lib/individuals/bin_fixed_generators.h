#pragma once

#include "bin_fixed.h"

template<int Length>
class _bin_init_t {
    float true_rate;
public:
    typedef bin_fixed_individual<Length> IndividualType;

    _bin_init_t(float true_rate) : true_rate(true_rate) { }

    esdl::group<IndividualType> operator()(int count) {
        auto pGroup = esdl::make_group<IndividualType>(count);
        auto& group = *pGroup;

        const float _rate = true_rate;
        if (_rate <= 0.0f) {
            parallel_for_each(group.accelerator_view, grid<2>(extent<2>(count, Length)),
                [&](index<2> i) restrict(direct3d) {
                    group(i[0]).genome[i[1]] = 0;
                    group(i[0]).fitness = 0;
            });
        } else if (_rate >= 1.0f) {
            parallel_for_each(group.accelerator_view, grid<2>(extent<2>(count, Length)),
                [&](index<2> i) restrict(direct3d) {
                    group(i[0]).genome[i[1]] = 1;
                    group(i[0]).fitness = 0;
            });
        } else {
            auto _rand = random_array(count, Length);
            auto& rand = *_rand;
            
            parallel_for_each(group.accelerator_view, grid<2>(extent<2>(count, Length)),
                [&, _rate](index<2> i) restrict(direct3d) {
                    group(i[0]).genome[i[1]] = (rand[i] < _rate) ? 1 : 0;
                    group(i[0]).fitness = 0;
            });
        }

        return pGroup;
    }
};

template<typename length>
_bin_init_t<length::value> random_binary(length, float true_rate) {
    return _bin_init_t<length::value>(true_rate);
}

template<typename length>
_bin_init_t<length::value> binary_true(length) {
    return _bin_init_t<length::value>(1.0f);
}

template<typename length>
_bin_init_t<length::value> binary_false(length) {
    return _bin_init_t<length::value>(0.0f);
}
