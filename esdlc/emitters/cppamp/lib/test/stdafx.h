// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define NOMINMAX 1

#include "targetver.h"
#include <tchar.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <amp.h>
#include <cvmarkers.h>
#include <cvmarkersobj.h>

using concurrency::array_view;
using concurrency::extent;
using concurrency::grid;
using concurrency::index;
using concurrency::parallel_for_each;

#include "utility.h"
#include "rng.h"
#include "statistics.h"
#include "output.h"

#include "individuals\individuals.h"

using std::begin;
using std::end;

inline void _assert(bool expr) {
    if (!expr) DebugBreak();
}

template<typename T, typename Pr>
inline void assert_all(const T& coll, Pr pred) {
    _assert(begin(coll) != end(coll));
    _assert(std::all_of(begin(coll), end(coll), pred)); }

template<typename T, typename Pr>
inline void assert_most(const T& coll, Pr pred) {
    _assert((int)std::count_if(begin(coll), end(coll), pred) >= (int)((9 * coll.size()) / 10));
}

template<typename It, typename Pr>
inline bool most_of(It first, It last, Pr pred) {
    return (int)std::count_if(first, last, pred) >= (int)((9 * (last - first)) / 10);
}

extern PCV_PROVIDER _provider;
extern PCV_MARKERSERIES _series;
extern PCV_SPAN _span;

inline void test_start(const wchar_t* name) {
    std::wcout << name << L"...";
    CvEnterSpanW(_series, &_span, name);
}

inline void test_pass() {
    CvLeaveSpan(_span);
    std::wcout << L"OK" << std::endl;
}

inline void test_notimpl() {
    CvLeaveSpan(_span);
    std::wcout << L"not implemented" << std::endl;
}

struct TestEvaluator {
    TestEvaluator() { }
    
    template<typename IndividualType>
    void operator()(esdl::group<IndividualType, TestEvaluator> group) const {
        auto& g = *group;
        concurrency::parallel_for_each(g.grid, [&](index<1> i) restrict(direct3d) {
            g[i].fitness = 0;
            for (int j = 0; j < 10; ++j) {
                g[i].fitness += g[i].genome[j];
            }
        });
    }
};
