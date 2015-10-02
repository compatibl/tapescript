/*
Copyright (C) 2003-2015 CompatibL

This file is part of TapeScript, an open source library and tape encoding
standard for adjoint algorithmic differentiation (AAD), available from

http://github.com/compatibl/tapescript (source)
http://tapescript.org (documentation)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef cl_tape_fwd_doublecl_hpp
#define cl_tape_fwd_doublecl_hpp

/// <summary>
///    Adapter for types convertible to double.
/// </summary>
namespace cl
{
#if defined CL_TAPE_CPPAD
    template <typename Base>
    using TapeInnerType = CppAD::AD<Base>;

    template <typename Base>
    using TapeFunctionBase = CppAD::ADFun<Base>;
#elif CL_TAPE_ADOLC
    template <typename Base>
    using TapeInnerType = Adolc::DoubleAdapter<Base>;
#else
    template <typename Base>
    struct TapeInnerType {    };
#endif
    /// <summary>
    ///    Alias on std reference wrapper type
    /// it used for preventing native type specification
    /// for adapted types.
    /// </summary>
    template <typename Type>
    using ref_type = std::reference_wrapper<Type>;

    /// <summary>
    ///   TapeDouble forward declaration
    /// </summary>
    template<typename Base>
    class tape_double;

    typedef tape_double<double> TapeDouble;

    typedef cl::TapeDouble t_double;
}

#endif