#pragma once

#include "..\utility.h"
#include "..\individuals\individuals.h"

template<typename SourceType>
class mutate_gaussian_t {
    SourceType source;
    float per_indiv_rate, per_gene_rate;
    int genes;
    float mean, sigma;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;
public:

    mutate_gaussian_t(SourceType source, float mean, float sigma, float per_indiv_rate, float per_gene_rate, int genes)
        : source(source), mean(mean), sigma(sigma), per_indiv_rate(per_indiv_rate), per_gene_rate(per_gene_rate), genes(genes)
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
        const float _sigma = sigma;
        const float _mean = mean;

        auto _rand2 = random_normal_array(count, length);
        auto& rand2 = *_rand2;

        if (_per_indiv_rate >= 1.0f && _per_gene_rate >= 1.0f) {
            parallel_for_each(dest.accelerator_view, grid<2>(extent<2>(count, length)),
                [=, &dest, &rand2](index<2> i) restrict(direct3d) {
                    dest(i[0]).genome[i[1]] += (esdl::tt::element_type<IndividualType>::type)(rand2[i] * _sigma + _mean);
                    dest(i[0]).fitness = 0;
            });
        } else if (_per_indiv_rate >= 1.0f) {
            auto _rand1 = random_array(count, length);
            auto& rand1 = *_rand1;

            parallel_for_each(dest.accelerator_view, grid<2>(extent<2>(count, length)),
                [=, &dest, &rand1, &rand2](index<2> i) restrict(direct3d) {
                    if (rand1[i] < _per_gene_rate) {
                        dest(i[0]).genome[i[1]] += (esdl::tt::element_type<IndividualType>::type)(rand2[i] * _sigma + _mean);
                        dest(i[0]).fitness = 0;
                    }
            });
        } else {
            // TODO: Fix apparent bias in rand1
            auto _rand1 = random_array(count, length + 1);
            auto& rand1 = *_rand1;

            parallel_for_each(dest.accelerator_view, grid<2>(extent<2>(count, length)),
                [=, &dest, &rand1, &rand2](index<2> i) restrict(direct3d) {
                    if (rand1(i[0], length) < _per_indiv_rate && (_per_gene_rate >= 1.0f || rand1[i] < _per_gene_rate)) {
                        dest(i[0]).genome[i[1]] += (esdl::tt::element_type<IndividualType>::type)(rand2[i] * _sigma + _mean);
                        dest(i[0]).fitness = 0;
                    }
            });
        }

        return pDest;
    }
};

template<typename SourceType>
mutate_gaussian_t<SourceType> mutate_gaussian(SourceType source, float mean, float sigma, float per_indiv_rate, float per_gene_rate, int genes) {
    return mutate_gaussian_t<SourceType>(source, mean, sigma, per_indiv_rate, per_gene_rate, genes);
}
