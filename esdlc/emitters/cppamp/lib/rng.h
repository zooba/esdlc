#pragma once

#include <vector>
#include <amp.h>

namespace esdl { void seed(unsigned int seed); }

std::vector<float> random_vector(int count);
std::vector<float> random_normal_vector(int count);
float random_one();

std::shared_ptr<concurrency::array<float, 1>> random_array(int count1);
std::shared_ptr<concurrency::array<float, 2>> random_array(int count1, int count2);
std::shared_ptr<concurrency::array<float, 3>> random_array(int count1, int count2, int count3);

std::shared_ptr<concurrency::array<float, 1>> random_normal_array(int count1);
std::shared_ptr<concurrency::array<float, 2>> random_normal_array(int count1, int count2);
std::shared_ptr<concurrency::array<float, 3>> random_normal_array(int count1, int count2, int count3);
