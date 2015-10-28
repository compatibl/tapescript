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

# include <valarray>

namespace CppAD
{
    /// CppAd forward declaration few classes

    template <typename Base>
    class AD;

    template <typename Base>
    class ADFun;
}

/// <summary>Adapter for types convertible to double.</summary>
namespace cl
{
#if defined CL_TAPE_CPPAD
    template <typename Base>
    using tape_inner_type = CppAD::AD<Base>;

    template <typename Base>
    using tape_function_base = CppAD::ADFun<Base>;
#elif CL_TAPE_ADOLC
    template <typename Base>
    using tape_inner_type = Adolc::DoubleAdapter<Base>;
#else
    template <typename Base>
    struct tape_inner_type {    };
#endif
    /// <summary>Alias on std reference wrapper type
    /// it used for preventing native type specification
    /// for adapted types.</summary>
    template <typename Type>
    using ref_type = std::reference_wrapper<Type>;

    /// <summary>tape_double forward declaration</summary>
    template<typename Base>
    class tape_wrapper;

#if defined CL_TAPE_GEN_ENABLED
    /// <summary>Code generation base type.</summary>
    typedef CppAD::cg::CG<double> tape_cg_base_type;

    /// <summary> tape_double fwd declaration </summary>
    typedef tape_wrapper<tape_cg_base_type> tape_double;
#else
    typedef tape_wrapper<double> tape_double;
#endif

    /// <summary>Empty structure.
    /// Used as default argument for validsating alternative in SFINAE</summary>
    struct dummy;

    /// <summary>Compiletime use for trying
    /// generate syntax constructions in SFINAE princip</summary>
    template <typename Ty_>
    struct solve_dummy { typedef dummy type; };

    /// <summary>Tape function is a compatible external functional implementation
    /// this should be suitable inside external framework.</summary>
    template <typename Base>
    class tape_function;

    template <class Array> struct tape_inner;
    typedef std::valarray<double> tape_array;
    typedef tape_inner<tape_array> tape_value;
    typedef tape_wrapper<tape_value> tape_object;
}

namespace cl
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

    /// Case when we don't have implement io_binary
    template <typename Ty_, typename Ch_ = cl::dummy>
    struct is_io_typed : std::false_type
    {
        typedef cl::dummy impl_type;
    };

    template <typename Ty_>
    struct is_io_typed<Ty_, typename cl::solve_dummy<typename Ty_::io_typed>::type > : std::true_type
    {
        typedef typename Ty_::io_typed io_typed;
    };
}

///<summary>Short type alias for tape based classes</summary>
namespace cl
{
    typedef cl::tape_double tdouble;
    typedef cl::tape_value tvalue;
    typedef cl::tape_array tarray;
    typedef cl::tape_object tobject;

    template <typename Base>
    using tfunc = cl::tape_function<Base>;
}

#endif