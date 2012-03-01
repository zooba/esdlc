#pragma once

#include <type_traits>
#include "individuals.h"

template<int Length>
struct real_fixed_individual : esdl::fixed_individual<float, Length> {
    float lowest, highest;
    real_fixed_individual() { }
    real_fixed_individual(float lowest, float highest) : lowest(lowest), highest(highest) { }

    void operator=(const real_fixed_individual<Length>& other) restrict(cpu, amp)
    {
        for (int i = 0; i < Length; ++i) {
            genome[i] = other.genome[i];
        }
        fitness = other.fitness;
        lowest = other.lowest;
        highest = other.highest;
    }
};

namespace esdl
{
    namespace tt
    {
        template<int Length>
            struct is_individual<real_fixed_individual<Length>> : std::true_type { };
        template<int Length>
            struct individual_type<real_fixed_individual<Length>>
            { typedef individual_type<real_fixed_individual<Length>> type; };
        template<int Length>
            struct element_type<real_fixed_individual<Length>>
            { typedef float type; };
        template<int Length>
            struct length<real_fixed_individual<Length>> : std::integral_constant<int, Length> { };
        template<int Length>
            struct minimum_length<real_fixed_individual<Length>> : std::integral_constant<int, Length> { };
    }
}

namespace esdl_sort
{
    template<int Length>
        struct key_index_type<real_fixed_individual<Length>>
        { typedef FitnessIndex<real_fixed_individual<Length>> type; };
}

template<int Length>
struct _output<real_fixed_individual<Length>> {
    static const int WIDTH = 9;

    static void write(std::wostream& stream, const real_fixed_individual<Length>& value) {
        for (int i = 0; i < Length; ++i) {
            stream << std::setw(7) << value.genome[i];
            if (i < Length-1) stream << ", ";
        }
    }

    static void writeRaw(std::wostream& dest, const real_fixed_individual<Length>& value) {
        for (int i = 0; i < Length; ++i) {
            dest << value.genome[i];
            if (i < Length-1) dest << ", ";
        }
    }
};
