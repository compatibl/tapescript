# TapeScript

TapeScript is an open source library for adjoint algorithmic differentiation
(AAD) developed and maintained by CompatibL. It can be downloaded from
github.com/compatibl and used free of charge in academic or commercial applications.

TapeScript supports vector AAD (tape compression), an approach in which
each slot of the calculation record (AAD tape) can store not only a
single double number, but also an entire array of values. Vector AAD
can lead to performance gain of several orders of magnitude due to the reduction
of tape size.

### TapeScript features:

* Scalar AAD
* Vector AAD (tape compression)
* APIs for C++, C#, and Java
* Complex numbers
* Works with Boost and QuantLib
* Multithreading support

## About CompatibL

CompatibL offers turnkey solutions for XVA and regulatory capital
as well as custom development, integration, and consultancy.

Check out TapeLib, CompatibL's product suite for AAD that incorporates:

* A C++ library extending TapeScript with features specific to quantitative finance
* An application platform for interactive AAD

### TapeLib features:

* Document database preserving AAD data
* Tape database
* Finance-specific atomics including adjointable AMC
* User defined atomics
* Tape cutting and splicing
* Parallel tape execution
* Specialized gate checking API
* Excel addin, desktop and client
