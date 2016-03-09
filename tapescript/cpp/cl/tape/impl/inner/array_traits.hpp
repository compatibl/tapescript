/*
Copyright (C) 2015-present CompatibL

Performance test results and finance-specific examples are available at:

http://www.modval.org/adjoint

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

#ifndef cl_tape_impl_tape_inner_traits_hpp
#define cl_tape_impl_tape_inner_traits_hpp

#include <limits>
#include <valarray>
#include <sstream>

#if defined CL_EIGEN_ENABLED
#   include <Eigen/Dense>
#endif

#include <cl/tape/impl/tape_fwd.hpp>

namespace cl
{

#if defined CL_EIGEN_ENABLED
    typedef tape_inner<Eigen::ArrayXd> tape_valueXd;
#endif

    /// <summary>Traits of array type for using it as tape_inner template parameter.</summary>
    template <class Array>
    struct array_traits;

#define CL_INNER_ARRAY_FUNCTION_TRAITS(Qualifier, Name)     \
    static inline array_type Name(const array_type& x)      \
    {                                                       \
    return Qualifier Name(x);                           \
    }

#define CL_INNER_ARRAY_FUNCTION_NOT_DEF(Array, Name)                        \
    static inline array_type Name(const array_type& x)                      \
    {                                                                       \
    cl::throw_("The function " #Name " is not implemented for " Array); \
    return x;                                                           \
    }

    /// <summary>Array traits of std::valaray.</summary>
    template <class Scalar>
    struct array_traits<std::valarray<Scalar>>
    {
        typedef Scalar scalar_type;
        typedef std::valarray<scalar_type> array_type;
        typedef size_t size_type;

        static inline array_type get_const(size_t count, scalar_type const& val)
        {
            return std::valarray<scalar_type>(val, count);
        }

        static inline array_type get_from_init_list(std::initializer_list<scalar_type> il)
        {
            return std::valarray<scalar_type>(il);
        }

        template <class Ty1, class Ty2>
        static inline bool operator_Ne(Ty1&& x, Ty2&& y)
        {
            return all_true(std::forward<Ty1>(x) != std::forward<Ty2>(y));
        }

        template <class Ty1, class Ty2>
        static inline bool operator_Eq(Ty1&& x, Ty2&& y)
        {
            return all_true(std::forward<Ty1>(x) == std::forward<Ty2>(y));
        }

        template <class Ty1, class Ty2>
        static inline bool operator_Lt(Ty1&& x, Ty2&& y)
        {
            return all_true(std::forward<Ty1>(x) < std::forward<Ty2>(y));
        }

        template <class Ty1, class Ty2>
        static inline bool operator_Le(Ty1&& x, Ty2&& y)
        {
            return all_true(std::forward<Ty1>(x) <= std::forward<Ty2>(y));
        }

        CL_INNER_ARRAY_FUNCTION_TRAITS(std::, abs)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, acos)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, sqrt)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, asin)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, atan)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, cos)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, sin)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, cosh)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, sinh)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, exp)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, log)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, tan)
            CL_INNER_ARRAY_FUNCTION_TRAITS(std::, tanh)

            template <class Ty1, class Ty2>
        static inline array_type pow(const Ty1& x, const Ty2& y)
        {
            return std::pow(x, y);
        }

    private:
        static inline bool all_true(std::valarray<bool> const& val)
        {
            bool result = true;
            for (bool item : val)
            {
                result &= item;
            }
            return result;
        }
    };

#if defined CL_EIGEN_ENABLED
    /// <summary>Array traits of Eigen::Array.
    /// Note, the CppAD does not support Eigen aligment requirements for fixed-size types.</summary>
    template <class Scalar, int Rows, int Cols, int Options, int MaxRows, int MaxCols>
    struct array_traits<
        Eigen::Array<Scalar, Rows, Cols, Options, MaxRows, MaxCols>
    >
    {
        typedef Scalar scalar_type;
        typedef Eigen::Array<Scalar, Rows, Cols, Options, MaxRows, MaxCols> array_type;
        typedef typename array_type::Index size_type;

        static inline array_type get_const(size_t count, scalar_type const& val)
        {
            return array_type::Constant(count, val);
        }

        static inline array_type get_from_init_list(std::initializer_list<scalar_type> il)
        {
            return array_type(Eigen::Map<const array_type>(il.begin(), il.size()));
        }

        template <class Ty1, class Ty2>
        static inline bool operator_Ne(Ty1&& x, Ty2&& y)
        {
            return (std::forward<Ty1>(x) != std::forward<Ty2>(y)).all();
        }

        template <class Ty1, class Ty2>
        static inline bool operator_Eq(Ty1&& x, Ty2&& y)
        {
            return (std::forward<Ty1>(x) == std::forward<Ty2>(y)).all();
        }

        template <class Ty1, class Ty2>
        static inline bool operator_Lt(Ty1&& x, Ty2&& y)
        {
            return (std::forward<Ty1>(x) < std::forward<Ty2>(y)).all();
        }

        template <class Ty1, class Ty2>
        static inline bool operator_Le(Ty1&& x, Ty2&& y)
        {
            return (std::forward<Ty1>(x) <= std::forward<Ty2>(y)).all();
        }

        CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, abs)
            CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, acos)
            CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, sqrt)
            CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, asin)
            CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, cos)
            CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, sin)
            CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, exp)
            CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, log)
            CL_INNER_ARRAY_FUNCTION_TRAITS(Eigen::, tan)

            CL_INNER_ARRAY_FUNCTION_NOT_DEF("Eigen::Array", atan)
            CL_INNER_ARRAY_FUNCTION_NOT_DEF("Eigen::Array", cosh)
            CL_INNER_ARRAY_FUNCTION_NOT_DEF("Eigen::Array", sinh)
            CL_INNER_ARRAY_FUNCTION_NOT_DEF("Eigen::Array", tanh)

            template <class Ty>
        static inline array_type pow(const array_type& x, const Ty& y)
        {
            return Eigen::pow(x, y);
        }

        static inline array_type pow(const scalar_type& x, const array_type& y)
        {
            return Eigen::exp(std::log(x) * y);
        }
    };
#endif // CL_EIGEN_ENABLED
}

#endif // cl_tape_impl_tape_inner_traits_hpp
