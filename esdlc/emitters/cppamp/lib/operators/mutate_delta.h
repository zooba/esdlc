#pragma once

#include "..\utility.h"
#include "..\individuals\individuals.h"

template<typename SourceType>
class mutate_delta_t {
    SourceType source;
    float per_indiv_rate, per_gene_rate;
    int genes;
    float step_size, positive_rate;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;
public:

    mutate_delta_t(SourceType source, float step_size, float positive_rate, float per_indiv_rate, float per_gene_rate, int genes)
        : source(source), step_size(step_size), positive_rate(positive_rate), per_indiv_rate(per_indiv_rate), per_gene_rate(per_gene_rate), genes(genes)
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
        const float _positive_rate = positive_rate;
        const typename esdl::tt::element_type<IndividualType>::type _step_size = 
            (typename esdl::tt::element_type<IndividualType>::type)step_size;

        auto _rand = random_array(count, length + 1, 2);
        auto& rand = *_rand;

        parallel_for_each(esdl::acc, grid<2>(extent<2>(count, length)),
            [=, &dest, &rand](index<2> i) restrict(direct3d) {
                index<3> i_i(i[0], length, 0), i_g(i[0], i[1], 0), i_p(i[0], i[1], 1);
                
                if (rand[i_i] < _per_indiv_rate && rand[i_g] < _per_gene_rate) {
                    if (rand[i_p] < _positive_rate) {
                        dest(i[0]).genome[i[1]] += _step_size;
                    } else {
                        dest(i[0]).genome[i[1]] -= _step_size;
                    }
                    dest(i[0]).fitness = 0;
                }
        });

        return pDest;
    }
};

template<typename SourceType>
mutate_delta_t<SourceType> mutate_delta(SourceType source, float step_size, float positive_rate, float per_indiv_rate, float per_gene_rate, int genes) {
    return mutate_delta_t<SourceType>(source, step_size, positive_rate, per_indiv_rate, per_gene_rate, genes);
}
