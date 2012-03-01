#pragma once

#include "..\rng.h"
#include "..\utility.h"
#include "..\individuals\individuals.h"

template<typename SourceType>
class crossover_average_t
{
private:
    SourceType source;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

    float per_pair_rate, per_gene_rate;
    int genes;
public:
    crossover_average_t(SourceType source, float per_pair_rate, float per_gene_rate, int genes)
        : source(source), per_pair_rate(per_pair_rate), per_gene_rate(per_gene_rate), genes(genes) { }

    esdl::group<IndividualType> operator()() {
        return exec(source());
    }

    esdl::group<IndividualType> operator()(int count) {
        return exec(source(count * 2));
    }
private:
    esdl::group<IndividualType> exec(const typename esdl::tt::group_type<SourceType>::type& src) {
        const int count = src.size() / 2;
        auto& source = *src;

        auto pResult = esdl::make_group<IndividualType>(count);
        auto& result = *pResult;

        const int length = esdl::tt::length<IndividualType>::value;
        const float _per_pair_rate = per_pair_rate;
        const float _per_gene_rate = (genes > 0) ? (float)genes / length : per_gene_rate;

        if (_per_pair_rate >= 1.0f && _per_gene_rate >= 1.0f) {
            parallel_for_each(result.accelerator_view, extent<2>(count, length),
                [=, &result, &source](index<2> i) restrict(amp) {
                    result(i[0]).genome[i[1]] = (source(i[0] * 2).genome[i[1]] + source(i[0] * 2 + 1).genome[i[1]]) / 2;
            });
        } else if (_per_pair_rate >= 1.0f) {
            auto _rand = random_array(count, length);
            auto& rand = *_rand;

            parallel_for_each(result.accelerator_view, extent<2>(count, length),
                [=, &result, &source, &rand](index<2> i) restrict(amp) {
                    if (rand[i] < _per_gene_rate) {
                        result(i[0]).genome[i[1]] = (source(i[0] * 2).genome[i[1]] + source(i[0] * 2 + 1).genome[i[1]]) / 2;
                    } else {
                        result(i[0]).genome[i[1]] = source(i[0] * 2).genome[i[1]];
                    }
            });
        } else {
            auto _rand = random_array(count, length + 1);
            auto& rand = *_rand;
            
            parallel_for_each(result.accelerator_view, extent<2>(count, length),
                [=, &result, &source, &rand](index<2> i) restrict(amp) {
                    if (rand(i[0], length) < _per_pair_rate && (_per_gene_rate >= 1.0f || rand[i] < _per_gene_rate)) {
                        result(i[0]).genome[i[1]] = (source(i[0] * 2).genome[i[1]] + source(i[0] * 2 + 1).genome[i[1]]) / 2;
                    } else {
                        result(i[0]).genome[i[1]] = source(i[0] * 2).genome[i[1]];
                    }
            });
        }

        pResult.evaluate_using(src);
        return pResult;
    }
};

template<typename SourceType>
crossover_average_t<SourceType>
crossover_average(SourceType source, float per_pair_rate, float per_gene_rate, int genes) {
    return crossover_average_t<SourceType>(source, per_pair_rate, per_gene_rate, genes);
}
