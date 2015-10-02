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

#ifndef CL_INNER_ARRAY_HPP_INCLUDED
#define CL_INNER_ARRAY_HPP_INCLUDED

#pragma once

#include <limits>
#include <valarray>
#include <sstream>

#if defined CL_EIGEN_ENABLED
#   include <Eigen/Dense>
#endif

#include <cl/tape/impl/tape_fwd.hpp>

namespace cl
{
    template <class Array> struct inner_array;
    typedef inner_array<std::valarray<double>> InnerValArray;
    typedef tape_double<InnerValArray> TapeValArray;
    typedef InnerValArray InnerArray;
    typedef tape_double<InnerArray> TapeArray;

#if defined CL_EIGEN_ENABLED
    typedef inner_array<Eigen::ArrayXd> InnerArrayXd;
#endif


    /// <summary>Traits of array type for using it as inner_array template parameter.</summary>
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
#endif


    /// <summary>Class that aggregates a scalar value and an array value.
    /// Used as Base tamplate parameter for CppAD::AD class.</summary>
    template <class Array>
    struct inner_array
    {
        typedef array_traits<Array> traits;
        typedef typename traits::scalar_type scalar_type;
        typedef typename traits::array_type array_type;
        typedef typename traits::size_type size_type;

        enum Mode
        {
            ScalarMode
            , ArrayMode
        };

        // Default and scalar_type constructor.
        inner_array(const scalar_type& val = scalar_type())
            : mode_(ScalarMode)
            , scalar_value_(val)
            , array_value_()
        {}

        inner_array(const inner_array&) = default;

        // Array mode is used for array value storage.
        inner_array(const array_type& v)
            : mode_(ArrayMode)
            , scalar_value_()
            , array_value_(v)
        {}

        inner_array(array_type&& v)
            : mode_(ArrayMode)
            , scalar_value_()
            , array_value_(std::move(v))
        {}

        // Construct as array with equal coefficients.
        inner_array(const scalar_type& val, size_t count)
            : mode_(ArrayMode)
            , scalar_value_()
            , array_value_(traits::get_const(count, val))
        {}

        // Construct as array with values passed by initializer_list.
        inner_array(std::initializer_list<scalar_type> il)
            : mode_(ArrayMode)
            , scalar_value_()
            , array_value_(traits::get_from_init_list(il))
        {}

        // Returns true if scalar mode used.
        bool is_scalar() const
        {
            return mode_ == ScalarMode;
        }

        // Returns true if array mode used.
        bool is_array() const
        {
            return !is_scalar();
        }

        // Converts to scalar value.
        scalar_type to_scalar() const
        {
            if (is_array())
            {
                cl::throw_("Not a scalar.");
            }
            return scalar_value_;
        }

        // Returns arithmetic negation.
        inline inner_array operator-() const
        {
            if (is_scalar())
            {
                return -scalar_value_;
            }
            return inner_array(-array_value_);
        }

        // Returns a new inner_array of the same size with values which are acquired
        // by applying function func to the previous values of the elements.
        inner_array apply(scalar_type func(scalar_type)) const
        {
            if (is_scalar())
            {
                return func(scalar_value_);
            }
            array_type result(array_value_.size());
            for (size_type i = 0; i < result.size(); i++)
            {
                result[i] = func(array_value_[i]);
            }
            return result;
        }

        // Assign operations.
#define CL_INNER_ARRAY_ASSIGN_OPERATOR(Op)                              \
        inline inner_array& operator Op ## = (const inner_array& right) \
        {                                                               \
            if (is_scalar() && right.is_scalar())                       \
            {                                                           \
                scalar_value_ Op##= right.scalar_value_;                \
            }                                                           \
            else if (is_array() && right.is_scalar())                  \
            {                                                           \
                array_value_ Op##= right.scalar_value_;                 \
            }                                                           \
            else if (is_scalar() && right.is_array())                  \
            {                                                           \
                array_value_ = scalar_value_ Op right.array_value_;     \
                mode_ = ArrayMode;                                      \
            }                                                           \
            else if (is_array() && right.is_array())                  \
            {                                                           \
                array_value_ Op##= right.array_value_;                  \
            }                                                           \
            return *this;                                               \
        }
        CL_INNER_ARRAY_ASSIGN_OPERATOR(+)
        CL_INNER_ARRAY_ASSIGN_OPERATOR(-)
        CL_INNER_ARRAY_ASSIGN_OPERATOR(*)
        CL_INNER_ARRAY_ASSIGN_OPERATOR(/)
#undef CL_INNER_ARRAY_ASSIGN_OPERATOR

        // Gives the element of an array by the index.
        // If the object is array valued returns an element of the array value.
        // Othervise returns scalar value.
        // Use this method for element-wise operations.
        const scalar_type& element_at(size_t index) const
        {
            if (is_scalar())
            {
                return scalar_value_;
            }
            return array_value_[(size_type)index];
        }

        // Returns size of array value.
        size_t size() const
        {
            assert(is_array());
            size_type temp = array_value_.size();
            assert(temp >= 0);
            return (size_t)temp;
        }

