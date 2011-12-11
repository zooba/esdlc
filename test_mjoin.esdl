size=10
length=7
FROM random_binary(length=length) SELECT (size) population
JOIN population, population INTO pairs USING random_tuples(distinct=true)
EVAL pairs USING joined_sum(length=length)
YIELD pairs

BEGIN generation
    FROM population SELECT (size) offspring USING uniform_random
    FROM offspring  SELECT population       USING mutate_bitflip(per_gene_rate=(1/size))
    
    JOIN population, population INTO pairs USING random_tuples(distinct=true)
    EVAL pairs USING joined_sum(length=length)
    YIELD pairs
END generation
