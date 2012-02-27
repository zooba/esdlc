#pragma once

#include <memory>
#include <list>
#include <type_traits>
#include <vector>
#include <amp.h>

namespace esdl
{
    struct _nil { };
    
    template<int JoinCount, typename G0, typename G1, typename G2 = _nil, typename G3 = _nil, typename G4 = _nil>
    class joinedgroup;

    template<typename G0, typename G1>
    class joinedgroup<2, G0, G1>
    {
    public:
        G0 group1;
        G1 group2;
        bool evaluated;

        template<typename EvaluatorType>
        void evaluate_using(const std::shared_ptr<EvaluatorType>&) {
            static_assert(false, "evaluate_using is not yet implemented");
        }

        joinedgroup() : group1(), group2(), evaluated(false) { }
        joinedgroup(G0 group1, G1 group2)
            : group1(group1), group2(group2), evaluated(group1.evaluated && group2.evaluated) { }

        operator bool() const { return (bool)group1 && (bool)group2; }

        int size() const { return group1 ? group1.size() : 0; }
        void reset() { group1.reset(); group2.reset(); evaluated = false; }

        template<int Index> void get() { }
        template<> G0& get<1>() { return group1; }
        template<> G1& get<2>() { return group2; }
    };

    template<typename G0, typename G1, typename G2>
    class joinedgroup<3, G0, G1, G2>
    {
    public:
        G0 group1;
        G1 group2;
        G2 group3;
        bool evaluated;

        template<typename EvaluatorType>
        joinedgroup<3, G0, G1, G2> evaluate_using(const std::shared_ptr<EvaluatorType>&) {
            static_assert(false, "evaluate_using is not yet implemented");
        }

        
        joinedgroup() : group1(), group2(), group3(), evaluated(false) { }
        joinedgroup(G0 group1, G1 group2, G2 group3)
            : group1(group1), group2(group2), group3(group3),
            evaluated(group1.evaluated && group2.evaluated && group3.evaluated) { }

        operator bool() const { return (bool)group1 && (bool)group2 && (bool)group3; }

        int size() const { return group1 ? group1.size() : 0; }
        void reset() { group1.reset(); group2.reset(); group3.reset(); evaluated = false; }

        template<int Index> void get() { }
        template<> G0& get<1>() { return group1; }
        template<> G1& get<2>() { return group2; }
        template<> G2& get<3>() { return group3; }
    };

    namespace tt
    {
        template<typename T>
            struct is_joined_group : std::false_type { };
        template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4>
            struct is_joined_group<esdl::joinedgroup<JoinCount, G0, G1, G2, G3, G4>> : std::true_type { };
        
        template<typename T>
            struct joined_count { };
        template<typename T, int Index>
            struct joined_group_type { };
        template<typename T, int Index>
            struct joined_individual_type
            { typedef typename individual_type<typename joined_group_type<T, Index>::type>::type type; };

        template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4>
            struct joined_count<esdl::joinedgroup<JoinCount, G0, G1, G2, G3, G4>> : std::integral_constant<int, JoinCount> { };
        template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4>
            struct joined_group_type<esdl::joinedgroup<JoinCount, G0, G1, G2, G3, G4>, 0> { typedef G0 type; };
        template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4>
            struct joined_group_type<esdl::joinedgroup<JoinCount, G0, G1, G2, G3, G4>, 1> { typedef G1 type; };
        template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4>
            struct joined_group_type<esdl::joinedgroup<JoinCount, G0, G1, G2, G3, G4>, 2> { typedef G2 type; };
        template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4>
            struct joined_group_type<esdl::joinedgroup<JoinCount, G0, G1, G2, G3, G4>, 3> { typedef G3 type; };
        template<int JoinCount, typename G0, typename G1, typename G2, typename G3, typename G4>
            struct joined_group_type<esdl::joinedgroup<JoinCount, G0, G1, G2, G3, G4>, 4> { typedef G4 type; };
    }
}
