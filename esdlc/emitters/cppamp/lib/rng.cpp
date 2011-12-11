#pragma once

#include "utility.h"
#include "rng.h"
#include <amp.h>
#include <cvmarkers.h>
#include <tuple>

using namespace concurrency;

namespace
{
    PCV_PROVIDER _cv_provider;
    PCV_MARKERSERIES _cv_series;

    void do_tidy() {
        CvReleaseMarkerSeries(_cv_series);
        CvReleaseProvider(_cv_provider);
    }

    bool do_init() {
        CvInitProvider(&CvDefaultProviderGuid, &_cv_provider);
        CvCreateMarkerSeriesW(_cv_provider, L"RNG", &_cv_series);
        
        atexit(do_tidy);
        return true;
    }
    static const bool _cv_init = do_init();
}

namespace
{
    const int STATE_WIDTH = 512;
    const int STATE_DEPTH = 4;
    std::shared_ptr<array<unsigned int, 2>> state;
    unsigned int cpuState[STATE_DEPTH];

    unsigned int lcg_step(unsigned int& state, unsigned int a, unsigned int c) restrict(cpu, direct3d) {  
        return state = (a * state + c);  
    }

    unsigned int tausworthe_step(unsigned int& state, unsigned int s1, unsigned int s2, unsigned int s3, unsigned int m) restrict(cpu, direct3d) {
        unsigned int b = (((state * s1) ^ state) / s2);  
        return state = (((state & m) * s3) ^ b);  
    }

    template<typename State>
    unsigned int next_uint(State state) restrict(cpu, direct3d) {
        return (tausworthe_step(state[0], 1 << 13, 1 << 19, 1 << 12, 4294967294UL) ^
            tausworthe_step(state[1], 1 << 2, 1 << 25, 1 << 4, 4294967288UL) ^
            tausworthe_step(state[2], 1 << 3, 1 << 11, 1 << 17, 4294967280UL) ^
            lcg_step(state[3], 1664525, 1013904223UL)
        );
    }

    template<typename State>
    float next_float(State state) restrict(cpu, direct3d) {
        return (float)(2.3283064365387e-10f * next_uint(state));
    }

    struct float2 {
        float f1;
        float f2;
        float2(float f1, float f2) restrict(cpu, direct3d) : f1(f1), f2(f2) { }
    };

    const float PI = 3.1415926535897932384626433832795f;
    template<typename State>
    float2 next_normal(State state) restrict(cpu) {
        float u0 = next_float(state), u1 = next_float(state);
        float r = sqrt(-2 * log(u0));  
        float theta = 2 * PI * u1;
        return float2(r * sin(theta), r * cos(theta));
    }

    template<typename State>
    float2 next_normal(State state) restrict(direct3d) {
        float u0 = next_float(state), u1 = next_float(state);
        float r = sqrt(-2 * log(u0));  
        float theta = 2 * u1;
        return float2(r * sinpi(theta), r * cospi(theta));
    }

    template<int Rank>
    void fill_array(array<float, Rank>& arr) {
        auto& _state = *state;
        const int count = (int)arr.extent.size();
        parallel_for_each(esdl::acc, grid<1>(extent<1>(STATE_WIDTH)), [=, &_state, &arr](index<1> i) restrict(direct3d) {
            for (int start = 0; start < count; start += STATE_WIDTH) {
                if (i[0] + start < count) {
                    arr.data()[i[0] + start] = next_float(_state[i[0]]);
                }
            }
        });
    }

    template<int Rank>
    void fill_normal_array(array<float, Rank>& arr) {
        auto& _state = *state;
        const int count = (int)arr.extent.size();
        parallel_for_each(esdl::acc, grid<1>(extent<1>(STATE_WIDTH)), [=, &_state, &arr](index<1> i) restrict(direct3d) {
            for (int start = 0; start < count; start += 2*STATE_WIDTH) {
                float2 r = next_normal(_state[i[0]]);
                if (i[0] + start < count) {
                    arr.data()[i[0] + start] = r.f1;
                }
                if (i[0] + STATE_WIDTH + start < count) {
                    arr.data()[i[0] + STATE_WIDTH + start] = r.f2;
                }
            }
        });
    }
}

