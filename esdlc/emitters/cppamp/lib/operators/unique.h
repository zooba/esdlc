#pragma once

#include <set>
#include "..\utility.h"
#include "..\individuals\individuals.h"

template<typename SourceType>
class unique_t {
    SourceType source;
    typedef typename esdl::tt::individual_type<SourceType>::type IndividualType;
    typedef typename esdl::tt::evaluator_type<SourceType>::type EvaluatorType;

    struct indiv_less {
        bool operator() (const IndividualType& lhs, const IndividualType& rhs) const
        {
            for (int i = 0; i < esdl::tt::length<IndividualType>::value; ++i) {
                if (lhs.genome[i] < rhs.genome[i]) return true;
                else if (lhs.genome[i] > rhs.genome[i]) return false;
            }
            return false;
        }
    };

public:
    unique_t(SourceType source) : source(source) { }

    esdl::group<IndividualType, EvaluatorType> operator()(int count) {
        bool evaluated = true;
        std::list<IndividualType> indivs;
        std::set<IndividualType, indiv_less> seen;

        while (indivs.size() < (unsigned int)count) {
            auto src = source(count);
            if (src.size() == 0) break;
            if (!src.evaluated) evaluated = false;
            auto src_list = src.as_list();
            for (auto i = std::begin(src_list); i != std::end(src_list); ++i) {
                if (seen.find(*i) == std::end(seen)) {
                    indivs.push_back(*i);
                    if (indivs.size() == count) break;
                    seen.insert(*i);
                }
            }
        }

        while (indivs.size() > (unsigned int)count) {
            indivs.pop_back();
        }

        auto result = esdl::make_group<IndividualType, EvaluatorType>(indivs);
        result.evaluated = evaluated;
        return result;
    }

    esdl::group<IndividualType, EvaluatorType> operator()() {
        auto src = source();
        auto src_list = src.as_list();
        decltype(src_list) indivs;
        std::set<IndividualType, indiv_less> seen;

        for (auto i = std::begin(src_list); i != std::end(src_list); ++i) {
            if (seen.find(*i) == std::end(seen)) {
                indivs.push_back(*i);
                seen.insert(*i);
            }
        }

        auto result = esdl::make_group<IndividualType, EvaluatorType>(indivs);
        result.evaluated = src.evaluated;
        return result;
    }
};

template<typename SourceType>
unique_t<SourceType> unique(SourceType source) {
    return unique_t<SourceType>(source);
}
