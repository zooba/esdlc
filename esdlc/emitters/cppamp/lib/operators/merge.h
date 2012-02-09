#pragma once

#include <algorithm>
#include <memory>

#include <amp.h>

#include "individuals\individuals.h"
#include "individuals\joined_individuals.h"

namespace esdl
{
    template<typename GroupType, int Count>
    class Merger_t {
        typedef typename tt::individual_type<GroupType>::type IndividualType;

        GroupType groups[Count];
        int upToGroup, upToIndividual, availableCount;

    public:
        Merger_t() : upToGroup(0), upToIndividual(0), availableCount(0) { }

        Merger_t(GroupType group)
            : upToGroup(0), upToIndividual(0), availableCount(group.size())
        {
            static_assert(Count == 1, "Must provide array for Count != 1");
            groups[0] = group;
        }

        Merger_t(GroupType (&group_array)[Count])
            : upToGroup(0), upToIndividual(0), availableCount(0)
        {
            for (int i = 0; i < Count; ++i) {
                groups[i] = group_array[i];
                availableCount += group_array[i].size();
            }
        }

        GroupType operator()(int count)
        {
            if (count > availableCount) count = availableCount;

            if (count == 0) return make_group<IndividualType>();

            auto pStream = make_group<IndividualType>(count);
            auto& dest = *pStream;

            for (int i = 0; i < count; ) {
                int len = groups[upToGroup]->extent.size() - upToIndividual;
                if (i + len > count) len = count - i;

                auto& src = *groups[upToGroup];
                const int j = upToIndividual;
                parallel_for_each(src.accelerator_view, grid<1>(extent<1>(len)), [&, i, j](index<1> k) restrict(direct3d) {
                    dest[k+i] = src[k+j];
                });

                i += len;
                upToIndividual += len;
                if (upToIndividual >= groups[upToGroup].size()) {
                    upToIndividual -= groups[upToGroup].size();
                    upToGroup += 1;
                }
            }

            availableCount -= pStream.size();
            pStream.evaluate_using(groups[0]);
            return pStream;
        }

        GroupType operator()()
        {
            if (Count == 1 && upToIndividual == 0) {
                upToIndividual = -1;
                availableCount = 0;
                return groups[0];
            }
            return (*this)(0x7FFFFFFF);
        }
    };

    template<typename GroupType, int Count, typename Generator>
    struct MergerWithGenerator_t {
        typedef typename GroupType::IndividualType IndividualType;

        GroupType groups[Count];
        Generator generator;
        int upToGroup, upToIndividual;

        MergerWithGenerator_t(GroupType group, Generator generator)
            : upToGroup(0), upToIndividual(0), generator(generator)
        {
            static_assert(Count == 1, "Must provide array for Count != 1");
            groups[0] = group;
        }

        MergerWithGenerator_t(GroupType (&group_array)[Count], Generator generator)
            : upToGroup(0), upToIndividual(0), generator(generator)
        {
            for (int i = 0; i < Count; ++i) {
                groups[i] = group_array[i];
            }
        }

        GroupType operator()(int count)
        {
            if (count <= 0) return make_group<IndividualType>();

            auto pStream = make_group<IndividualType>(count);
            auto& dest = *pStream;

            for (int i = 0; i < count; ) {
                if (upToGroup >= Count) {
                    int len = count - i;

                    auto pSrc = generator(len);
                    auto& src = *pSrc;
                    parallel_for_each(src.accelerator_view, grid<1>(extent<1>(len)), [&, i](index<1> k) restrict(direct3d) {
                        dest[k+i] = src[k];
                    });

                    i += len;
                } else {
                    int len = groups[upToGroup]->extent.size() - upToIndividual;
                    if (i + len > count) len = count - i;

                    auto& src = *groups[upToGroup];
                    const int j = upToIndividual;
                    parallel_for_each(src.accelerator_view, grid<1>(extent<1>(len)), [&, i, j](index<1> k) restrict(direct3d) {
                        dest[k+i] = src[k+j];
                    });

                    i += len;
                    upToIndividual += len;
                    if (upToIndividual >= groups[upToGroup].size()) {
                        upToIndividual -= groups[upToGroup].size();
                        upToGroup += 1;
                    }
                }
            }

            availableCount -= pStream.size();
            pStream.evaluate_using(group[0]);
            return pStream;
        }
    };

    template<typename GroupType, int Count, typename GeneratorType>
    typename std::enable_if<!esdl::tt::is_joined_group<GroupType>::value, MergerWithGenerator_t<GroupType, Count, GeneratorType>>::type
    merge(GroupType (&groups)[Count], GeneratorType source)
    {
        return MergerWithGenerator_t<GroupType, Count, GeneratorType>(groups, source);
    }

    template<typename GroupType, int Count>
    typename std::enable_if<!esdl::tt::is_joined_group<GroupType>::value, Merger_t<GroupType, Count>>::type
    merge(GroupType (&groups)[Count])
    {
        return Merger_t<GroupType, Count>(groups);
    }

