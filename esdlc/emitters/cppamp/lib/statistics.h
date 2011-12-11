#pragma once

#include <map>
#include <string>
#include <vector>

#include <amp.h>

#include "utility.h"
#include "individuals\groups.h"

// Global state variables, which are accessible throughout.
namespace {
    int _iteration = 0;
    int _evaluations = 0;
    std::wstring _current_block(L"initialisation");
}

namespace esdl
{
    // Called by evaluators to increment the evaluation count.
    namespace {
        void count_evaluations(int count) {
            _evaluations += count;
        }
    }

    enum EndReason {
        IterLimit,
        EvalLimit,
        FitLimit
    };


    struct Statistics
    {
        float min_fitness;
        float mean_fitness;
        float median_fitness;
        float max_fitness;
    };

    namespace {
        std::map<std::wstring, Statistics> _statistics;
        Statistics _default_statistics;
        std::wstring _default_statistics_name;
    }

    template<typename IndividualType, typename EvaluatorType>
    std::vector<IndividualType> make_stats(const std::wstring& name, const group<IndividualType, EvaluatorType>& source)
    {
        std::vector<IndividualType> group;
        group.reserve(source.size());
        concurrency::copy(*esdl_sort::parallel_sort(*source), std::back_inserter(group));
        Statistics stats;
        stats.max_fitness = (float)group.front().fitness;
        stats.min_fitness = (float)group.back().fitness;
        stats.median_fitness = (float)group[group.size() / 2].fitness;
        double sum = 0.0;
        std::for_each(std::begin(group), std::end(group), [&](const IndividualType& i) {
            sum += (double)(float)i.fitness;
        });
        stats.mean_fitness = (float)sum / group.size();

        _statistics[name] = stats;
        if (equals(name, _default_statistics_name))
            _default_statistics = stats;

        return group;
    }

    namespace {
        const std::map<std::wstring, Statistics>& statistics = _statistics;
        const Statistics& default_statistics = _default_statistics;
    }
}