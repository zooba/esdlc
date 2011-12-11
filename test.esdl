FROM random_binary(length=config.length.max) SELECT 100 population
t = 0
delta_t = -0.1
lambda = 100
def = class(lambda)
EVAL population USING evaluators.population(t)
YIELD population

BEGIN generation
    t = t + (delta_t * 1.4)
    `py print t

    REPEAT 4
        FROM population SELECT 100 parents USING tournament(k=2, greediness=0.7)
        
        mutate_parents = parents
        parents2 = parents
        crossover_parents = parents
        FROM mutate_parents SELECT mutated USING mutate_delta(stepsize)
        FROM crossover_parents SELECT crossed USING uniform_crossover
        EVAL mutated, crossed USING evaluator(t=t, lambda)
        
        JOIN mutated, crossed INTO merged USING tuples
        FROM merged SELECT offspring USING best_of_tuple
        
        FROM population, offspring SELECT 99 population, rest USING best
        FROM rest SELECT 1 extras USING uniform_random
        FROM population, rest, extras SELECT (((100))) population
    END REPEAT
    
    EVAL population USING evaluators.config(t)
    YIELD population
END generation