        Mode mode_;
        scalar_type scalar_value_;
        array_type array_value_;
    };


    // Stream insertion operator.
    template <class Array>
    inline std::ostream& operator<<(std::ostream& os, const inner_array<Array>& x)
    {
        if (x.is_scalar())
        {
            return os << x.scalar_value_;
        }
        if (x.size() == 0)
        {
            return os << "{}";
        }

        std::stringstream ss;
        ss.precision(os.precision());
        ss << "{ " << x.array_value_[0];
        for (size_t i = 1; i < x.size(); ++i)
        {
            ss << ", " << x.array_value_[i];
        }
        ss << " }";

        return os << ss.str();
    }


    // Arithmetic binary operations.
#define CL_BIN_INNER_ARRAY_OPERATOR(Op)                                                         \
    template <class Array>                                                                      \
    inline inner_array<Array> operator Op(                                                      \
        const inner_array<Array>& x                                                             \
        , const inner_array<Array>& y)                                                          \
    {                                                                                           \
        if (x.is_scalar() && y.is_scalar())                                                     \
        {                                                                                       \
            return x.scalar_value_ Op y.scalar_value_;                                          \
        }                                                                                       \
        else if (x.is_array() && y.is_scalar())                                                 \
        {                                                                                       \
            return x.array_value_ Op y.scalar_value_;                                           \
        }                                                                                       \
        else if (x.is_scalar() && y.is_array())                                                 \
        {                                                                                       \
            return x.scalar_value_ Op y.array_value_;                                           \
        }                                                                                       \
        else /* (x.is_array() && y.is_array()) */                                               \
        {                                                                                       \
            return x.array_value_ Op y.array_value_;                                            \
        }                                                                                       \
    }                                                                                           \
                                                                                                \
    template <class Array>                                                                      \
    inline inner_array<Array> operator Op(                                                      \
        const inner_array<Array>& x                                                             \
        , const typename inner_array<Array>::scalar_type& y)                                    \
    {                                                                                           \
        if (x.is_scalar())                                                                      \
        {                                                                                       \
            return x.scalar_value_ Op y;                                                        \
        }                                                                                       \
        return x.array_value_ Op y;                                                             \
    }                                                                                           \
                                                                                                \
    template <class Array>                                                                      \
    inline inner_array<Array> operator Op(                                                      \
        const typename inner_array<Array>::scalar_type& x                                       \
        , const inner_array<Array>& y)                                                          \
    {                                                                                           \
        if (y.is_scalar())                                                                      \
        {                                                                                       \
            return x Op y.scalar_value_;                                                        \
        }                                                                                       \
        return x Op y.array_value_;                                                             \
    }

    CL_BIN_INNER_ARRAY_OPERATOR(-)
    CL_BIN_INNER_ARRAY_OPERATOR(*)
    CL_BIN_INNER_ARRAY_OPERATOR(/)
    CL_BIN_INNER_ARRAY_OPERATOR(+)
#undef CL_BIN_INNER_ARRAY_OPERATOR


    // Logical binary operations.
#define CL_BOOL_INNER_ARRAY_OPERATOR(Op, Code)                                                  \
    template <class Array>                                                                      \
    inline bool operator Op(const inner_array<Array>& x, const inner_array<Array>& y)           \
    {                                                                                           \
        if (x.is_scalar() && y.is_scalar())                                                     \
        {                                                                                       \
            return x.scalar_value_ Op y.scalar_value_;                                          \
        }                                                                                       \
        bool result = true;                                                                     \
        if (x.is_array() && y.is_scalar())                                                      \
        {                                                                                       \
            result = array_traits<Array>::operator_##Code(x.array_value_, y.scalar_value_);     \
        }                                                                                       \
        else if (x.is_scalar() && y.is_array())                                                 \
        {                                                                                       \
            result = array_traits<Array>::operator_##Code(x.scalar_value_, y.array_value_);     \
        }                                                                                       \
        else /* (left.is_array() && right.is_array()) */                                        \
        {                                                                                       \
            result = array_traits<Array>::operator_##Code(x.array_value_, y.array_value_);      \
        }                                                                                       \
        return result;                                                                          \
    }                                                                                           \
                                                                                                \
    template <class Array>                                                                      \
    inline bool operator Op(                                                                    \
        const inner_array<Array>& x                                                             \
        , const typename inner_array<Array>::scalar_type& y)                                    \
    {                                                                                           \
        if (x.is_scalar())                                                                      \
        {                                                                                       \
            return x.scalar_value_ Op y;                                                        \
        }                                                                                       \
        return array_traits<Array>::operator_##Code(x.array_value_, y);                         \
    }                                                                                           \
                                                                                                \
    template <class Array>                                                                      \
    inline bool operator Op(                                                                    \
        const typename inner_array<Array>::scalar_type& x                                       \
        , const inner_array<Array>& y)                                                          \
    {                                                                                           \
        if (y.is_scalar())                                                                      \
        {                                                                                       \
            return x Op y.scalar_value_;                                                        \
        }                                                                                       \
        return array_traits<Array>::operator_##Code(x, y.array_value_);                         \
    }

