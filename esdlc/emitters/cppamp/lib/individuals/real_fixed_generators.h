#pragma once

#include "real_fixed.h"

template<int Length, typename EvaluatorType>
class _real_init_t {
    float lowest, highest;
    float init;
    bool random;
public:
    typedef real_fixed_individual<Length> IndividualType;
    typedef EvaluatorType EvaluatorType;

    _real_init_t(float lowest, float highest, float init, bool random)
        : lowest(lowest), highest(highest), init(init), random(random) { }

    esdl::group<IndividualType, EvaluatorType> operator()(int count) {
        auto pGroup = esdl::make_group<IndividualType, EvaluatorType>(count);
        auto& group = *pGroup;

        const float _highest = highest;
        const float _lowest = lowest;
        if (random) {
            auto _rand = random_array(count, Length);
            auto& rand = *_rand;

            
            parallel_for_each(group.accelerator_view, grid<2>(extent<2>(count, Length)),
                [=, &group, &rand](index<2> i) restrict(direct3d) {
                    group(i[0]).genome[i[1]] = _lowest + rand[i] * (_highest - _lowest);
                    group(i[0]).fitness = 0;
                    group(i[0]).lowest = _lowest;
                    group(i[0]).highest = _highest;
            });
        } else {
            const float _init = init;
            parallel_for_each(group.accelerator_view, grid<2>(extent<2>(count, Length)),
                [=, &group](index<2> i) restrict(direct3d) {
                    group(i[0]).genome[i[1]] = _init;
                    group(i[0]).fitness = 0;
                    group(i[0]).lowest = _lowest;
                    group(i[0]).highest = _highest;
            });
        }
        return pGroup;
    }
};

template<typename length>
_real_init_t<length::value, void> random_real(length, float lowest, float highest) {
    return _real_init_t<length::value, void>(lowest, highest, 0.0f, true);
}

template<typename length>
_real_init_t<length::value, void> real_value(length, float lowest, float highest, float value) {
    return _real_init_t<length::value, void>(lowest, highest, value, false);
}

template<typename length>
_real_init_t<length::value, void> real_low(length, float lowest, float highest) {
    return _real_init_t<length::value, void>(lowest, highest, lowest, false);
}

template<typename length>
_real_init_t<length::value, void> real_high(length, float lowest, float highest) {
    return _real_init_t<length::value, void>(lowest, highest, highest, false);
}

template<typename length>
_real_init_t<length::value, void> real_mid(length, float lowest, float highest) {
    return _real_init_t<length::value, void>(lowest, highest, lowest + (highest - lowest) / 2, false);
}
