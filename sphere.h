#pragma once

template<typename IndividualType>
struct sphere_t {
    sphere_t() { }
    void operator()(esdl::group<IndividualType, sphere_t<IndividualType>>& source) {
        if (source.evaluated) return;
        auto& src = *source;

        parallel_for_each(src.grid, [&](index<1> i) restrict(direct3d) {
            float sum = 0;
            for (int j = 0; j < esdl::tt::length<IndividualType>::value; ++j) {
                sum += src[i].genome[j] * src[i].genome[j];
            }
            src[i].fitness = -sum;
        });

        source.evaluated = true;
    }
};

template<typename IndividualType>
sphere_t<IndividualType> sphere() { return sphere_t<IndividualType>(); }

template<typename IndividualType, typename EvaluatorType>
sphere_t<IndividualType> sphere(esdl::group<IndividualType, EvaluatorType>) { return sphere_t<IndividualType>(); }
