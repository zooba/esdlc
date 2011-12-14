#pragma once

#include <list>
#include <memory>
#include <type_traits>
#include <vector>

#include <amp.h>

namespace esdl
{
    namespace tt
    {
        template<typename EvaluatorType>
        struct evaluator_invoke {
            template<typename GroupType> static void invoke(std::shared_ptr<EvaluatorType> ep, GroupType& group) {
                (*ep)(group);
            }
        };

        template<>
        struct evaluator_invoke<void> {
            template<typename GroupType> static void invoke(std::shared_ptr<void>, GroupType& group) { }
        };
    }

    template<typename IndividualType, typename EvaluatorType>
    class group
    {
        std::shared_ptr<concurrency::array<IndividualType, 1>> ptr;
    public:
        std::shared_ptr<EvaluatorType> evalptr;
        bool evaluated;

        void evaluate() {
            if (evalptr) {
                tt::evaluator_invoke<EvaluatorType>::invoke(evalptr, *this);
                evaluated = true;
            }
        }

        template<typename NewEvaluatorType>
        group<IndividualType, NewEvaluatorType> evaluate_using(std::shared_ptr<NewEvaluatorType> newEval) {
            return group<IndividualType, NewEvaluatorType>(ptr, newEval);
        }
        
        group() : ptr(nullptr), evalptr(nullptr), evaluated(false) { }
        group(int size, std::shared_ptr<EvaluatorType> ep)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>(size, esdl::acc)), evalptr(ep), evaluated(false) { }
        group(std::shared_ptr<concurrency::array<IndividualType, 1>> p, std::shared_ptr<EvaluatorType> ep)
            : ptr(p), evalptr(ep), evaluated(false) { }
        group(const concurrency::array<IndividualType, 1>& other, std::shared_ptr<EvaluatorType> ep)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>(other, esdl::acc)), evalptr(ep), evaluated(false) { }
        group(const std::list<IndividualType>& list, std::shared_ptr<EvaluatorType> ep)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>((int)list.size(), std::begin(list), std::end(list), esdl::acc)), 
            evalptr(ep), evaluated(false) { }
        group(const std::vector<IndividualType>& list, std::shared_ptr<EvaluatorType> ep)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>((int)list.size(), std::begin(list), std::end(list), esdl::acc)),
            evalptr(ep), evaluated(false) { }
        
        operator bool() const { return (bool)ptr; }
        concurrency::array<IndividualType, 1>& operator*() { return *ptr; }
        const concurrency::array<IndividualType, 1>& operator*() const { return *ptr; }
        concurrency::array<IndividualType, 1>* operator->() { return ptr.get(); }
        const concurrency::array<IndividualType, 1>* operator->() const { return ptr.get(); }

        operator std::list<IndividualType>() const {
            if (!ptr) return std::list<IndividualType>();

            std::list<IndividualType> result;
            copy(*ptr, std::back_inserter(result));
            return result;
        }
        std::list<IndividualType> as_list() const { return (std::list<IndividualType>)(*this); }

        operator std::vector<IndividualType>() const {
            if (!ptr) return std::vector<IndividualType>();
            
            std::vector<IndividualType> result;
            result.reserve(1000);
            copy(*ptr, std::back_inserter(result));
            return result;
        }

        std::vector<IndividualType> as_vector() const { return (std::vector<IndividualType>)(*this); }

        int size() const { return ptr ? ptr->extent.size() : 0; }
        void reset() { ptr.reset(); evaluated = false; }
    };

    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group() {
        return group<IndividualType, EvaluatorType>();
    }

    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(int size) {
        return group<IndividualType, EvaluatorType>(size, nullptr);
    }

    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(int size, std::shared_ptr<EvaluatorType> ep) {
        return group<IndividualType, EvaluatorType>(size, ep);
    }

    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(group<IndividualType, EvaluatorType> other) {
        return group<IndividualType, EvaluatorType>(*other, other.evalptr);
    }

    /*template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(const std::vector<IndividualType>& other) {
        return group<IndividualType, EvaluatorType>(other, nullptr);
    }
    
    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(const std::list<IndividualType>& other) {
        return group<IndividualType, EvaluatorType>(other, nullptr);
    }*/

    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(const std::vector<IndividualType>& other, std::shared_ptr<EvaluatorType> ep) {
        return group<IndividualType, EvaluatorType>(other, ep);
    }
    
    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(const std::list<IndividualType>& other, std::shared_ptr<EvaluatorType> ep) {
        return group<IndividualType, EvaluatorType>(other, ep);
    }
}
