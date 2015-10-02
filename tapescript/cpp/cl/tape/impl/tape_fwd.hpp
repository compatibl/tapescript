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

#if defined CL_TAPE_INNER_ARRAY_ENABLED
    template <class Array> struct inner_array;
#endif

#if defined CL_TAPE_GEN_ENABLED
    ///<summary> codegeneration based type </summary>
    typedef CppAD::cg::CG<double> CgBaseType;

    /// <summary> TapeDouble fwd declaration </summary>
    typedef tape_double<CgBaseType> TapeDouble;
#else
    typedef tape_double<double> TapeDouble;
#endif

    ///<summary> Architeht </summary>
    struct dummy;

    template <typename Ty_>
    struct solve_dummy { typedef dummy type; };
}

namespace CppAD
{
    /// Forward declaration about serialization
    template <typename T>
    struct tape_serializer;

    /// Case when we don't have implement tag
    template <typename Ty_, typename Ch_ = cl::dummy>
    struct is_implemented : std::false_type
    {
        typedef cl::dummy impl_type;
    };

    /// Case when we have impl tag
    template <typename Ty_>
    struct is_implemented<Ty_, typename cl::solve_dummy<typename Ty_::impl>::type> : std::true_type
    {
        typedef typename
            Ty_::impl impl_type;
    };
}

#endif