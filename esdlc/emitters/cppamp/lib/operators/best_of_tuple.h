#pragma once

#include <tuple>
#include <type_traits>
#include "..\utility.h"
#include "..\individuals\individuals.h"

template<typename SourceType>
class best_of_tuple2_t {
    SourceType source;
    typedef typename esdl::tt::joined_individual_type<SourceType, 0>::type IndividualType;
    static_assert(esdl::tt::joined_count<SourceType>::value == 2, "best_of_tuple requires two joined individuals");
    static_assert(std::is_same<typename esdl::tt::joined_individual_type<SourceType, 0>::type, typename esdl::tt::joined_individual_type<SourceType, 1>::type>::value,
        "best_of_tuple requires joined individuals to be the same type");
public:

    best_of_tuple2_t(SourceType source) : source(source) { }

    esdl::group<IndividualType> operator()() {
        auto src = source();
        src.group1.evaluate();
        src.group2.evaluate();

        auto pResult = esdl::make_group<IndividualType>(src.size());
        
        auto& src1 = *src.group1;
        auto& src2 = *src.group2;
        auto& result = *pResult;

        parallel_for_each(result.accelerator_view, result.extent, [&](index<1> i) restrict(amp) {
            result[i] = (src1[i].fitness > src2[i].fitness) ? src1[i] : src2[i];
        });

        pResult.evaluate_using(src.group1);
        pResult.evaluated = true;
        return pResult;
    }

    esdl::group<IndividualType> operator()(int count) {
        auto src = source(count);
        src.group1.evaluate();
        src.group2.evaluate();

        auto pResult = esdl::make_group<IndividualType>(src.size());
        
        auto& src1 = *src.group1;
        auto& src2 = *src.group2;
        auto& result = *pResult;

        parallel_for_each(result.accelerator_view, result.extent, [&](index<1> i) restrict(amp) {
            result[i] = (src1[i].fitness > src2[i].fitness) ? src1[i] : src2[i];
        });

        pResult.evaluate_using(src.group1);
        pResult.evaluated = true;
        return pResult;
    }
};

template<typename SourceType>
class best_of_tuple3_t {
    SourceType source;
    typedef typename esdl::tt::joined_individual_type<SourceType, 0>::type IndividualType;
    static_assert(esdl::tt::joined_count<SourceType>::value == 3, "best_of_tuple requires three joined individuals");
    static_assert(std::is_same<typename esdl::tt::joined_individual_type<SourceType, 0>::type, typename esdl::tt::joined_individual_type<SourceType, 1>::type>::value &&
        std::is_same<typename esdl::tt::joined_individual_type<SourceType, 0>::type, typename esdl::tt::joined_individual_type<SourceType, 2>::type>::value,
        "best_of_tuple requires joined individuals to be the same type");
public:

    best_of_tuple3_t(SourceType source) : source(source) { }

    esdl::group<IndividualType> operator()() {
        auto src = source();
        src.group1.evaluate();
        src.group2.evaluate();
        src.group3.evaluate();

        auto pResult = esdl::make_group<IndividualType>(src.size());
        
        auto& src1 = *src.group1;
        auto& src2 = *src.group2;
        auto& src3 = *src.group3;
        auto& result = *pResult;

        parallel_for_each(result.accelerator_view, result.extent, [&](index<1> i) restrict(amp) {
            if (src1[i].fitness > src2[i].fitness && src1[i].fitness > src3[i].fitness) {
                result[i] = src1[i];
            } else if (src2[i].fitness > src1[i].fitness && src2[i].fitness > src3[i].fitness) {
                result[i] = src2[i];
            } else {
                result[i] = src3[i];
            }
        });

        pResult.evaluate_using(src.group1);
        pResult.evaluated = true;
        return pResult;
    }

    esdl::group<IndividualType> operator()(int count) {
        auto src = source(count);
        src.group1.evaluate();
        src.group2.evaluate();
        src.group3.evaluate();

        auto pResult = esdl::make_group<IndividualType>(src.size());
        
        auto& src1 = *src.group1;
        auto& src2 = *src.group2;
        auto& src3 = *src.group3;
        auto& result = *pResult;

        parallel_for_each(result.accelerator_view, result.extent, [&](index<1> i) restrict(amp) {
            if (src1[i].fitness > src2[i].fitness && src[1].fitness > src[3].fitness) {
                result[i] = src1[i];
            } else if (src2[i].fitness > src[1].fitness && src2[i].fitness > src3[i].fitness) {
                result[i] = src2[i];
            } else {
                result[i] = src3[i];
            }
        });

        pResult.evaluate_using(src.group1);
        pResult.evaluated = true;
        return pResult;
    }
};

template<typename SourceType>
typename std::enable_if<esdl::tt::joined_count<SourceType>::value == 2, best_of_tuple2_t<SourceType>>::type
best_of_tuple(SourceType source) {
    return best_of_tuple2_t<SourceType>(source);
}

template<typename SourceType>
typename std::enable_if<esdl::tt::joined_count<SourceType>::value == 3, best_of_tuple3_t<SourceType>>::type
best_of_tuple(SourceType source) {
    return best_of_tuple3_t<SourceType>(source);
}
