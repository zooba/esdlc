FROM suitable_individuals SELECT (size) population
YIELD population

BEGIN generation
    FROM population SELECT (size) offspring USING binary_tournament
    FROM offspring  SELECT population       USING crossover_one(per_pair_rate=0.8), \
                                                  mutate_random(per_indiv_rate=(1.0/size))
    YIELD population
END generation
