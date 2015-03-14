`esdlc` is a multi-targeting compiler for Evolutionary System Definition Language (ESDL)

A target ("emitter") in this context refers to an underlying library or framework, rather than a specific operating system or processor. `esdlc` is also designed to be embeddable within a framework, allowing users to avoid 

For example, `esdlc` currently provides an emitter for [esec](http://github.com/zooba/esec), as well as being embedded in `esec`.

[Python](http://www.python.org/) 2.6 or 2.7 is required to run `esdlc`, and emitters may have added requirements, though the produced code only requires Python if that is the target. A (currently hypothetical) C++ emitter may produce a `.cpp` file, which needs a C++ compiler but does not require Python.