    template<typename GroupType, typename GeneratorType>
    typename std::enable_if<!esdl::tt::is_joined_group<GroupType>::value, MergerWithGenerator_t<GroupType, 1, GeneratorType>>::type
    merge(GroupType group, GeneratorType source)
    {
        return MergerWithGenerator_t<GroupType, 1, GeneratorType>(group, source);
    }

    template<typename GroupType>
    typename std::enable_if<!esdl::tt::is_joined_group<GroupType>::value, Merger_t<GroupType, 1>>::type
    merge(GroupType group)
    {
        return Merger_t<GroupType, 1>(group);
    }


    namespace tt
    {
        template<typename GroupType, int Count>
            struct individual_type<Merger_t<GroupType, Count>>
            { typedef typename individual_type<GroupType>::type type; };
        template<typename GroupType, int Count, typename GeneratorType>
            struct individual_type<MergerWithGenerator_t<GroupType, Count, GeneratorType>>
            { typedef typename individual_type<GroupType>::type type; };
    }


    template<int JoinCount, typename GroupType, int Count>
    struct JoinedMerger_t;

    template<typename GroupType, int Count>
    struct JoinedMerger_t<2, GroupType, Count> {
        typename Merger_t<typename esdl::tt::joined_group_type<GroupType, 0>::type, Count> source1;
        typename Merger_t<typename esdl::tt::joined_group_type<GroupType, 1>::type, Count> source2;
        
        JoinedMerger_t(GroupType group)
        {
            static_assert(Count == 1, "Must provide array for Count != 1");
            source1 = esdl::merge(group.group1);
            source2 = esdl::merge(group.group2);
        }

        JoinedMerger_t(GroupType (&group_array)[Count])
        {
            typename esdl::tt::joined_group_type<GroupType, 0>::type groups1[Count];
            typename esdl::tt::joined_group_type<GroupType, 1>::type groups2[Count];

            for (int i = 0; i < Count; ++i) {
                groups1[i] = group_array[i].group1;
                groups2[i] = group_array[i].group2;
            }

            source1 = esdl::merge(groups1);
            source2 = esdl::merge(groups2);
        }

        GroupType operator()(int count)
        {
            return GroupType(source1(count), source2(count));
        }

        GroupType operator()()
        {
            return GroupType(source1(), source2());
        }
    };

    template<typename GroupType, int Count>
    struct JoinedMerger_t<3, GroupType, Count> {
        typename Merger_t<typename esdl::tt::joined_group_type<GroupType, 0>::type, Count> source1;
        typename Merger_t<typename esdl::tt::joined_group_type<GroupType, 1>::type, Count> source2;
        typename Merger_t<typename esdl::tt::joined_group_type<GroupType, 2>::type, Count> source3;
        
        JoinedMerger_t(GroupType group)
        {
            static_assert(Count == 1, "Must provide array for Count != 1");
            source1 = esdl::merge(group.group1);
            source2 = esdl::merge(group.group2);
            source3 = esdl::merge(group.group3);
        }

        JoinedMerger_t(GroupType (&group_array)[Count])
        {
            typename esdl::tt::joined_group_type<GroupType, 0>::type groups1[Count];
            typename esdl::tt::joined_group_type<GroupType, 1>::type groups2[Count];
            typename esdl::tt::joined_group_type<GroupType, 2>::type groups3[Count];

            for (int i = 0; i < Count; ++i) {
                groups1[i] = group_array[i].group1;
                groups2[i] = group_array[i].group2;
                groups3[i] = group_array[i].group3;
            }

            source1 = esdl::merge(groups1);
            source2 = esdl::merge(groups2);
            source3 = esdl::merge(groups3);
        }

        GroupType operator()(int count)
        {
            return GroupType(source1(count), source2(count), source3(count));
        }

        GroupType operator()()
        {
            return GroupType(source1(), source2(), source3());
        }
    };

    template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4, int Count>
    JoinedMerger_t<JoinCount, joinedgroup<JoinCount, G0, G1, G2, G3, G4>, Count>
    merge(joinedgroup<JoinCount, G0, G1, G2, G3, G4> (&groups)[Count])
    {
        return JoinedMerger_t<JoinCount, joinedgroup<JoinCount, G0, G1, G2, G3, G4>, Count>(groups);
    }

    template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4>
    JoinedMerger_t<JoinCount, joinedgroup<JoinCount, G0, G1, G2, G3, G4>, 1>
    merge(const joinedgroup<JoinCount, G0, G1, G2, G3, G4>& group)
    {
        return JoinedMerger_t<JoinCount, joinedgroup<JoinCount, G0, G1, G2, G3, G4>, 1>(group);
    }

    namespace tt
    {
        template<int JoinCount, typename GroupType, int Count>
            struct joined_count<esdl::JoinedMerger_t<JoinCount, GroupType, Count>> : std::integral_constant<int, JoinCount> { };
        template<int JoinCount, typename GroupType, int Count, int Index>
            struct joined_group_type<esdl::JoinedMerger_t<JoinCount, GroupType, Count>, Index>
            { typedef typename joined_group_type<GroupType, Index>::type type; };
    }
}
