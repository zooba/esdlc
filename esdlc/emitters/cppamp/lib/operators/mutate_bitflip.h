#pragma once

#include "..\utility.h"
#include "..\individuals\individuals.h"
#include "..\individuals\bin_fixed.h"

template<typename SourceType>
class mutate_bitflip_t {
    SourceType source;
    float per_indiv_rate, per_gene_rate;
    int genes;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;
public:

    mutate_bitflip_t(SourceType source, float per_indiv_rate, float per_gene_rate, int genes)
        : source(source), per_indiv_rate(per_indiv_rate), per_gene_rate(per_gene_rate), genes(genes)
    { }

    esdl::group<IndividualType, EvaluatorType> operator()(int count) { return mutate(source(count)); }
    esdl::group<IndividualType, EvaluatorType> operator()() { return mutate(source()); }

private:
    esdl::group<IndividualType, EvaluatorType> mutate(esdl::group<IndividualType, EvaluatorType> pSrc) {
        int count = pSrc.size();
        auto& src = *pSrc;
        auto pDest = make_group(pSrc);
        auto& dest = *pDest;

        const int length = esdl::tt::length<IndividualType>::value;
        const float _per_indiv_rate = per_indiv_rate;
        const float _per_gene_rate = (genes > 0) ? (float)genes / length : per_gene_rate;
        
        if (_per_indiv_rate >= 1.0f && _per_gene_rate >= 1.0f) {
            parallel_for_each(src.accelerator_view, grid<2>(extent<2>(count, length)),
                [=, &dest](index<2> i) restrict(direct3d) {
                    dest(i[0]).genome[i[1]] = (dest(i[0]).genome[i[1]] == 0) ? 1 : 0;
                    dest(i[0]).fitness = 0;
            });
        } else if (_per_indiv_rate >= 1.0f) {
            auto _rand = random_array(count, length);
            auto& rand = *_rand;

            parallel_for_each(src.accelerator_view, grid<2>(extent<2>(count, length)),
                [=, &dest, &rand](index<2> i) restrict(direct3d) {
                    if (rand[i] < _per_gene_rate) {
                        dest(i[0]).genome[i[1]] = (dest(i[0]).genome[i[1]] == 0) ? 1 : 0;
                        dest(i[0]).fitness = 0;
                    }
            });
        } else {
            auto _rand = random_array(count, length + 1);
            auto& rand = *_rand;

            parallel_for_each(src.accelerator_view, grid<2>(extent<2>(count, length)),
                [=, &dest, &rand](index<2> i) restrict(direct3d) {
                    if (rand(i[0], length) < _per_indiv_rate && (_per_gene_rate >= 1.0f || rand[i] < _per_gene_rate)) {
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
    mutate_bitflip_t<SourceType>>::type
mutate_bitflip(SourceType source, float per_indiv_rate, float per_gene_rate, int genes) {
    return mutate_bitflip_t<SourceType>(source, per_indiv_rate, per_gene_rate, genes);
}