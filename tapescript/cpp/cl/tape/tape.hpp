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

#ifndef cl_tape_doublecl_hpp
#define cl_tape_doublecl_hpp

namespace cl
{
    template <typename Base>
    struct remove_ad;
}

#if defined CL_TAPE_CPPAD

#if !defined CL_USE_NATIVE_FORWARD
// Lock forward1sweep include
#   define CPPAD_FORWARD1SWEEP_INCLUDED
// Lock forward1sweep include
#   define CPPAD_FORWARD0SWEEP_INCLUDED
#endif

// Lock undef include 
#define CPPAD_UNDEF_INCLUDED

#include <cppad/cppad.h>

#if !defined CL_USE_NATIVE_FORWARD
#   if !defined TYPE_SERIALIZER
#       define  TYPE_SERIALIZER CppAD::tape_serializer
#   endif
#   include <iostream>
#   include <cl/tape/impl/tape_fwd.hpp>
#   include <cl/tape/impl/ad/tape_forward0sweep.hpp>
#   include <cl/tape/impl/ad/tape_forward1sweep.hpp>
#   include <cl/tape/impl/ad/tape_serializer_fwd.hpp>
#   include <cl/tape/impl/ad/tape_serializer_call.hpp>
#   if defined CL_BASE_SERIALIZER_OPEN
#       include <cl/tape/impl/ad/tape_serializer.hpp>
#   endif
#endif

#if defined CL_TAPE_GEN_ENABLED
#   include <cppad/cg/cppadcg.hpp>
#endif

namespace ext = CppAD;

namespace cl
{
    typedef CppAD::AD<double> cppad_double;
}
#elif defined CL_TAPE_ADOLC
#include <adolc/adouble.h>
namespace adolc {
    template <typename Type>
    inline void Independent(Type const&) { }
}
#else
namespace cl
{
    typedef double cppad_double;
}
namespace ext
{
    template <typename Type>
    inline typename Type Value(Type const& v)
    {   return v; }
}
#endif

#include <cl/tape/impl/tape_fwd.hpp>

#include <cl/tape/impl/double.hpp>

#if defined CL_TAPE_COMPLEX_ENABLED
#   include <cl/tape/impl/complex.hpp>
#endif

#include <cl/tape/impl/adjointref.hpp>
#include <cl/tape/impl/doublelimits.hpp>
#include <cl/tape/impl/doubleoperators.hpp>
#include <cl/tape/impl/doublemath.hpp>

#if defined CL_TAPE_COMPLEX_ENABLED
#   include <cl/tape/impl/traits.hpp>
#endif

#if defined CL_TAPE_INNER_ARRAY_ENABLED
#   include <cl/tape/impl/inner/tape_array.hpp>
#endif

/// Adaptation adjoint framework essences
namespace cl
{
#if defined CL_TAPE && defined CL_TAPE_CPPAD
    //   The scope of fwd implementation
    // which is not need to base functionality
    // but need to derived
#elif defined CL_TAPE_ADOLC
    template <typename Type>
    using TapeFunctionBase = empty_template<Type>;
    typedef std::vector<cl::TapeDouble::value_type > AdjVectorBase;

    inline void Independent(AdjVectorBase& v)
    {
    }

    inline void Independent(AdjVectorBase& v, std::size_t abort_index)
    {
    }
#else
    template <typename Type>
    using AdjFun = empty_template<Type>;

    typedef std::vector<cl::TapeDouble> AdjVectorBase;

    inline void Independent(AdjVectorBase& v)
    {
    }

    inline void Independent(AdjVectorBase& v, std::size_t abort_index)
    {
    }
#endif
}

#include <cl/tape/impl/adjointrefoperator.hpp>

# undef CPPAD_UNDEF_INCLUDED
# include <cppad/local/undef.hpp>

#endif // cl_tape_doublecl_hpp
