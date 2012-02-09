#pragma once

#include "individuals.h"

template<int Length>
struct bin_fixed_individual : esdl::fixed_individual<int, Length> {
    bin_fixed_individual() { }

    void operator=(const bin_fixed_individual<Length>& other) restrict(cpu, direct3d)
    {
        for (int i = 0; i < Length; ++i) {
            genome[i] = other.genome[i];
        }
        fitness = other.fitness;
    }
};

namespace esdl
{
    namespace tt
    {
        template<int Length>
            struct is_individual<bin_fixed_individual<Length>> : std::true_type { };
        template<int Length>
            struct individual_type<bin_fixed_individual<Length>>
            { typedef individual_type<bin_fixed_individual<Length>> type; };
        template<int Length>
            struct element_type<bin_fixed_individual<Length>>
            { typedef int type; };
        template<int Length>
            struct length<bin_fixed_individual<Length>> : std::integral_constant<int, Length> { };
        template<int Length>
            struct minimum_length<bin_fixed_individual<Length>> : std::integral_constant<int, Length> { };

        template<int Length>
            struct is_binary_individual<bin_fixed_individual<Length>> : std::true_type { };
    }
}

namespace esdl_sort
{
    template<int Length>
        struct key_index_type<bin_fixed_individual<Length>>
        { typedef FitnessIndex<bin_fixed_individual<Length>> type; };
}

template<int Length>
struct _output<bin_fixed_individual<Length>> {
    static const int WIDTH = 9;

    static void write(std::wostream& stream, const bin_fixed_individual<Length>& value) {
        for (int i = 0; i < Length; ++i) {
            stream << (value.genome[i] == 0) ? '0' : '1';
        }
    }

    static void writeRaw(std::wostream& dest, const bin_fixed_individual<Length>& value) {
        for (int i = 0; i < Length; ++i) {
            dest << value.genome[i];
        }
    }
};
