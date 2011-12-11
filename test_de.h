#pragma once

template<typename SourceType>
class mutate_de_t {
    SourceType source;
    float scale;
    float cr;

    typedef typename esdl::tt::joined_individual_type<SourceType, 0>::type IndividualType;
    typedef typename esdl::tt::joined_evaluator_type<SourceType, 0>::type EvaluatorType;
public:
    static_assert(esdl::tt::joined_count<SourceType>::value == 3, "mutate_de requires three joined individuals");
    static_assert(
        std::is_same<typename esdl::tt::joined_individual_type<SourceType, 0>::type, typename esdl::tt::joined_individual_type<SourceType, 1>::type>::value &&
        std::is_same<typename esdl::tt::joined_individual_type<SourceType, 0>::type, typename esdl::tt::joined_individual_type<SourceType, 2>::type>::value,
        "mutate_de requires joined individuals to be the same type");

    mutate_de_t(SourceType source, float scale, float cr) : source(source), scale(scale), cr(cr) { }

    esdl::group<IndividualType, EvaluatorType> operator()() {
        return mutate(source());
    }

    esdl::group<IndividualType, EvaluatorType> operator()(int count) {
        return mutate(source(count));
    }
private:
    esdl::group<IndividualType, EvaluatorType> mutate(typename esdl::tt::group_type<SourceType>::type src) {
        auto pResult = esdl::make_group<IndividualType, EvaluatorType>(src.size());

        auto& base = *src.group1;
        auto& v1 = *src.group2;
        auto& v2 = *src.group3;
        auto& dest = *pResult;

        auto rand = random_array(src.size(), esdl::tt::length<IndividualType>::value);

        const float _scale = scale;
        const float _cr = cr;
        parallel_for_each(grid<2>(extent<2>(pResult.size(), esdl::tt::length<IndividualType>::value)),
            [&, _scale, _cr](index<2> i) restrict(direct3d) {
            dest(i.y).genome[i.x] = base(i.y).genome[i.x] + (rand[i] < _cr ? _scale * (v1(i.y).genome[i.x] - v2(i.y).genome[i.x]) : 0);
        });

        return pResult;
    }
};

// ESDL operator: mutate_de(scale=(float)1,crossover_rate=(float)0.9)
template<typename SourceType>
mutate_de_t<SourceType> mutate_de(SourceType source, float scale, float cr) {
    return mutate_de_t<SourceType>(source, scale, cr);
}
