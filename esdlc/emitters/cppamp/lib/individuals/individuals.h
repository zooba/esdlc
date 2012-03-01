#pragma once

#include "sort.h"
#include <memory>
#include <list>
#include <vector>

#include "groups.h"

namespace esdl
{
    template<typename ElementType, int Length>
    struct fixed_individual {
        ElementType genome[Length];
        float fitness;

        void operator=(const fixed_individual<ElementType, Length>& other) restrict(cpu, amp)
        {
            for (int i = 0; i < Length; ++i) {
                genome[i] = other.genome[i];
            }
            fitness = other.fitness;
        }
    };

    template<typename ElementType, int Shortest, int Longest>
    struct variable_individual {
        ElementType genome[Longest];
        float fitness;
        int length;

        variable_individual<ElementType, Shortest, Longest>& operator=(const variable_individual<ElementType, Shortest, Longest>& other) restrict(amp)
        {
            for (int i = 0; i < length; ++i) {
                genome[i] = other.genome[i];
            }
            fitness = other.fitness;
            length = other.length;
            return *this;
        }
    };

    namespace tt
    {
        template<typename T>
            struct is_individual : std::false_type { };
        template<typename ElementType, int Length>
            struct is_individual<esdl::fixed_individual<ElementType, Length>> : std::true_type { };
        template<typename ElementType, int Shortest, int Longest>
            struct is_individual<esdl::variable_individual<ElementType, Shortest, Longest>> : std::true_type { };

        template<typename T>
            struct is_variable_individual : std::false_type { };
        template<typename ElementType, int Shortest, int Longest>
            struct is_variable_individual<esdl::variable_individual<ElementType, Shortest, Longest>> : std::true_type { };

        template<typename T>
            struct element_type { };
        template<typename ElementType, int Length>
            struct element_type<esdl::fixed_individual<ElementType, Length>> { typedef ElementType type; };
        template<typename ElementType, int Shortest, int Longest>
            struct element_type<esdl::variable_individual<ElementType, Shortest, Longest>> { typedef ElementType type; };
        
        template<typename T>
            struct length { };
        template<typename ElementType, int Length>
            struct length<esdl::fixed_individual<ElementType, Length>> : std::integral_constant<int, Length> { };
        template<typename ElementType, int Shortest, int Longest>
            struct length<esdl::variable_individual<ElementType, Shortest, Longest>> : std::integral_constant<int, Longest> { };

        template<typename T>
            struct minimum_length { };
        template<typename ElementType, int Length>
            struct minimum_length<esdl::fixed_individual<ElementType, Length>> : std::integral_constant<int, Length> { };
        template<typename ElementType, int Shortest, int Longest>
            struct minimum_length<esdl::variable_individual<ElementType, Shortest, Longest>> : std::integral_constant<int, Shortest> { };

        template<typename T>
            struct group_type {
            private:
                static auto callT(T* t=nullptr) -> decltype((*t)(0));
            public:
                typedef decltype(callT()) type;
            };

        template<typename T>
            struct individual_type {
                typedef typename individual_type<typename group_type<T>::type>::type type;
            };
        template<typename ElementType, int Length>
            struct individual_type<esdl::fixed_individual<ElementType, Length>>
            { typedef esdl::fixed_individual<ElementType, Length> type; };
        template<typename ElementType, int Shortest, int Longest>
            struct individual_type<esdl::variable_individual<ElementType, Shortest, Longest>>
            { typedef esdl::variable_individual<ElementType, Shortest, Longest> type; };

        template<typename T>
            struct is_binary_individual : std::false_type { };
    }
}

template<typename ElementType, int Length1, int Length2>
bool operator ==(
    const esdl::fixed_individual<ElementType, Length1>& a,
    const esdl::fixed_individual<ElementType, Length2>& b) restrict(cpu, amp) {
    return a.fitness == b.fitness;
}
template<typename ElementType, int Length1, int Length2>
bool operator !=(
    const esdl::fixed_individual<ElementType, Length1>& a,
    const esdl::fixed_individual<ElementType, Length2>& b) restrict(cpu, amp) {
    return !(a.fitness == b.fitness);
}
template<typename ElementType, int Length1, int Length2>
bool operator <(
    const esdl::fixed_individual<ElementType, Length1>& a,
    const esdl::fixed_individual<ElementType, Length2>& b) restrict(cpu, amp) {
    return a.fitness < b.fitness;
}
template<typename ElementType, int Length1, int Length2>
bool operator <=(
    const esdl::fixed_individual<ElementType, Length1>& a,
    const esdl::fixed_individual<ElementType, Length2>& b) restrict(cpu, amp) {
    return a.fitness < b.fitness || a.fitness == b.fitness;
}
template<typename ElementType, int Length1, int Length2>
bool operator >=(
    const esdl::fixed_individual<ElementType, Length1>& a,
    const esdl::fixed_individual<ElementType, Length2>& b) restrict(cpu, amp) {
    return !(a.fitness < b.fitness);
}
template<typename ElementType, int Length1, int Length2>
bool operator >(
    const esdl::fixed_individual<ElementType, Length1>& a,
    const esdl::fixed_individual<ElementType, Length2>& b) restrict(cpu, amp) {
    return !(a.fitness < b.fitness || a.fitness == b.fitness);
}

