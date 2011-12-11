#pragma once

#include "individuals.h"

template<int Length>
struct int_fixed_individual : esdl::fixed_individual<int, Length> {
    int lowest, highest;
    int_fixed_individual() { }
    int_fixed_individual(int lowest, int highest) : lowest(lowest), highest(highest) { }
};

namespace esdl
{
    namespace tt
    {
        template<int Length>
            struct is_individual<int_fixed_individual<Length>> : std::true_type { };
        template<int Length>
            struct individual_type<int_fixed_individual<Length>>
            { typedef individual_type<int_fixed_individual<Length>> type; };
        template<int Length>
            struct element_type<int_fixed_individual<Length>>
            { typedef int type; };
        template<int Length>
            struct length<int_fixed_individual<Length>> : std::integral_constant<int, Length> { };
        template<int Length>
            struct minimum_length<int_fixed_individual<Length>> : std::integral_constant<int, Length> { };
    }
}

namespace esdl_sort
{
    template<int Length>
        struct key_index_type<int_fixed_individual<Length>>
        { typedef FitnessIndex<int_fixed_individual<Length>> type; };
}

template<int Length>
struct _output<int_fixed_individual<Length>> {
    static const int WIDTH = 9;

    static void write(std::wostream& stream, const int_fixed_individual<Length>& value) {
        for (int i = 0; i < Length; ++i) {
            stream << std::setw(7) << value.genome[i];
            if (i < Length-1) stream << ", ";
        }
    }

    static void writeRaw(std::wostream& dest, const int_fixed_individual<Length>& value) {
        for (int i = 0; i < Length; ++i) {
            dest << value.genome[i];
            if (i < Length-1) dest << ", ";
        }
    }
};
