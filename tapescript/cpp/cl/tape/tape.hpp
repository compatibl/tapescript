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

#ifndef __cl_tape_doublecl_hpp__
#define __cl_tape_doublecl_hpp__

namespace cl
{
    template <typename Base>
    struct remove_ad;
}

#if defined CL_TAPE_CPPAD
#include <cppad/cppad.h>
#include <cppad/local/ad.hpp>

namespace ext = CppAD;

namespace cl
{
    typedef CppAD::AD<double> tape_double;
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
    typedef double tape_double;
}
namespace ext
{
    template <typename Type>
    inline typename Type Value(Type const& v)
    {   return v; }
}
#endif

#include <cl/tape/impl/double.hpp>

#if defined CL_TAPE_COMPLEX_ENABLED
#   include <cl/tape/impl/complex.hpp>
#endif

#include <cl/tape/impl/doublelimits.hpp>
#include <cl/tape/impl/doubleoperators.hpp>
#include <cl/tape/impl/doublemath.hpp>

/// Adaptation adjoint framework essencies
namespace cl
{
    template <typename > struct empty_template {};

#if defined CL_TAPE && defined CL_TAPE_CPPAD

    template <typename Type>
    using AdjFunBase = CppAD::ADFun<Type>;
    typedef std::vector<cl::CppDouble::value_type > AdjVectorBase;

    inline void Independent(AdjVectorBase& v)
    {
        CppAD::Independent(v);
    }

    inline void Independent(AdjVectorBase& v, std::size_t abort_index)
    {
        CppAD::Independent(v, abort_index);
    }

#elif defined CL_TAPE_ADOLC

    template <typename Type>
    using AdjFunBase = empty_template<Type>;
    typedef std::vector<cl::CppDouble::value_type > AdjVectorBase;

    inline void Independent(AdjVectorBase& v)
    {
    }

    inline void Independent(AdjVectorBase& v, std::size_t abort_index)
    {
    }
#else
    template <typename Type>
    using AdjFun = empty_template<Type>;

    typedef std::vector<cl::CppDouble> AdjVectorBase;

    inline void Independent(AdjVectorBase& v)
    {
    }

    inline void Independent(AdjVectorBase& v, std::size_t abort_index)
    {
    }
#endif
}

#include <cl/tape/impl/adjointref.hpp>
#include <cl/tape/impl/adjointrefoperator.hpp>

#endif // __cl_tape_doublecl_hpp__
