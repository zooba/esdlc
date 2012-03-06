#pragma once

#include <algorithm>
#include <list>
#include "bitonic_sort.h"
#include "individuals\individuals.h"
#include "operators\merge.h"

template<typename SourceType>
class rank_proportional_t
{
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename bitonic_sort::key_index_type<IndividualType>::type KeyType;

    typename esdl::tt::group_type<SourceType>::type pSource;
    std::shared_ptr<concurrency::array<KeyType, 1>> indices;

    float total;

public:
    rank_proportional_t(SourceType source, float expectation, bool invert) : pSource(source()) {
        pSource.evaluate();
        indices = bitonic_sort::parallel_sort_keys(*pSource);
        auto& keys = *indices;

        const int _length = keys.extent.size();
        const float _scale = expectation - 2.0f * (expectation - 1) / (_length - 1.0f);
        total = expectation * _length - _scale * (_length * (_length + 1)) * 0.5f;
        if (!invert) {
            parallel_for_each(keys.accelerator_view, keys.extent, [=, &keys](index<1> i) restrict(amp) {
                keys[i].k = expectation - _scale * i[0];
            });
        } else {
            parallel_for_each(keys.accelerator_view, keys.extent, [=, &keys](index<1> i) restrict(amp) {
                keys[i].k = expectation - _scale * (_length - i[0] - 1);
            });
        }
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

        const float _total = total;

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
rank_proportional_t<SourceType>
rank_proportional(
    SourceType source,
    float expectation,
    bool invert,
    std::true_type, std::false_type) {
    return rank_proportional_t<SourceType>(source, expectation, invert);
}

template<typename SourceType>
auto rank_proportional(
    SourceType source,
    float expectation,
    bool invert,
    std::false_type, std::true_type)
-> decltype(esdl::merge(rank_proportional_t<SourceType>(source, expectation, invert).take_all())) {
    return esdl::merge(rank_proportional_t<SourceType>(source, expectation, invert).take_all());
}

template<typename SourceType, typename same_bool>
auto rank_proportional(
    SourceType source,
    float expectation,
    bool invert,
    same_bool, same_bool)
-> decltype(esdl::merge(rank_proportional_t<SourceType>(source, expectation, invert).take_all())) {
    return esdl::merge(rank_proportional_t<SourceType>(source, expectation, invert).take_all());
}