    CL_BOOL_INNER_ARRAY_OPERATOR(!=, Ne)
    CL_BOOL_INNER_ARRAY_OPERATOR(==, Eq)
    CL_BOOL_INNER_ARRAY_OPERATOR(< , Lt)
    CL_BOOL_INNER_ARRAY_OPERATOR(<=, Le)

    template <class Array>
    inline bool operator>(const inner_array<Array>& x, const inner_array<Array>& y)
    {
        return y < x;
    }
    template <class Array>
    inline bool operator>(const inner_array<Array>& x, const typename inner_array<Array>::scalar_type& y)
    {
        return y < x;
    }
    template <class Array>
    inline bool operator>(const typename inner_array<Array>::scalar_type& x, const inner_array<Array>& y)
    {
        return y < x;
    }
    template <class Array>
    inline bool operator>=(const inner_array<Array>& x, const inner_array<Array>& y)
    {
        return y <= x;
    }
    template <class Array>
    inline bool operator>=(const inner_array<Array>& x, const typename inner_array<Array>::scalar_type& y)
    {
        return y <= x;
    }
    template <class Array>
    inline bool operator>=(const typename inner_array<Array>::scalar_type& x, const inner_array<Array>& y)
    {
        return y <= x;
    }
#undef CL_BOOL_INNER_ARRAY_OPERATOR


    namespace tapescript
    {
        // Standart math functions.
#define CL_INNER_ARRAY_FUNCTION(Name)                                                       \
        template <class Array>                                                              \
        inline cl::inner_array<Array> Name(const cl::inner_array<Array>& x)                 \
        {                                                                                   \
            if (x.is_scalar())                                                              \
            {                                                                               \
                return std::Name(x.scalar_value_);                                          \
            }                                                                               \
            return cl::inner_array<Array>::traits::Name(x.array_value_);                    \
        }
        CL_INNER_ARRAY_FUNCTION(abs)
        CL_INNER_ARRAY_FUNCTION(acos)
        CL_INNER_ARRAY_FUNCTION(sqrt)
        CL_INNER_ARRAY_FUNCTION(asin)
        CL_INNER_ARRAY_FUNCTION(atan)
        CL_INNER_ARRAY_FUNCTION(cos)
        CL_INNER_ARRAY_FUNCTION(sin)
        CL_INNER_ARRAY_FUNCTION(cosh)
        CL_INNER_ARRAY_FUNCTION(sinh)
        CL_INNER_ARRAY_FUNCTION(exp)
        CL_INNER_ARRAY_FUNCTION(log)
        CL_INNER_ARRAY_FUNCTION(tan)
        CL_INNER_ARRAY_FUNCTION(tanh)
#undef CL_INNER_ARRAY_FUNCTION

        // Math power functioon.
        template <class Array>
        inline cl::inner_array<Array> pow(
            const cl::inner_array<Array>& left
            , const cl::inner_array<Array>& right)
        {
            typedef typename cl::inner_array<Array>::traits traits;
            if (left.is_scalar() && right.is_scalar())
            {
                return std::pow(left.scalar_value_, right.scalar_value_);
            }
            else if (left.is_array() && right.is_scalar())
            {
                return traits::pow(left.array_value_, right.scalar_value_);
            }
            else if (left.is_scalar() && right.is_array())
            {
                return traits::pow(left.scalar_value_, right.array_value_);
            }
            else // (left.is_array() && right.is_array())
            {
                return traits::pow(left.array_value_, right.array_value_);
            }
        }

        // Math power functioon.
        template <class Array>
        inline cl::inner_array<Array> pow(
            const cl::inner_array<Array>& left
            , const typename cl::inner_array<Array>::scalar_type& right)
        {
            typedef typename cl::inner_array<Array>::traits traits;
            if (left.is_scalar())
            {
                return std::pow(left.scalar_value_, right);
            }
            return traits::pow(left.array_value_, right);
        }

        // Math power functioon.
        template <class Array>
        inline cl::inner_array<Array> pow(
            const typename cl::inner_array<Array>::scalar_type& left
            , const cl::inner_array<Array>& right)
        {
            typedef typename cl::inner_array<Array>::traits traits;
            if (right.is_scalar())
            {
                return std::pow(left, right.scalar_value_);
            }
            return traits::pow(left, right.array_value_);
        }

    } // namespace tapescript
} // namespace cl


namespace std
{
    // CLASS numeric_limits<cl::inner_array<Array>>
    template<class Array>
    class numeric_limits<cl::inner_array<Array>>
    {
        typedef typename cl::inner_array<Array>::scalar_type scalar_type;
    public:
        typedef cl::inner_array<Array> _Ty;

        static _Ty min()
        {    // return minimum value
            return numeric_limits<scalar_type>::min();
        }

        static _Ty max()
        {    // return maximum value
            return numeric_limits<scalar_type>::max();
        }

        static _Ty epsilon()
        {    // return smallest effective increment from 1.0
            return numeric_limits<scalar_type>::epsilon();
        }
    };
}

#endif
