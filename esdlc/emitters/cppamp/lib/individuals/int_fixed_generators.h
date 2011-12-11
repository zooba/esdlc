#pragma once

#include "int_fixed.h"

template<int Length, typename EvaluatorType>
class _int_init_t {
    int lowest, highest;
    int init;
    bool random;
public:
    typedef int_fixed_individual<Length> IndividualType;
    typedef EvaluatorType EvaluatorType;

    _int_init_t(int lowest, int highest, int init, bool random)
        : lowest(lowest), highest(highest), init(init), random(random) { }

    esdl::group<IndividualType, EvaluatorType> operator()(int count) {
        auto pGroup = esdl::make_group<IndividualType, EvaluatorType>(count);
        auto& group = *pGroup;

        const int _highest = highest;
        const int _lowest = lowest;
        if (random) {
            auto _rand = random_array(count, Length);
            auto& rand = *_rand;
            
            parallel_for_each(esdl::acc, grid<2>(extent<2>(count, Length)),
                [=, &group, &rand](index<2> i) restrict(direct3d) {
                    group(i[0]).genome[i[1]] = _lowest + (int)(rand[i] * (_highest - _lowest));
                    group(i[0]).fitness = 0;
                    group(i[0]).lowest = _lowest;
                    group(i[0]).highest = _highest;
            });
        } else {
            const int _init = init;
            parallel_for_each(esdl::acc, grid<2>(extent<2>(count, Length)),
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
_int_init_t<length::value, void> random_integer(length, int lowest, int highest) {
    return _int_init_t<length::value, void>(lowest, highest, 0, true);
}

template<typename length>
_int_init_t<length::value, void> integer_low(length, int lowest, int highest) {
    return _int_init_t<length::value, void>(lowest, highest, lowest, false);
}

template<typename length>
_int_init_t<length::value, void> integer_high(length, int lowest, int highest) {
    return _int_init_t<length::value, void>(lowest, highest, highest, false);
}

template<typename length>
_int_init_t<length::value, void> integer_mid(length, int lowest, int highest) {
    return _int_init_t<length::value, void>(lowest, highest, lowest + (highest - lowest) / 2, false);
}
