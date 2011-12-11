#include "stdafx.h"


void test_random_vector() {
    test_start(L"Uniformly distributed RNG (CPU)");
    esdl::seed((unsigned int)std::clock());
    auto r = random_vector(100000);
    float sum = 0.0f;
    int within[10] = { 0 };

    std::for_each(begin(r), end(r), [&](float i) { 
        sum += i;
        within[(int)(i * 10)] += 1;
    });

    _assert(std::abs(sum - r.size() / 2) < 0.01f * r.size());

    assert_all(within, [](int i) { return 9000 < i && i < 11000; });

    test_pass();
}

void test_random_array() {
    test_start(L"Uniformly distributed RNG (GPU)");
    esdl::seed((unsigned int)std::clock());
    auto _r = random_array(100000);
    std::vector<float> r(*_r);
    
    float sum = 0.0f;
    int within[10] = { 0 };

    std::for_each(begin(r), end(r), [&](float i) { 
        sum += i;
        within[(int)(i * 10)] += 1;
    });

    _assert(std::abs(sum - r.size() / 2) < 0.01f * r.size());

    assert_all(within, [](int i) { return 9000 < i && i < 11000; });

    test_pass();
}

void test_random() {
    test_random_vector();
    test_random_array();
}

void test_random_normal_vector() {
    test_start(L"Normally distributed RNG (CPU)");

    esdl::seed((unsigned int)std::clock());
    auto r = random_normal_vector(100000);
    float sum = 0.0f;

    std::for_each(begin(r), end(r), [&](float i) { sum += i; });

    _assert(std::abs(sum) < 0.01f * r.size());

    float within1p = std::count_if(begin(r), end(r), [&](float i) { return 0.0f < i && i < 1.0f; }) / (float)r.size();
    float within2p = std::count_if(begin(r), end(r), [&](float i) { return 0.0f < i && i < 2.0f; }) / (float)r.size();
    float within3p = std::count_if(begin(r), end(r), [&](float i) { return 0.0f < i && i < 3.0f; }) / (float)r.size();
    float within1n = std::count_if(begin(r), end(r), [&](float i) { return -1.0f < i && i < 0.0f; }) / (float)r.size();
    float within2n = std::count_if(begin(r), end(r), [&](float i) { return -2.0f < i && i < 0.0f; }) / (float)r.size();
    float within3n = std::count_if(begin(r), end(r), [&](float i) { return -3.0f < i && i < 0.0f; }) / (float)r.size();

    _assert(0.33f < within1p && within1p < 0.35f);
    _assert(0.33f < within1n && within1n < 0.35f);
    _assert(0.465f < within2p && within2p < 0.485f);
    _assert(0.465f < within2n && within2n < 0.485f);
    _assert(0.48f < within3p && within3p < 0.51f);
    _assert(0.48f < within3n && within3n < 0.51f);

    test_pass();
}

void test_random_normal_array() {
    test_start(L"Normally distributed RNG (GPU)");

    esdl::seed((unsigned int)std::clock());
    auto _r = random_normal_array(100000);
    std::vector<float> r(*_r);
    float sum = 0.0f;

    std::for_each(begin(r), end(r), [&](float i) { sum += i; });

    _assert(std::abs(sum) < 0.01f * r.size());

    float within1p = std::count_if(begin(r), end(r), [&](float i) { return 0.0f < i && i < 1.0f; }) / (float)r.size();
    float within2p = std::count_if(begin(r), end(r), [&](float i) { return 0.0f < i && i < 2.0f; }) / (float)r.size();
    float within3p = std::count_if(begin(r), end(r), [&](float i) { return 0.0f < i && i < 3.0f; }) / (float)r.size();
    float within1n = std::count_if(begin(r), end(r), [&](float i) { return -1.0f < i && i < 0.0f; }) / (float)r.size();
    float within2n = std::count_if(begin(r), end(r), [&](float i) { return -2.0f < i && i < 0.0f; }) / (float)r.size();
    float within3n = std::count_if(begin(r), end(r), [&](float i) { return -3.0f < i && i < 0.0f; }) / (float)r.size();

    _assert(0.33f < within1p && within1p < 0.35f);
    _assert(0.33f < within1n && within1n < 0.35f);
    _assert(0.465f < within2p && within2p < 0.485f);
    _assert(0.465f < within2n && within2n < 0.485f);
    _assert(0.48f < within3p && within3p < 0.51f);
    _assert(0.48f < within3n && within3n < 0.51f);

    test_pass();
}

void test_random_normal() {
    test_random_normal_vector();
    test_random_normal_array();
}

