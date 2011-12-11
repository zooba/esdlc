FROM suitable_individuals SELECT (size) population
YIELD population

BEGIN generation
    REPEAT (size)
        FROM population SELECT 2 parents USING binary_tournament
        FROM parents    SELECT offspring USING crossover_one(per_pair_rate=0.9), \
                                               mutate_random(per_gene_rate=0.01)
        
        FROM offspring      SELECT 1 replacer       USING best
        FROM population     SELECT 1 replacee, rest USING uniform_shuffle
        FROM replacer, rest SELECT population
    END repeat

    YIELD population
END generation
