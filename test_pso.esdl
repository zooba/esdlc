FROM random_pso(length=config.landscape.size.exact, \
                lowest=config.landscape.bounds[0],highest=config.landscape.bounds[1], \
                position_bounds=config.landscape.bounds, \
                velocity_bounds=config.landscape.bounds) \
        SELECT (size) population
FROM population SELECT 1 global_best USING best_only
FROM population SELECT (size) p_bests
YIELD population

BEGIN GENERATION
    JOIN population, p_bests INTO pairs USING tuples
    
    FROM pairs SELECT population USING \
         update_velocity(global_best=global_best, w=inertia, c1=c1, c2=c2, constriction=True), \
         update_position_clamp
    
    JOIN population, p_bests INTO pairs USING tuples
    FROM pairs SELECT p_bests USING best_of_tuple
    
    FROM population, global_best SELECT 1 global_best USING best_only
    
    YIELD global_best, population
END GENERATION
