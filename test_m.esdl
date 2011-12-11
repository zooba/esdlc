FROM random_real(length=2,lowest=-5.12,highest=5.12) SELECT 10 population
EVAL population USING rosenbrock
YIELD population

BEGIN GENERATION
    FROM population SELECT 10 parents USING tournament(k=2)
    FROM parents SELECT offspring USING mutate_gaussian
    FROM population, offspring SELECT 10 population USING best
    YIELD population
END GENERATION
