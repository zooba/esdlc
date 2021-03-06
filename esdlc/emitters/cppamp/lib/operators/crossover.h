#pragma once

#include "..\rng.h"
#include "..\utility.h"
#include "..\individuals\individuals.h"

template<typename SourceType, int points>
class crossover_t
{
private:
    SourceType source;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

    float per_pair_rate;
    bool one_child;
public:
    crossover_t(SourceType source, float per_pair_rate, bool one_child)
        : source(source), per_pair_rate(per_pair_rate), one_child(one_child) { }

    esdl::group<IndividualType> operator()() {
        return exec(source());
    }

    esdl::group<IndividualType> operator()(int count) {
        return exec(source(count * (one_child ? 2 : 1)));
    }
private:
    esdl::group<IndividualType> exec(const typename esdl::tt::group_type<SourceType>::type& src) {
        const int count = src.size() / 2;
        auto& source = *src;
        const int length = esdl::tt::length<IndividualType>::value;

        esdl::group<IndividualType> pResult;

        if (one_child) {
            pResult = esdl::make_group<IndividualType>(count);
            auto& result = *pResult;

            auto _rand = random_array(count, points);
            auto& rand = *_rand;
            
            parallel_for_each(result.accelerator_view, extent<2>(count, length),
                [&, length](index<2> i) restrict(amp) {
                    int crosses = 0;
                    for (int j = 0; j < points; ++j) {
                        if (i[1] >= (int)(rand(i[0], j) * length)) {
                            crosses += 1;
                        }
                    }

                    result(i[0]).genome[i[1]] = source(i[0] * 2 + (crosses & 1)).genome[i[1]];
            });
        } else {
            pResult = esdl::make_group<IndividualType>(count * 2);
            auto& result = *pResult;
            parallel_for_each(result.accelerator_view, result.extent, [&](index<1> i) restrict(amp) {
                result[i] = source[i];
            });

            for (int point = 0; point < points; ++point) {
                auto _rand = random_array(count);
                auto& rand = *_rand;

                parallel_for_each(result.accelerator_view, extent<2>(count, length),
                    [&, length](index<2> i) restrict(amp) {
                        if (i[1] >= (int)(rand(i[0]) * length)) {
                            auto temp = result(i[0] * 2).genome[i[1]];
                            result(i[0] * 2).genome[i[1]] = result(i[0] * 2 + 1).genome[i[1]];
                            result(i[0] * 2 + 1).genome[i[1]] = temp;
                        }
                });

            }
        }

        pResult.evaluate_using(src);
        return pResult;
    }
};

template<typename SourceType, typename points>
crossover_t<SourceType, points::value>
crossover(SourceType source, points, float per_pair_rate, bool one_child, bool two_children) {
    return crossover_t<SourceType, points::value>(source, per_pair_rate, one_child && !two_children);
}


template<typename SourceType>
crossover_t<SourceType, 1>
crossover_one(SourceType source, float per_pair_rate, bool one_child, bool two_children) {
    return crossover_t<SourceType, 1>(source, per_pair_rate, one_child && !two_children);
}
