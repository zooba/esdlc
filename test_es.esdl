FROM suitable_individuals SELECT (size) population
YIELD population

current_step = 1.0
adapt_step = 0.1

BEGIN generation
    FROM population SELECT 1 parent       USING uniform_random
    FROM parent     SELECT (size) parents USING repeated
    FROM parents    SELECT offspring      USING mutate_gaussian(step_size=current_step,per_gene_rate=1.0)
    YIELD offspring
    
    # calculate success rate based on parents and offspring
    success_rate = es_success_rate(parents, offspring)
    current_step = es_adapt(current_step=current_step, adapt_step=adapt_step, success_rate=success_rate)
    
    FROM population, offspring SELECT (size) population USING best
    YIELD population
END generation
