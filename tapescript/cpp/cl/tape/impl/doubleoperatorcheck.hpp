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

#ifndef __cl_tape_impl_doubleoperatorcheck_hpp__
#define __cl_tape_impl_doubleoperatorcheck_hpp__

#if defined CL_TAPE_CPPAD
#include <cppad/cppad.hpp>
#endif

//!! Supporting code for double operators, in progress

namespace cl
{
    class TapeDouble;

    struct empty_type {};

    template <typename T>
    struct certain_type
    {
        typedef T type;
    };

    template <typename T, int size>
    struct certain_type<T[size]>
    {
        typedef T* type;
    };

    template<typename T>
    inline typename certain_type<T>::type
    get_instance();

    namespace detail
    {
        struct dummy {};

        template <typename Type, Type V>
        struct extract_dummy {
            typedef dummy Dummy;
            enum { value = 0 };
        };

        template <typename Type, typename Return>
        struct oper_signature {
            typedef Return(Type::*type)();
        };

        template <typename Type
            , typename Return, Return(Type::*Ref)()>
        struct extract_dummy<Return(Type::*)(), Ref>
        {
            typedef dummy Dummy;
            enum { value = 1 };
        };

        // Typedef is not visible outside the detail namespace
        typedef cl::TapeDouble double_type;

        template <typename Type, typename V = dummy>
        struct is_has_operator_real
        {
            enum { value = 0, is_const = 0 };
            typedef std::integral_constant<bool, value > type;
        };

        typedef cl::TapeDouble double_type;

        // This is partial specialization of SFINAE
        // when we have a operator to convert to Double
        template <typename Type>
        struct is_has_operator_real
        <
            Type
            , typename extract_dummy
                <
                    double_type (Type::*)(), &Type::operator double_type
                >::Dummy
        >
        {
            enum { value = 1, is_const = 0 };
            typedef std::integral_constant<bool, value > type;
        };

        // This is partial specialization of SFINAE
        // when we have a operator to convert to Double const
        template <typename Type>
        struct is_has_operator_real
        <
            Type
            , typename extract_dummy
                <
                    double_type (Type::*)() const, &Type::operator double_type
                >::Dummy
        >
        {
            enum { value = 1, is_const = 1 };
            typedef std::integral_constant<bool, value > type;
        };

        template <int >
        struct extract_int_dummy
        {
            typedef detail::dummy type;
        };
    }

    // This approach to calculate is has type static const or enum ::value field
    template <typename Type, typename Dummy = cl::detail::dummy>
    struct has_value {
        enum { value = 0, solved_value = 0 };
        typedef std::false_type type;
    };

    template <typename Type>
    struct has_value<Type
            , typename cl::detail::extract_int_dummy<Type::value>::type >
    {
        enum { value = 1, solved_value = Type::value };
        typedef std::true_type type;
    };

    template <typename Type>
    struct remove_ad { typedef Type type; };

#if defined CL_TAPE_CPPAD
    template <typename Base>
    struct remove_ad<CppAD::AD<Base> >
    {
        enum { value = 1 };
        typedef Base type;
    };
#endif


    namespace detail
    {
        template <bool V, typename Then, typename Else>
        struct if_c;

        template <typename Then, typename Else>
        struct if_c<1, Then, Else> : std::true_type
        {
            typedef Then type;
        };

        template <typename Then, typename Else>
        struct if_c<0, Then, Else> : std::false_type
        {
            typedef Else type;
        };
    }
}

namespace cl
{
    template <typename Char>
    inline void throw_(Char what)
    {
        throw std::runtime_error(what);
    }

    template <typename _AD_type>
    inline void CheckParameter(_AD_type& x)
    {
        if (ext::Parameter(x))
            cl::throw_("Can't be parameter");
    }
}

#endif // __cl_tape_impl_doubleoperatorcheck_hpp__
