#pragma once

#define NOMINMAX 1

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>

#include <amp.h>
#include <cvmarkers.h>

using concurrency::array_view;
using concurrency::extent;
using concurrency::grid;
using concurrency::index;
using concurrency::parallel_for_each;

#include "utility.h"
#include "sort.h"
#include "rng.h"
#include "statistics.h"
#include "command_line.h"
#include "output.h"

#include "individuals\groups.h"
#include "individuals\individuals.h"
#include "individuals\joined_individuals.h"


#include "individuals\real_fixed.h"
#include "individuals\real_variable.h"
#include "individuals\int_fixed.h"
#include "individuals\int_variable.h"
#include "individuals\bin_fixed.h"
#include "individuals\bin_variable.h"

#include "individuals\real_fixed_generators.h"
// ESDL generator: random_real(length=<int>10, lowest=(float)0, highest=(float)1)
// ESDL generator: real_value(length=<int>10, lowest=(float)0, highest=(float)1, value=(float)0)
// ESDL generator: real_low(length=<int>10, lowest=(float)0, highest=(float)1)
// ESDL generator: real_mid(length=<int>10, lowest=(float)0, highest=(float)1)
// ESDL generator: real_high(length=<int>10, lowest=(float)0, highest=(float)1)
#include "individuals\int_fixed_generators.h"
// ESDL generator: random_integer(length=<int>10, lowest=(int)0, highest=(int)100)
// ESDL generator: integer_value(length=<int>10, lowest=(int)0, highest=(int)100, value=(int)0)
// ESDL generator: integer_low(length=<int>10, lowest=(int)0, highest=(int)100)
// ESDL generator: integer_mid(length=<int>10, lowest=(int)0, highest=(int)100)
// ESDL generator: integer_high(length=<int>10, lowest=(int)0, highest=(int)100)
#include "individuals\bin_fixed_generators.h"
// ESDL generator: random_binary(length=<int>10, true_rate=(float)0.5f)
// ESDL generator: binary_true(length=<int>10)
// ESDL generator: binary_false(length=<int>10)

#include "operators\merge.h"

#include "operators\mutate_random.h"
#include "operators\mutate_bitflip.h"
#include "operators\mutate_delta.h"
#include "operators\mutate_gaussian.h"
// ESDL operator: mutate_random(per_indiv_rate=(float)1, per_gene_rate=(float)0.1, genes=(int)0)
// ESDL operator: mutate_bitflip(per_indiv_rate=(float)1, per_gene_rate=(float)0.1, genes=(int)0)
// ESDL operator: mutate_delta(step_size=(float)1, positive_rate=(float)0.5, per_indiv_rate=(float)1, per_gene_rate=(float)1, genes=(int)0)
// ESDL operator: mutate_gaussian(mean=(float)0, sigma=(float)0.5, per_indiv_rate=(float)1, per_gene_rate=(float)1, genes=(int)0)

#include "operators\crossover.h"
#include "operators\crossover_average.h"
// ESDL operator: crossover(points=<int>1, per_pair_rate=(float)1.0, one_child=(bool)true, two_children=(bool)false)
// ESDL operator: crossover_one(per_pair_rate=(float)1.0, one_child=(bool)true, two_children=(bool)false)
// ESDL operator: crossover_average(per_pair_rate=(float)1.0, per_gene_rate=(float)0.1, genes=(int)0)

#include "filters\unique.h"
// ESDL operator: unique()


#include "joiners\tuples.h"
#include "joiners\random_tuples.h"
// ESDL joiner: tuples()
// ESDL joiner: random_tuples(distinct=<bool>false)

#include "operators\best_of_tuple.h"
#include "operators\crossover_tuple.h"
// ESDL operator: best_of_tuple()
// ESDL operator: crossover_tuple(per_pair_rate=(float)1, per_gene_rate=(float)0.5)

#include "selectors\best.h"
#include "selectors\tournament.h"
#include "selectors\uniform_random.h"
#include "selectors\uniform_shuffle.h"
#include "selectors\repeated.h"
#include "selectors\fitness_proportional.h"
#include "selectors\fitness_sus.h"
// ESDL operator: best()
// ESDL operator: worst()
// ESDL operator: tournament(k=(int)2, greediness=(float)1.0, with_replacement=<bool>true, without_replacement=<bool>false)
// ESDL operator: uniform_random()
// ESDL operator: uniform_shuffle()
// ESDL operator: repeated()
// ESDL operator: fitness_proportional(offset=nullptr, with_replacement=<bool>true, without_replacement=<bool>false)
// ESDL operator: fitness_sus(offset=nullptr, mu=(int)0)
