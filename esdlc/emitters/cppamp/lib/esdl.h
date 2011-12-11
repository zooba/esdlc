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

#include "individuals\real_fixed_generators.h"
// ESDL generator: random_real(length=<int>10, lowest=(float)0, highest=(float)1)
// ESDL generator: real_low(length=<int>10, lowest=(float)0, highest=(float)1)
// ESDL generator: real_mid(length=<int>10, lowest=(float)0, highest=(float)1)
// ESDL generator: real_high(length=<int>10, lowest=(float)0, highest=(float)1)
#include "individuals\int_fixed_generators.h"
// ESDL generator: random_integer(length=<int>10, lowest=(int)0, highest=(int)99)
// ESDL generator: integer_low(length=<int>10, lowest=(int)0, highest=(int)99)
// ESDL generator: integer_mid(length=<int>10, lowest=(int)0, highest=(int)99)
// ESDL generator: integer_high(length=<int>10, lowest=(int)0, highest=(int)99)

#include "operators\merge.h"

#include "operators\mutate_random.h"
#include "operators\mutate_delta.h"
#include "operators\mutate_gaussian.h"
// ESDL operator: mutate_random(per_indiv_rate=(float)1, per_gene_rate=(float)0.1, genes=(int)0)
// ESDL operator: mutate_delta(step_size=(float)1, positive_rate=(float)0.5, per_indiv_rate=(float)1, per_gene_rate=(float)1, genes=(int)0)
// ESDL operator: mutate_gaussian(mean=(float)0, sigma=(float)0.5, per_indiv_rate=(float)1, per_gene_rate=(float)1, genes=(int)0)

#include "operators\unique.h"
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
#include "selectors\uniform_random.h"
#include "selectors\fitness_sus.h"
// ESDL operator: best()
// ESDL operator: worst()
// ESDL operator: uniform_random()
// ESDL operator: fitness_sus(mu=(int)0)
