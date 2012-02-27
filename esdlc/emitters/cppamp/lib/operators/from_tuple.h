#pragma once

#include <type_traits>
#include "..\individuals\individuals.h"

template<typename SourceType, int Index>
class from_tuple_t {
    SourceType source;
    static_assert(1 <= Index && Index <= esdl::tt::joined_count<SourceType>::value, "from_tuple index is not valid");
    typedef typename esdl::tt::joined_group_type<SourceType, Index-1>::type GroupType;

public:
    from_tuple_t(SourceType source) : source(source) { }

    GroupType operator()() {
        auto src = source();
        return src.get<Index>();
    }

    GroupType operator()(int count) {
        auto src = source(count);
        return src.get<Index>();
    }
};

template<typename SourceType, typename Index>
from_tuple_t<SourceType, Index::value>
from_tuple(SourceType source, Index) {
    return from_tuple_t<SourceType, Index::value>(source);
}
