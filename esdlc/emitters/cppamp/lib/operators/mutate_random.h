#pragma once

#include "..\rng.h"
#include "..\utility.h"
#include "..\individuals\individuals.h"

template<typename SourceType>
class mutate_random_t {
    SourceType source;
    float per_indiv_rate, per_gene_rate;
    int genes;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

public:
    mutate_random_t(SourceType source, float per_indiv_rate, float per_gene_rate, int genes)
        : source(source), per_indiv_rate(per_indiv_rate), per_gene_rate(per_gene_rate), genes(genes) { }

    esdl::group<IndividualType> operator()(int count) { return mutate(source(count)); }
    esdl::group<IndividualType> operator()() { return mutate(source()); }

private:
    esdl::group<IndividualType> mutate(esdl::group<IndividualType> pSrc) {
        int count = pSrc.size();
        auto& src = *pSrc;
        auto pDest = make_group(pSrc);
        auto& dest = *pDest;

        const int length = esdl::tt::length<IndividualType>::value;
        const auto _per_indiv_rate = per_indiv_rate;
        const auto _per_gene_rate = (genes > 0) ? (float)genes / length : per_gene_rate;

        auto _rand = random_array(count, length + 1, 2);
        auto& rand = *_rand;

        parallel_for_each(dest.accelerator_view, grid<2>(extent<2>(count, length)),
            [=, &dest, &rand](index<2> i) restrict(direct3d) {
                index<3> i_i(i[0], length, 0), i_g(i[0], i[1], 0), i_r(i[0], i[1], 1);
                
                if (rand[i_i] < _per_indiv_rate && rand[i_g] < _per_gene_rate) {
                    dest(i[0]).genome[i[1]] = dest(i[0]).lowest + 
                        esdl::scale_value<typename esdl::tt::element_type<IndividualType>::type>::
                        scale(rand[i_r], dest(i[0]).highest, dest(i[0]).lowest);
                    dest(i[0]).fitness = 0;
                }
        });

        return pDest;
    }
};

template<typename SourceType>
mutate_random_t<SourceType> mutate_random(SourceType source, float per_indiv_rate, float per_gene_rate, int genes) {
    return mutate_random_t<SourceType>(source, per_indiv_rate, per_gene_rate, genes);
}
