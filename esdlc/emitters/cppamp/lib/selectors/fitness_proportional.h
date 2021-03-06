#pragma once

#include <algorithm>
#include <list>
#include "bitonic_sort.h"
#include "individuals\individuals.h"
#include "operators\merge.h"

template<typename SourceType>
class fitness_proportional_t
{
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;

    typename esdl::tt::group_type<SourceType>::type pSource;

    typedef typename bitonic_sort::key_index_type<IndividualType>::type KeyType;
    std::shared_ptr<concurrency::array<KeyType, 1>> indices;

    float total_fitness;
    bool need_total_fitness;

    void init_total_fitness() {
        if (!need_total_fitness) {
            return;
        }

        std::vector<KeyType> key_list;
        concurrency::copy(*indices, std::back_inserter(key_list));
        float _total_fitness = 0;
        std::for_each(std::begin(key_list), std::end(key_list),
            [&](const KeyType& i) {
            _total_fitness += i.k;
        });
        total_fitness = _total_fitness;

        need_total_fitness = false;
    }
public:

    fitness_proportional_t(SourceType source) : pSource(source()) {
        pSource.evaluate();
        indices = bitonic_sort::parallel_sort_keys(*pSource);
        auto& keys = *indices;

        concurrency::array<float, 1> offset(keys.extent, keys.accelerator_view);
        parallel_for_each(keys.accelerator_view, keys.extent, [&](index<1> i) restrict(amp) {
            offset[i] = keys[keys.extent.size() - 1].k;
        });
        parallel_for_each(keys.accelerator_view, keys.extent, [&](index<1> i) restrict(amp) {
            keys[i].k -= offset[i];
        });

        need_total_fitness = true;
    }

    fitness_proportional_t(SourceType source, const concurrency::array<IndividualType, 1>& offset) : pSource(source()) {
        pSource.evaluate();
        indices = bitonic_sort::parallel_sort_keys(*pSource);
        auto& keys = *indices;
        
        parallel_for_each(keys.accelerator_view, keys.extent, [&](index<1> i) restrict(amp) {
            keys[i].k -= offset[0].fitness;
        });

        need_total_fitness = true;
    }

    esdl::group<IndividualType> take_all() {
        const int count = indices->extent.size();
        auto _rand = random_array(count);
        auto& rand = *_rand;
        auto& keys = *indices;

        auto pResult = esdl::make_group<IndividualType>(count);
        auto& src = *pSource;
        auto& result = *pResult;

        parallel_for_each(keys.accelerator_view, keys.extent, [&](index<1> i) restrict(amp) {
            keys[i].k *= rand[i];
        });

        auto new_indices = bitonic_sort::parallel_sort(keys);
        auto& new_keys = *new_indices;

        parallel_for_each(result.accelerator_view, result.extent, [&](index<1> i) restrict(amp) {
            result[i] = src[new_keys[i].i];
        });

        pResult.evaluate_using(pSource);
        return pResult;
    }

    esdl::group<IndividualType> operator()(int count) {
        auto _rand = random_array(count);
        auto& rand = *_rand;
        auto& keys = *indices;

        auto pResult = esdl::make_group<IndividualType>(count);
        auto& src = *pSource;
        auto& result = *pResult;

        init_total_fitness();
        const float _total = total_fitness;

        parallel_for_each(src.accelerator_view, result.extent, [&, _total](index<1> i) restrict(amp) {
            float p = rand[i] * _total;
            for (unsigned int j = 0; p > 0 && j < rand.extent.size(); ++j) {
                p -= keys[j].k;
                if (p <= 0) {
                    result[i] = src[keys[j].i];
                }
            }
        });

        pResult.evaluate_using(pSource);
        return pResult;
    }
};

template<typename SourceType>
fitness_proportional_t<SourceType>
fitness_proportional(
    SourceType source,
    esdl::group<typename esdl::tt::individual_type<SourceType>::type> offset,
    std::true_type, std::false_type) {
    return fitness_proportional_t<SourceType>(source, *offset);
}

template<typename SourceType>
fitness_proportional_t<SourceType>
fitness_proportional(
    SourceType source,
    nullptr_t offset,
    std::true_type, std::false_type) {
    return fitness_proportional_t<SourceType>(source);
}

template<typename SourceType>
auto fitness_proportional(
    SourceType source,
    esdl::group<typename esdl::tt::individual_type<SourceType>::type> offset,
    std::false_type, std::true_type)
-> decltype(esdl::merge(fitness_proportional_t<SourceType>(source, *offset).take_all())) {
    return esdl::merge(fitness_proportional_t<SourceType>(source, *offset).take_all());
}

template<typename SourceType>
auto fitness_proportional(
    SourceType source,
    nullptr_t offset,
    std::false_type, std::true_type) 
-> decltype(esdl::merge(fitness_proportional_t<SourceType>(source).take_all())) {
    return esdl::merge(fitness_proportional_t<SourceType>(source).take_all());
}


template<typename SourceType, typename same_bool>
auto fitness_proportional(
    SourceType source,
    esdl::group<typename esdl::tt::individual_type<SourceType>::type> offset,
    same_bool, same_bool)
-> decltype(esdl::merge(fitness_proportional_t<SourceType>(source, *offset).take_all())) {
    return esdl::merge(fitness_proportional_t<SourceType>(source, *offset).take_all());
}

template<typename SourceType, typename same_bool>
auto fitness_proportional(
    SourceType source,
    nullptr_t offset,
    same_bool, same_bool) 
-> decltype(esdl::merge(fitness_proportional_t<SourceType>(source).take_all())) {
    return esdl::merge(fitness_proportional_t<SourceType>(source).take_all());
}

