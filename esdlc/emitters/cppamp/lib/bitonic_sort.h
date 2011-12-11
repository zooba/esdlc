#pragma once

#include <amp.h>

template<typename KeyType>
struct KeyIndex
{
    KeyType k;
    int i;

    KeyIndex() restrict(cpu, direct3d) { }
    KeyIndex(KeyType key, int index) restrict(cpu, direct3d) : k(key), i(index) { }

    bool operator<(const KeyIndex<KeyType>& other) const restrict(cpu, direct3d) {
        return k < other.k;
    }
};

template<typename T>
struct key_index_type { typedef KeyIndex<T> type; };
template<> struct key_index_type<float> { typedef float type; };
template<> struct key_index_type<int> { typedef int type; };
template<> struct key_index_type<unsigned int> { typedef unsigned int type; };
template<> struct key_index_type<double> { typedef KeyIndex<double> type; };

namespace details
{
    template<bool First, typename KeyType>
    void parallel_sort_pass(int virtual_size, int block_size, concurrency::array<KeyType, 1>& dest) {
        const int actual_size = dest.extent.size();
        const int half_block_size = block_size / 2;
        const int mask = block_size - 1;
        const int half_mask = half_block_size - 1;
        concurrency::parallel_for_each(dest.accelerator_view,
            concurrency::grid<1>(concurrency::extent<1>(virtual_size / 2)),
            [=, &dest](index<1> i) restrict(direct3d) {
            const int i1 = ((i[0] & ~half_mask) << 1) | (i[0] & half_mask);
            const int i2 = (First) ? ((i1 | mask) - (i1 & mask)) : (i1 + half_block_size);

            if (i2 < actual_size && !(dest[i1] < dest[i2])) {
                auto temp = dest[i1];
                dest[i1] = dest[i2];
                dest[i2] = temp;
            }
        });
    }
}

template<typename SourceType>
typename std::enable_if<!std::is_same<typename key_index_type<SourceType>::type, SourceType>::value, std::shared_ptr<concurrency::array<typename key_index_type<SourceType>::type, 1>>>::type
parallel_sort_keys(const concurrency::array<SourceType, 1>& source) {
    const int actual_size = source.extent.size();
    int virtual_size = 1, stage_count = 0;
    while (virtual_size < actual_size) {
        virtual_size *= 2;
        stage_count += 1;
    }

    typedef typename key_index_type<SourceType>::type Key;
    auto pKeys = std::make_shared<concurrency::array<Key, 1>>(actual_size, source.accelerator_view);
    auto& keys = *pKeys;
    concurrency::parallel_for_each(keys.accelerator_view, keys.grid,
        [=, &keys, &source](concurrency::index<1> i) restrict(direct3d) {
        keys[i] = Key(source[i], i[0]);
    });

    int starting_block_size = 2;
    while (--stage_count >= 0) {
        int block_size = starting_block_size;
        starting_block_size *= 2;

        details::parallel_sort_pass<true>(virtual_size, block_size, keys);
        for (block_size /= 2; block_size >= 2; block_size /= 2) {
            details::parallel_sort_pass<false>(virtual_size, block_size, keys);
        }
    }

    return pKeys;
}

template<typename SourceType>
typename std::enable_if<std::is_same<typename key_index_type<SourceType>::type, SourceType>::value, std::shared_ptr<concurrency::array<SourceType, 1>>>::type
parallel_sort_keys(const concurrency::array<SourceType, 1>& source) {
    const int actual_size = source.extent.size();
    int virtual_size = 1, stage_count = 0;
    while (virtual_size < actual_size) {
        virtual_size *= 2;
        stage_count += 1;
    }

    auto pKeys = std::make_shared<concurrency::array<SourceType, 1>>(source);
    auto& keys = *pKeys;

    int starting_block_size = 2;
    while (--stage_count >= 0) {
        int block_size = starting_block_size;
        starting_block_size *= 2;

        details::parallel_sort_pass<true>(virtual_size, block_size, keys);
        for (block_size /= 2; block_size >= 2; block_size /= 2) {
            details::parallel_sort_pass<false>(virtual_size, block_size, keys);
        }
    }

    return pKeys;
}

template<typename SourceType>
typename std::enable_if<!std::is_same<typename key_index_type<SourceType>::type, SourceType>::value, std::shared_ptr<concurrency::array<SourceType, 1>>>::type
parallel_sort(const concurrency::array<SourceType, 1>& source, bool reverse=false) {
    auto pKeys = parallel_sort_keys(source);
    auto& keys = *pKeys;

    auto pResult = std::make_shared<concurrency::array<SourceType, 1>>(source.extent.size(), source.accelerator_view);
    auto& result = *pResult;
    if (!reverse) {
        concurrency::parallel_for_each(result.accelerator_view, result.grid,
            [&](concurrency::index<1> i) restrict(direct3d) {
            result[i] = source[keys[i].i];
        });
    } else {
        concurrency::parallel_for_each(result.accelerator_view, result.grid,
            [&](concurrency::index<1> i) restrict(direct3d) {
            result[i] = source[keys[keys.extent.size() - 1 - i].i];
        });
    }
    return pResult;
}

template<typename SourceType>
typename std::enable_if<std::is_same<typename key_index_type<SourceType>::type, SourceType>::value, std::shared_ptr<concurrency::array<SourceType, 1>>>::type
parallel_sort(const concurrency::array<SourceType, 1>& source, bool reverse=false) {
    auto pKeys = parallel_sort_keys(source);
    auto& keys = *pKeys;

    if (!reverse) {
        return pKeys;
    } else {
        auto pResult = std::make_shared<concurrency::array<SourceType, 1>>(source.extent.size(), source.accelerator_view);
        auto& result = *pResult;
        concurrency::parallel_for_each(result.accelerator_view, result.grid,
            [&](concurrency::index<1> i) restrict(direct3d) {
            result[i] = keys[keys.extent.size() - 1 - i];
        });
        return pResult;
    }
}
