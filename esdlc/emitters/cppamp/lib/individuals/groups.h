#pragma once

#include <list>
#include <memory>
#include <type_traits>
#include <vector>

#include <amp.h>

namespace esdl
{
    template<typename IndividualType, typename EvaluatorType>
    class group
    {
        std::shared_ptr<concurrency::array<IndividualType, 1>> ptr;
    public:
        bool evaluated;

        typename std::enable_if<std::is_constructible<EvaluatorType>::value>::type
        evaluate() {
            EvaluatorType()(*this);
            evaluated = true;
        }

        template<typename NewEvaluatorType>
        group<IndividualType, NewEvaluatorType> evaluate_using(NewEvaluatorType) {
            return group<IndividualType, NewEvaluatorType>(ptr);
        }
        
        group() : ptr(nullptr), evaluated(false) { }
        group(int size) : ptr(std::make_shared<concurrency::array<IndividualType, 1>>(size, esdl::acc)), evaluated(false) { }
        group(std::shared_ptr<concurrency::array<IndividualType, 1>> p) : ptr(p), evaluated(false) { }
        group(const concurrency::array<IndividualType, 1>& other) : ptr(std::make_shared<concurrency::array<IndividualType, 1>>(other, esdl::acc)), evaluated(false) { }
        group(const std::list<IndividualType>& list)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>(list.size(), std::begin(list), std::end(list), esdl::acc)), evaluated(false) { }
        group(const std::vector<IndividualType>& list)
            : ptr(std::make_shared<concurrency::array<IndividualType, 1>>(list.size(), std::begin(list), std::end(list), esdl::acc)), evaluated(false) { }
        group(nullptr_t) : ptr(nullptr), evaluated(false) { }

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
        return group<IndividualType, EvaluatorType>(nullptr);
    }

    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(int size) {
        return group<IndividualType, EvaluatorType>(size);
    }

    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(const group<IndividualType, EvaluatorType>& other) {
        return group<IndividualType, EvaluatorType>(*other);
    }

    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(const std::vector<IndividualType>& other) {
        return group<IndividualType, EvaluatorType>(other);
    }
    
    template<typename IndividualType, typename EvaluatorType>
    group<IndividualType, EvaluatorType> make_group(const std::list<IndividualType>& other) {
        return group<IndividualType, EvaluatorType>(other);
    }
}
