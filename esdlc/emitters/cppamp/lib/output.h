#pragma once
#include "statistics.h"

template<typename T>
struct _output {
    static const int WIDTH = 10;

    static void write(std::wostream& stream, const T& value) {
        stream << value;
    }

    static void writeRaw(std::wostream& stream, const T& value) {
        stream << value;
    }
};

namespace esdl
{

    template<typename T>
    void write_group(const wchar_t* name, const std::vector<T>& group) {
        std::wcout << L"Yielded " << group.size() << L" " << name << std::endl;

        std::for_each(std::begin(group), std::end(group), [](const T& indiv) {
            _output<float>::write(std::wcout, indiv.fitness);
            std::wcout << L" | ";
            _output<T>::write(std::wcout, indiv);
            std::wcout << std::endl;
        });

        std::wcout << std::endl;
    }

    template<typename T>
    void write_group_raw(const wchar_t* name, const std::vector<T>& group) {
        std::wcout << L"Iteration, Block, Group Size, Group Name" << std::endl;
        std::wcout << _iteration << L", " << _current_block << L", " << group.size() << L", " << name << std::endl << std::endl;

        std::wcout << L"Fitness, Genome" << std::endl;
        std::for_each(std::begin(group), std::end(group), [](const T& indiv) {
            _output<float>::writeRaw(std::wcout, indiv.fitness);
            std::wcout << L", ";
            _output<T>::writeRaw(std::wcout, indiv);
            std::wcout << std::endl;
        });

        std::wcout << std::endl;
    }

    template<typename IndividualType>
    void write_statistics_header() {
        const int WIDTH = _output<float>::WIDTH;
    
        std::wcout << L"Iter  Evals | "
            << std::left << std::setw(WIDTH) << std::setfill(L' ') << L"Best" << L"|";
    
        std::wcout << std::right << std::setw(_output<float>::WIDTH) << "Min. ";
        std::wcout << std::right << std::setw(_output<float>::WIDTH) << "Mean. ";
        std::wcout << std::right << std::setw(_output<float>::WIDTH) << "Med. ";
        std::wcout << std::right << std::setw(_output<float>::WIDTH) << "Max. ";

        std::wcout << std::endl;

        std::wcout << L"------------+-" << std::wstring(WIDTH, L'-') << "+"
            << std::wstring((_output<float>::WIDTH*4), L'-') << std::endl;
    }

    template<typename IndividualType>
    void write_statistics(const Statistics& stats, const IndividualType& best) {
        std::wcout << std::setw(5) << _iteration << std::setw(6) << _evaluations << L" | ";
        _output<float>::write(std::wcout, best.fitness);
        std::wcout << L'|';
        _output<float>::write(std::wcout, stats.min_fitness);
        _output<float>::write(std::wcout, stats.mean_fitness);
        _output<float>::write(std::wcout, stats.median_fitness);
        _output<float>::write(std::wcout, stats.max_fitness);
        std::wcout << std::endl;
    }

    template<typename IndividualType>
    void write_statistics_footer(EndReason reason, const Statistics& stats, const IndividualType& best) {
        switch (reason)
        {
        case IterLimit:
            std::wcout << L"Iteration limit reached" << std::endl << std::endl;
            break;
        case EvalLimit:
            std::wcout << L"Evaluation limit reached" << std::endl << std::endl;
            break;
        case FitLimit:
            std::wcout << L"Fitness limit reached" << std::endl << std::endl;
            break;
        default:
            std::wcout << std::endl;
            break;
        }
    
        std::wostringstream fitnessBuilder;
        _output<float>::write(fitnessBuilder, best.fitness);
        std::wstring fitness = fitnessBuilder.str();

        std::wcout << L"Iter  Evals | "
            << std::left << std::setw(fitness.size()) << L"Fitness "
            << "|  Genome" << std::endl;
    
        std::wcout << L"------------+-"
            << std::left << std::setw(fitness.size()) << std::setfill(L'-') << "--------"
            << "+--------------------------------" << std::endl;
    
        std::wcout << std::setw(5) << _iteration << std::setw(6) << _evaluations << L" |"
            << std::left << std::setw(8) << fitness << L" | ";
        _output<IndividualType>::write(std::wcout, best);
        std::wcout << std::endl;
    }
}

template<>
struct _output<float> {
    static const int WIDTH = 12;

    static void write(std::wostream& dest, const float& value) {
        dest << L' ' << std::setw(11) << std::fixed << value;
    }

    static void writeRaw(std::wostream& dest, const float& value) {
        dest << value;
    }
};
