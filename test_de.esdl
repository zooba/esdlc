`include "test_de.h"
`include "sphere.h"
`evaluator sphere()
length = 10

FROM random_real(length, \
     lowest=-2.0,highest=2.0) SELECT (size) population USING unique
YIELD population

BEGIN GENERATION
    targets = population
    
    # Stochastic Universal Sampling for bases
    FROM population SELECT (size) bases USING fitness_sus(mu=size)
    
    # Ensure r0 != r1 != r2, but any may equal i
    JOIN bases, population, population INTO mutators USING random_tuples(distinct)
    
    FROM mutators SELECT mutants USING mutate_DE(scale)
    
    JOIN targets, mutants INTO target_mutant_pairs USING tuples
    FROM target_mutant_pairs SELECT trials USING crossover_tuple(per_gene_rate=CR)
    
    JOIN targets, trials INTO targets_trial_pairs USING tuples
    FROM targets_trial_pairs SELECT population USING best_of_tuple
    
    YIELD population
    length=1
END GENERATION
