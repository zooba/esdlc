#pragma once

#include <algorithm>
#include <list>
#include "individuals\individuals.h"

template<typename SourceType>
class rank_sus_t
{
    int mu;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename bitonic_sort::key_index_type<IndividualType>::type KeyType;

    typename esdl::tt::group_type<SourceType>::type pSource;
    std::vector<KeyType> src_list;

    float p, p_next, p_step;
    int p_i;
public:

    rank_sus_t(SourceType source, float expectation, int mu) : mu(mu), pSource(source()) {
        pSource.evaluate();
        auto pKeys = bitonic_sort::parallel_sort_keys(*pSource);
        auto& keys = *pKeys;
        
        const int _length = keys.extent.size();
        const float _scale = expectation - 2.0f * (expectation - 1) / (_length - 1.0f);
        parallel_for_each(keys.accelerator_view, keys.extent, [=, &keys](index<1> i) restrict(amp) {
            keys[i].k = expectation - _scale * i[0];
        });
        
        src_list.reserve(keys.extent.size());
        concurrency::copy(keys, std::back_inserter(src_list));
        
        float sum = 0;
        std::for_each(std::begin(src_list), std::end(src_list), [&sum](KeyType& x) { sum += x.k; });

        const int _mu = (mu <= 0) ? src_list.size() : mu;

        p_step = sum / _mu;
        p = random_one() * p_step;
        p_i = 0;
        p_next = src_list[0].k;
    }

    esdl::group<IndividualType> operator()(int count) {
        std::vector<int> index_list;
        index_list.reserve(count);

        while (index_list.size() < (unsigned int)count) {
            while (p > p_next) {
                p_i = (p_i + 1) % src_list.size();
                p_next += src_list[p_i].k;
            }
            p += p_step;
            index_list.push_back(src_list[p_i].i);
        }

        auto pResult = esdl::make_group<IndividualType>(index_list.size());
        auto& src = *pSource;
        auto& result = *pResult;
        concurrency::array<int, 1> indices(index_list.size(), std::begin(index_list), std::end(index_list), src.accelerator_view);
        parallel_for_each(src.accelerator_view, result.extent, [&](index<1> i) restrict(amp) {
            result[i] = src[indices[i]];
        });
        pResult.evaluate_using(pSource);
        return pResult;
    }
};

template<typename SourceType>
rank_sus_t<SourceType> rank_sus(
    SourceType source,
    float expectation,
    int mu) {
    return rank_sus_t<SourceType>(source, expectation, mu);
}
