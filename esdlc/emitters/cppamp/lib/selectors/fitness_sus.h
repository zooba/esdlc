#pragma once

#include <algorithm>
#include <list>
#include "individuals\individuals.h"

template<typename SourceType>
class fitness_sus_t
{
    int mu;

    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;

    typename esdl::tt::group_type<SourceType>::type pSource;

    typedef typename esdl_sort::key_index_type<typename esdl::tt::individual_type<SourceType>::type>::type KeyType;
    std::vector<KeyType> src_list;
    std::shared_ptr<EvaluatorType> evalptr;

    float p, p_next, p_step;
    int p_i;
public:

    fitness_sus_t(SourceType source, int mu) : mu(mu), pSource(source()) {
        pSource.evaluate();
        evalptr = pSource.evalptr;
        auto keys = esdl_sort::parallel_sort_keys(*pSource);
        src_list.reserve(keys->extent.size());
        concurrency::copy(*keys, std::back_inserter(src_list));
        
        float min_fitness = src_list.back().k;
        float sum = 0;
        std::for_each(std::begin(src_list), std::end(src_list), 
            [&sum, min_fitness](KeyType& x) { x.k -= min_fitness; sum += x.k; });

        const int _mu = (mu <= 0) ? src_list.size() : mu;

        p_step = sum / _mu;
        p = random_one() * p_step;
        p_i = 0;
        p_next = src_list[0].k;
    }

    esdl::group<IndividualType, EvaluatorType> operator()(int count) {
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

        //concurrency::array_view<const int, 1> indices(index_list.size(), index_list);
        concurrency::array<int, 1> indices(index_list.size(), std::begin(index_list), std::end(index_list), esdl::acc);
        auto pResult = esdl::make_group<IndividualType, EvaluatorType>(index_list.size(), evalptr);
        auto& src = *pSource;
        auto& result = *pResult;
        parallel_for_each(esdl::acc, result.grid, [&](index<1> i) restrict(direct3d) {
            result[i] = src[indices[i]];
        });
        return pResult;
    }
};

template<typename SourceType>
fitness_sus_t<SourceType> fitness_sus(SourceType source, int mu) {
    return fitness_sus_t<SourceType>(source, mu);
}
