FROM random_binary(length=config.landscape.size.exact) SELECT (size) population
JOIN population, population INTO pairs USING random_tuples
EVAL pairs USING config.landscape
EVAL population USING assign(source=pairs)
YIELD population

BEGIN generation
    FROM population SELECT (size) offspring USING binary_tournament
    FROM offspring  SELECT population       USING crossover_one(per_pair_rate=0.8), \
                                                  mutate_random(per_indiv_rate=(1.0/size))
    
    JOIN population, population INTO pairs USING random_tuples
    EVAL pairs USING config.landscape
    EVAL population USING assign(source=pairs)
    YIELD population
END generation
