#pragma once

#include "..\rng.h"
#include "..\utility.h"
#include "..\individuals\individuals.h"
#include "..\individuals\joined_individuals.h"

template<typename SourceType>
class crossover_tuple2_t
{
    SourceType source;
    float per_pair_rate, per_gene_rate;
    typedef typename esdl::tt::joined_individual_type<SourceType, 0>::type IndividualType;
    static_assert(esdl::tt::joined_count<SourceType>::value == 2, "crossover_tuple requires two joined individuals");
    static_assert(std::is_same<typename esdl::tt::joined_individual_type<SourceType, 0>::type, typename esdl::tt::joined_individual_type<SourceType, 1>::type>::value,
        "crossover_tuple requires joined individuals to be the same type");
public:

    crossover_tuple2_t(SourceType source, float per_pair_rate, float per_gene_rate)
        : source(source), per_pair_rate(per_pair_rate), per_gene_rate(per_gene_rate) { }

    esdl::group<IndividualType> operator()() {
        return exec(source());
    }

    esdl::group<IndividualType> operator()(int count) {
        return exec(source(count));
    }

private:
    esdl::group<IndividualType> exec(const typename esdl::tt::group_type<SourceType>::type& src) {
        int count = src.size();

        auto& s1 = *src.group1;
        auto& s2 = *src.group2;
        auto pResult = esdl::make_group<IndividualType>(src.group1);
        auto& dest = *pResult;

        const int length = esdl::tt::length<IndividualType>::value;

        if (per_pair_rate >= 1.0) {
            const float _per_gene_rate = per_gene_rate;
            auto _rand = random_array(count, length);
            auto& rand = *_rand;
            parallel_for_each(dest.accelerator_view, extent<2>(count, length),
                [&, _per_gene_rate](index<2> i) restrict(amp) {
                    dest(i[0]).genome[i[1]] = (rand[i] < _per_gene_rate) ? s1(i[0]).genome[i[1]] : s2(i[0]).genome[i[1]];
            });
        } else {
            const float _per_pair_rate = per_pair_rate;
            const float _per_gene_rate = per_gene_rate;
            auto _rand = random_array(count, length + 1);
            auto& rand = *_rand;
            parallel_for_each(dest.accelerator_view, extent<2>(count, length),
                [&, _per_pair_rate, _per_gene_rate, length](index<2> i) restrict(amp) {
                    dest(i[0]).genome[i[1]] = (rand(i[0], length) >= _per_pair_rate || rand[i] < _per_gene_rate) ? 
                        s1(i[0]).genome[i[1]] : 
                        s2(i[0]).genome[i[1]];
            });
        }

        pResult.evaluate_using(src.group1);
        return pResult;
    }
};


template<typename SourceType>
typename std::enable_if<esdl::tt::joined_count<SourceType>::value == 2, crossover_tuple2_t<SourceType>>::type
crossover_tuple(SourceType source, float per_pair_rate, float per_gene_rate) {
    return crossover_tuple2_t<SourceType>(source, per_pair_rate, per_gene_rate);
}