namespace esdl
{
    void seed(unsigned int seed)
    {
        const unsigned int original_seed = seed;
        
        if (state == nullptr) {
            state = std::make_shared<concurrency::array<unsigned int, 2>>(STATE_WIDTH, STATE_DEPTH, esdl::acc);
        }

        if (seed == 0) seed = GetTickCount();
        
        CvWriteMessageExW(_cv_series, CvImportanceNormal, 1, L"Seeded %d (%d)", seed, original_seed);

        unsigned int state0 = seed;
        for (int i = 0; i < STATE_DEPTH; ++i) {
            cpuState[i] = lcg_step(state0, 1664525, 1013904223UL);
        }
        
        std::vector<unsigned int> stateVector;
        stateVector.reserve(STATE_WIDTH * STATE_DEPTH);
        for (int i = 0; i < STATE_WIDTH * STATE_DEPTH; ++i) {
            stateVector.push_back(next_uint(cpuState));
        }
        
        copy(std::begin(stateVector), std::end(stateVector), *state);
    }
}

std::vector<float> random_vector(int count)
{
    PCV_SPAN _s; CvEnterSpanW(_cv_series, &_s, L"random_vector(%d)", count);
    std::vector<float> data;
    data.reserve(count);
    for(int i = 0; i < count; ++i)
        data.push_back(next_float(cpuState));
    
    CvLeaveSpan(_s);
    return std::move(data);
}

std::vector<float> random_normal_vector(int count)
{
    PCV_SPAN _s; CvEnterSpanW(_cv_series, &_s, L"random_normal_vector(%d)", count);
    std::vector<float> data;
    data.reserve(count);
    for(int i = 0; i < count; i += 2) {
        auto r = next_normal(cpuState);
        data.push_back(r.f1);
        data.push_back(r.f2);
    }
    while ((int)data.size() > count) {
        data.pop_back();
    }

    CvLeaveSpan(_s);
    return std::move(data);
}

float random_one()
{
    return next_float(cpuState);
}

std::shared_ptr<array<float, 1>> random_array(int count1) {
    PCV_SPAN _s; CvEnterSpanW(_cv_series, &_s, L"random_array<float, 1>(%d)", count1);
    auto result = std::make_shared<array<float, 1>>(count1, esdl::acc);
    fill_array(*result);
    CvLeaveSpan(_s);
    return result;
}

std::shared_ptr<array<float, 2>> random_array(int count1, int count2) {
    PCV_SPAN _s; CvEnterSpanW(_cv_series, &_s, L"random_array<float, 2>(%d, %d)", count1, count2);
    auto result = std::make_shared<array<float, 2>>(count1, count2, esdl::acc);
    fill_array(*result);
    CvLeaveSpan(_s);
    return result;
}

std::shared_ptr<array<float, 3>> random_array(int count1, int count2, int count3) {
    PCV_SPAN _s; CvEnterSpanW(_cv_series, &_s, L"random_array<float, 3>(%d, %d, %d)", count1, count2, count3);
    auto result = std::make_shared<array<float, 3>>(count1, count2, count3, esdl::acc);
    fill_array(*result);
    CvLeaveSpan(_s);
    return result;
}

std::shared_ptr<array<float, 1>> random_normal_array(int count1) {
    PCV_SPAN _s; CvEnterSpanW(_cv_series, &_s, L"random_normal_array<float, 1>(%d)", count1);
    auto result = std::make_shared<array<float, 1>>(count1, esdl::acc);
    fill_normal_array(*result);
    CvLeaveSpan(_s);
    return result;
}

std::shared_ptr<array<float, 2>> random_normal_array(int count1, int count2) {
    PCV_SPAN _s; CvEnterSpanW(_cv_series, &_s, L"random_normal_array<float, 2>(%d, %d)", count1, count2);
    auto result = std::make_shared<array<float, 2>>(count1,  count2, esdl::acc);
    fill_normal_array(*result);
    CvLeaveSpan(_s);
    return result;
}

std::shared_ptr<array<float, 3>> random_normal_array(int count1, int count2, int count3) {
    PCV_SPAN _s; CvEnterSpanW(_cv_series, &_s, L"random_normal_array<float, 3>(%d, %d, %d)", count1, count2, count3);
    auto result = std::make_shared<array<float, 3>>(count1, count2, count3, esdl::acc);
    fill_normal_array(*result);
    CvLeaveSpan(_s);
    return result;
}


