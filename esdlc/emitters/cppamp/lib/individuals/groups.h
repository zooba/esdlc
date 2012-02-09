#pragma once

#include <list>
#include <memory>
#include <type_traits>
#include <vector>

#include <amp.h>

namespace esdl
{
    struct _evaluator_erased_t {
        virtual void evaluate() = 0;
    };

    template<typename GroupType, typename EvaluatorType>
    struct _evaluator_t : _evaluator_erased_t {
        void evaluate() {
            (*ptr)(group);
            group.evaluated = true;
        }

        _evaluator_t(GroupType& group, std::shared_ptr<EvaluatorType> evalptr)
            : group(group), ptr(evalptr) { }

    private:
        std::shared_ptr<EvaluatorType> ptr;
        GroupType& group;
    };

    template<typename GroupType>
    struct _evaluator_t<GroupType, void> : _evaluator_erased_t {
        void evaluate() { }
        _evaluator_t(GroupType&, void*) { }
    };

    template<typename IndividualType>
    class group
    {
        std::shared_ptr<concurrency::array<IndividualType, 1>> ptr;
        std::shared_ptr<_evaluator_erased_t> evalptr;
    public:
        bool evaluated;

        void evaluate() {
            if (evalptr) {
                evalptr->evaluate();
            }
        }

        template<typename EvaluatorType>
        void evaluate_using(std::shared_ptr<EvaluatorType> newEval) {
            evalptr = std::make_shared<_evaluator_t<group<IndividualType>, EvaluatorType>>(*this, newEval);
            evaluated = false;
        }

        void evaluate_using(const group<IndividualType>& other) {
            evalptr = other.evalptr;
            evaluated = false;
        }
        
        group() : ptr(nullptr), evalptr(nullptr), evaluated(false) { }
        group(int size)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>(size, esdl::acc)), evaluated(false) { }
        group(std::shared_ptr<concurrency::array<IndividualType, 1>> p)
            : ptr(p), evaluated(false) { }
        group(const group<IndividualType>& other, bool deepCopy)
            : ptr(deepCopy ? std::make_shared<concurrency::array<IndividualType, 1>>(*other, other->accelerator_view) : other.ptr),
            evalptr(other.evalptr), evaluated(other.evaluated) { }
        group(const concurrency::array<IndividualType, 1>& other)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>(other, other.accelerator_view)), evaluated(false) { }
        group(const std::list<IndividualType>& list)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>((int)list.size(), std::begin(list), std::end(list), esdl::acc)), 
            evaluated(false) { }
        group(const std::vector<IndividualType>& list)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>((int)list.size(), std::begin(list), std::end(list), esdl::acc)),
            evaluated(false) { }
        
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
            copy(*ptr, std::back_inserter(result));
            return result;
        }

        std::vector<IndividualType> as_vector() const { return (std::vector<IndividualType>)(*this); }

        int size() const { return ptr ? ptr->extent.size() : 0; }
        void reset() { ptr.reset(); evaluated = false; }
    };

    template<typename IndividualType>
    group<IndividualType> make_group() {
        return group<IndividualType>();
    }

    template<typename IndividualType>
    group<IndividualType> make_group(int size) {
        return group<IndividualType>(size);
    }

    template<typename IndividualType>
    group<IndividualType> make_group(group<IndividualType> other) {
        return group<IndividualType>(other, true);
    }

    template<typename IndividualType>
    group<IndividualType> make_group(const std::vector<IndividualType>& other) {
        return group<IndividualType>(other);
    }
    
    template<typename IndividualType>
    group<IndividualType> make_group(const std::list<IndividualType>& other) {
        return group<IndividualType>(other);
    }
}
