FROM suitable_individuals SELECT (size) population
YIELD population

BEGIN generation
    FROM population SELECT offspring USING mutate_gaussian(step_size=1.0,per_gene_rate=1.0)
    
    FROM population, offspring SELECT (size) population USING best
    YIELD population
END generation
