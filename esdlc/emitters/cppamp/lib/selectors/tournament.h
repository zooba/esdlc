#pragma once

#include <amp.h>
#include <algorithm>
#include <list>
#include "bitonic_sort.h"
#include "individuals\individuals.h"
#include "operators\merge.h"

template<typename SourceType>
class tournament_t
{
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

    typename esdl::tt::group_type<SourceType>::type pSource;

    typedef typename bitonic_sort::key_index_type<typename esdl::tt::individual_type<SourceType>::type>::type KeyType;
    std::shared_ptr<concurrency::array<typename bitonic_sort::key_index_type<IndividualType>::type,1>> indices;

    int k;
    float greediness;
public:

    tournament_t(SourceType source, int k, float greediness) : pSource(source()), k(k), greediness(greediness) {
        pSource.evaluate();
    }

    esdl::group<IndividualType> take_all() {
        const int count = pSource.size();
        auto& src = *pSource;

        std::vector<IndividualType> src_list = pSource;
        std::vector<IndividualType> result_list;

        while ((int)src_list.size() > k) {
            const int size = src_list.size() - 1;
            int best_j = (int)(random_one() * size);
            if (random_one() < greediness) {
                for (int i = 1; i < k; ++i) {
                    int j = (int)(random_one() * size);
                    if (src_list[j] > src_list[best_j]) {
                        best_j = j;
                    }
                }
            }
            result_list.push_back(src_list[best_j]);
            src_list.erase(src_list.begin() + best_j);
        }

        while (src_list.size() > 0) {
            const int size = src_list.size() - 1;
            int j = (int)(random_one() * size);
            result_list.push_back(src_list[j]);
            src_list.erase(src_list.begin() + j);
        }

        auto pResult = esdl::make_group(result_list);
        pResult.evaluate_using(pSource);
        pResult.evaluated = true;
        return pResult;
    }

    esdl::group<IndividualType> operator()(int count) {
        auto _rand = random_array(count, k + 1);
        auto& rand = *_rand;
        
        auto pResult = esdl::make_group<IndividualType>(count);
        auto& src = *pSource;
        auto& result = *pResult;

        const int _k = k;
        const float _greediness = greediness;
        const float _size = (float)pSource.size();

        parallel_for_each(result.accelerator_view, result.extent, [&, _k, _greediness, _size](index<1> i) restrict(amp) {
            int winner = (int)(rand(i[0], 0) * _size);
            result[i] = src[winner];

            if (rand(i[0], _k) < _greediness) {
                for (int j = 1; j < _k; ++j) {
                    int competitor = (int)(rand(i[0], j) * _size);
                    
                    if (src[competitor] > result[i]) {
                        result[i] = src[competitor];
                    }
                }
            }
        });

        pResult.evaluate_using(pSource);
        pResult.evaluated = true;
        return pResult;
    }
};

template<typename SourceType>
tournament_t<SourceType>
tournament(SourceType source, int k, float greediness, std::true_type, std::false_type) {
    return tournament_t<SourceType>(source, k, greediness);
}

template<typename SourceType>
auto tournament(SourceType source, int k, float greediness, std::false_type, std::true_type) 
-> decltype(esdl::merge(tournament_t<SourceType>(source, k, greediness).take_all())) {
    return esdl::merge(tournament_t<SourceType>(source, k, greediness).take_all());
}

template<typename SourceType, typename same_bool>
auto tournament(SourceType source, int k, float greediness, same_bool, same_bool) 
-> decltype(esdl::merge(tournament_t<SourceType>(source, k, greediness).take_all())) {
    return esdl::merge(tournament_t<SourceType>(source, k, greediness).take_all());
}