template<typename ElementType, int Shortest1, int Shortest2, int Longest1, int Longest2>
bool operator ==(
    const esdl::variable_individual<ElementType, Shortest1, Longest1>& a,
    const esdl::variable_individual<ElementType, Shortest2, Longest2>& b) restrict(cpu, amp) {
    return a.fitness == b.fitness;
}
template<typename ElementType, int Shortest1, int Shortest2, int Longest1, int Longest2>
bool operator !=(
    const esdl::variable_individual<ElementType, Shortest1, Longest1>& a,
    const esdl::variable_individual<ElementType, Shortest2, Longest2>& b) restrict(cpu, amp) {
    return !(a.fitness == b.fitness);
}
template<typename ElementType, int Shortest1, int Shortest2, int Longest1, int Longest2>
bool operator <(
    const esdl::variable_individual<ElementType, Shortest1, Longest1>& a,
    const esdl::variable_individual<ElementType, Shortest2, Longest2>& b) restrict(cpu, amp) {
    return a.fitness < b.fitness;
}
template<typename ElementType, int Shortest1, int Shortest2, int Longest1, int Longest2>
bool operator <=(
    const esdl::variable_individual<ElementType, Shortest1, Longest1>& a,
    const esdl::variable_individual<ElementType, Shortest2, Longest2>& b) restrict(cpu, amp) {
    return a.fitness < b.fitness || a.fitness == b.fitness;
}
template<typename ElementType, int Shortest1, int Shortest2, int Longest1, int Longest2>
bool operator >=(
    const esdl::variable_individual<ElementType, Shortest1, Longest1>& a,
    const esdl::variable_individual<ElementType, Shortest2, Longest2>& b) restrict(cpu, amp) {
    return !(a.fitness < b.fitness);
}
template<typename ElementType, int Shortest1, int Shortest2, int Longest1, int Longest2>
bool operator >(
    const esdl::variable_individual<ElementType, Shortest1, Longest1>& a,
    const esdl::variable_individual<ElementType, Shortest2, Longest2>& b) restrict(cpu, amp) {
    return !(a.fitness < b.fitness || a.fitness == b.fitness);
}


namespace esdl
{
    namespace tt
    {
        template<typename IndividualType>
            struct individual_type<esdl::group<IndividualType>>
            { typedef IndividualType type; };
        
        template<typename T>
            struct fitness_type
            { typedef float type; };

        template<typename T>
            struct is_group : std::false_type { };
        template<typename IndividualType>
            struct is_group<esdl::group<IndividualType>> : std::true_type { };
            
        template<typename T>
            struct is_operator {
            private:
                typedef char one; class two { char x[2]; };
                template <typename T2> static one test(decltype(T2::operator()(0))*);
                template <typename T2> static two test(...);
            public:
                static const bool value = sizeof(test<T>(0)) == sizeof(one);
            };
        
        template<typename IndividualType>
            struct group_type<esdl::group<IndividualType>>
            { typedef esdl::group<IndividualType> type; };
    }
}

namespace esdl_sort
{
    template<typename IndividualType>
    struct FitnessIndex
    {
    private:
        typedef typename esdl::tt::fitness_type<IndividualType>::type KeyType;
    public:
        KeyType k;
        int i;

        FitnessIndex() restrict(cpu, amp) { }
        FitnessIndex(const IndividualType& key, int index) restrict(cpu, amp) : k(key.fitness), i(index) { }

        bool operator<(const FitnessIndex<IndividualType>& other) const restrict(cpu, amp) {
            return k > other.k || (k == other.k && i < other.i);
        }

        operator KeyType() const restrict(cpu, amp) {
            return k;
        }
    };

    template<typename ElementType, int Length>
        struct key_index_type<esdl::fixed_individual<ElementType, Length>>
        { typedef FitnessIndex<esdl::fixed_individual<ElementType, Length>> type; };
    template<typename ElementType, int Shortest, int Longest>
        struct key_index_type<esdl::variable_individual<ElementType, Shortest, Longest>>
        { typedef FitnessIndex<esdl::variable_individual<ElementType, Shortest, Longest>> type; };
}
