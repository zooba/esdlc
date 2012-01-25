// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void test_random();
void test_random_normal();

void test_sort();

void test_real_generators_fixed();
void test_int_generators_fixed();
void test_bin_generators_fixed();

void test_mutate_random_real();
void test_mutate_random_int();

void test_mutate_bitflip();

void test_mutate_delta_real();
void test_mutate_delta_int();

void test_mutate_gaussian_real();
void test_mutate_gaussian_int();

void test_crossover();
void test_crossover_average();

void test_unique();
void test_repeated();
void test_best();
void test_worst();
void test_uniform_random();
void test_uniform_shuffle();

void test_tuples_2();
void test_tuples_3();
void test_random_tuples_2();
void test_random_tuples_3();
void test_crossover_tuple_2();
void test_crossover_tuple_3();
void test_best_of_tuple_2();
void test_best_of_tuple_3();

void test_fitness_proportional();
void test_fitness_sus();


PCV_PROVIDER _provider;
PCV_MARKERSERIES _series;
PCV_SPAN _span;

void forceCppAmpLoad() {
    concurrency::array<int, 1> arr(100);
    parallel_for_each(arr.grid, [&](index<1> i) restrict(direct3d) { arr[i] = i.x; });
}

int wmain(int argc, wchar_t* argv[]) {
    CvCreateDefaultMarkerSeriesOfDefaultProvider(&_provider, &_series);

    forceCppAmpLoad();

    esdl::seed(1);

    for (int i = 0; i < 2; ++i) {
        test_random();
        test_random_normal();

        test_sort();
        
        test_real_generators_fixed();
        test_int_generators_fixed();
        test_bin_generators_fixed();

        test_mutate_random_real();
        test_mutate_random_int();

        test_mutate_bitflip();

        test_mutate_delta_real();
        test_mutate_delta_int();
        
        test_mutate_gaussian_real();
        test_mutate_gaussian_int();
        
        test_crossover();
        test_crossover_average();

        test_unique();
        test_repeated();
    
        test_best();
        test_worst();
        test_uniform_random();

        test_tuples_2();
        test_tuples_3();
        test_random_tuples_2();
        test_random_tuples_3();
        test_crossover_tuple_2();
        test_crossover_tuple_3();
        test_best_of_tuple_2();
        test_best_of_tuple_3();

        test_fitness_proportional();
        test_fitness_sus();
    }

    CvReleaseMarkerSeries(_series);
    CvReleaseProvider(_provider);
    _series = nullptr;
    _provider = nullptr;

    return 0;
}

