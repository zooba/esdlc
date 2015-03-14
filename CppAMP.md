# Introduction

`esdlc` provides an emitter and operator library for C++ AMP, allowing ESDL systems to be compiled for and executed with GPU acceleration.

# Dependencies

 * Visual Studio 2012 or later
 * [Python 2.7](http://www.python.org) (for compilation only)


# Command Line

The command must be invoked in the directory containing the `*.esdl` file; included headers are referenced relative to this directory. This means that a full path reference is required to the `esdlc.py` script.

```
python \full\path\to\esdlc.py /e:cppamp /o:example.cpp example.esdl
python \full\path\to\esdlc.py /e:cppamp /o:example.exe example.esdl
```

The `/e` option is required to select the C++ AMP emitter. The `/o` option is optional: omitting it will write the generated C++ code to standard output. If an executable is specified (eg. "example.exe"), the C++ code is written to "example.cpp" and the compiler is invoked; an error is raised if `cl` is not in the path. Starting `esdlc` within the VS11 Developer Command Prompt is recommended.

Specifying the `/P` option includes spans for use with the [Concurrency Visualizer](http://msdn.microsoft.com/en-us/library/dd537632.aspx). If the output file is an executable, the correct headers and libraries will be included. In particular, the path to the concurrency SDK (`%VSINSTALLDIR%Common7\IDE\Extensions\Microsoft\Concurrency Visualizer\SDK\Native\Inc`) must be specified first to override the stub `cvmarkers.h` file included with `esdlc`.

The compilation options used can be seen in [the source](esdlc/emitters/cppamp/__init__.py#659).

# Running

Compiled executables have the following options: 

| Option | Description |
| ------ | ----------- |
| /iteration (/iter, /i) `integer` | Stop after `integer` iterations |
| /fitness (/fit, /f) `fitness` | Stop when the best fitness is `fitness` or better |
| /evaluations (/evals, /e) `integer` | Stop after `integer` fitness evaluations |
| /csv | Write CSV output the console |
| /verbose (/v) | Write detailed output |
| /quiet (/q) | Writes no output |
| /seed (/s) `integer` | Specify the seed value. |
| `name`=`float` | Set parameter `name` to `float` |

The first three options are termination conditions; if none are specified, the algorithm will pause after each iteration and wait for user input. Pressing 'x' will exit the algorithm and any other key will continue.

All uninitialised variables must be specified on the command line.

*The current version generates code that requires _all_ command line options to include a parameter.* Use `/verbose:` and `/quiet:` or `/v:` and `/q:`.

*The current version does not support CSV output.*

# Example

A simple ES could be specified with the following ESDL:
```
`include "sphere.h"
`evaluator sphere()
length = 10

FROM random_real(length, lowest=-10, highest=10) SELECT (mu) population
YIELD population

BEGIN generation
    FROM population SELECT (lambda) parents USING repeated
    FROM parents SELECT offspring USING mutate_gaussian(sigma)

    FROM population, offspring SELECT (mu) population USING fitness_sus
    YIELD population
END
```

The `sphere.h` file contains the following definition:
```cpp
#pragma once

template<typename IndividualType>
struct sphere_t
{
    sphere_t() { }
    void operator()(esdl::group<IndividualType, sphere_t<IndividualType>>& source) {
        if (source.evaluated) return;
        auto& src = *source;

        parallel_for_each(esdl::acc, src.grid, [&](index<1> i) restrict(direct3d) {
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
sphere_t<IndividualType> sphere() {
    return sphere_t<IndividualType>();
}

template<typename IndividualType, typename EvaluatorType>
sphere_t<IndividualType> sphere(esdl::group<IndividualType, EvaluatorType>) {
    return sphere_t<IndividualType>();
}
```

This can be compiled with:
```
python.exe "full\path\to\esdlc.py" /e:cppamp /o:es.exe es.esdl
```

and executed with:
```
es.exe /iter 100 sigma=1 mu=4 lambda=10
```

The `length` parameter (number of values in each individuals) must be specified in the ESDL definition; it has to be compiled into the executable. The other parameters (`sigma`, `mu` and `lambda`) can be set each time the compiled algorithm is run.


# Limitations

The emitter is currently a prototype and there are a huge range of limitations. Anyone else interested in fixing/adding functionality and submitting patches is more than welcome.
