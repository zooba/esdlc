FROM random_pso(length=30, lowest=15, highest=30, lower_position_bound=-100, upper_position_bound=100) SELECT (size) population
FROM population SELECT 1 global_best USING best_only
FROM population SELECT (size) p_bests
YIELD population

BEGIN pso_generation
    JOIN population, p_bests INTO pairs USING tuples
    FROM pairs SELECT population USING update_velocity(global_best=global_best,w=0.7,c1=1.4,c2=1.4), \
                                       update_position_clamp
    
    # Update individual best known locations
    JOIN population, p_bests INTO pairs USING tuples
    FROM pairs SELECT p_bests USING best_of_tuple
    
    # Update global best known location
    FROM population, global_best SELECT 1 global_best USING best_only
    
    YIELD population
END pso_generation

BEGIN eo_generation_1
    REPEAT (size)
        FROM population SELECT 1 candidate, rest USING rank_unproportional_no_replacement
        FROM candidate SELECT replacement USING mutate_random(genes=1)
        FROM replacement, rest SELECT population
        YIELD population
    END
    
    # Uncomment to forget the previous global best
    #FROM population SELECT 1 global_best USING best_only
    # Uncomment to forget the previous personal bests
    FROM population SELECT (size) p_bests
END

BEGIN eo_generation_2
    JOIN population INTO replacements USING clone_tuples(count=30)
    FROM replacements SELECT replacements USING mutate_one_gene_each(tuples=True)
    FROM replacements SELECT population USING best_of_tuple
    YIELD population
    
    # Uncomment to forget the previous global best
    #FROM population SELECT 1 global_best USING best_only
    # Uncomment to forget the previous personal bests
    FROM population SELECT (size) p_bests
END

BEGIN eo_generation_3
    REPEAT (size)
        FROM population SELECT 1 candidate, rest USING uniform_shuffle
        FROM candidate SELECT 30 candidates USING repeated, mutate_one_gene_each
        FROM candidates SELECT 1 replacement USING best_only
        FROM rest, replacement SELECT population
    END
    YIELD population
    
    # Uncomment to forget the previous global best
    #FROM population SELECT 1 global_best USING best_only
    # Uncomment to forget the previous personal bests
    FROM population SELECT (size) p_bests
END

BEGIN eo_generation_4
    FROM population SELECT 1 candidate, rest USING rank_unproportional_no_replacement
    FROM candidate SELECT replacement USING mutate_random(genes=1)
    FROM replacement, rest SELECT population
    YIELD population
    
    # Uncomment to forget the previous global best
    #FROM population SELECT 1 global_best USING best_only
    # Uncomment to forget the previous personal bests
    FROM population SELECT (size) p_bests
END
