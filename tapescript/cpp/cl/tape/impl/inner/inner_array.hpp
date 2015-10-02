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

#pragma once

#ifndef CL_INNER_ARRAY_INCLUDED
#define CL_INNER_ARRAY_INCLUDED

#include <limits>
#include <vector>
#include <valarray>
#include <iterator>
#include <sstream>
#include <Eigen/Dense>

namespace cl
{
    // Class that used as Base for CppAD::AD<Base>.
    struct InnerArrayXd
    {
        typedef double scalar_type;
        typedef Eigen::ArrayXd vector_type;

        enum Mode
        {
            ScalarMode
            , VectorMode
        };

        // Default and double constructor.
        InnerArrayXd(double val = 0.)
            : mode_(ScalarMode)
            , scalar_value_(val)
            , vector_value_()
        {}

        InnerArrayXd(const InnerArrayXd&) = default;

        // Vector mode is used for vector value storage.
        InnerArrayXd(const vector_type& v)
            : mode_(VectorMode)
            , scalar_value_()
            , vector_value_(v)
        {}

        InnerArrayXd(vector_type&& v)
            : mode_(VectorMode)
            , scalar_value_()
            , vector_value_(std::move(v))
        {}

        // Construct as vector with equal coefficients.
        InnerArrayXd(double val, size_t n)
            : mode_(VectorMode)
            , scalar_value_()
            , vector_value_(vector_type::Constant(n, val))
        {}

        // Construct as vector with values passed by initializer_list.
        InnerArrayXd(std::initializer_list<double> il)
            : mode_(VectorMode)
            , scalar_value_()
            , vector_value_()
        {
            vector_value_ = Eigen::Map<const vector_type>(il.begin(), il.size());
        }

        // Returns true if scalar mode used.
        bool is_scalar() const
        {
            return mode_ == ScalarMode;
        }

        // Returns true if vector mode used.
        bool is_vector() const
        {
            return !is_scalar();
        }

        // Converts to scalar value.
        double to_scalar() const
        {
            if (is_vector())
            {
                cl::throw_("Not a scalar.");
            }
            return scalar_value_;
        }

        // Returns arithmetic negation.
        inline InnerArrayXd operator-() const
        {
            if (is_scalar())
            {
                return -scalar_value_;
            }
            return vector_type(-vector_value_);
        }

        // Returns a new InnerArrayXd of the same size with values which are acquired
        // by applying function func to the previous values of the elements.
        InnerArrayXd apply(double func(double)) const
        {
            if (is_scalar())
            {
                return func(scalar_value_);
            }
            vector_type result(vector_value_.size());
            for (int i = 0; i < result.size(); i++)
            {
                result(i) = func(vector_value_(i));
            }
            return result;
        }

#define CL_INNER_VECTOR_ASSIGN_OPERATOR(Op)                                 \
    inline InnerArrayXd& operator##Op##=(const InnerArrayXd& right)           \
        {                                                                   \
            if (is_scalar() && right.is_scalar())                           \
            {                                                               \
                scalar_value_ Op##= right.scalar_value_;                    \
            }                                                               \
            else if (is_vector() && right.is_scalar())                      \
            {                                                               \
                vector_value_ Op##= right.scalar_value_;                    \
            }                                                               \
            else if (is_scalar() && right.is_vector())                      \
            {                                                               \
                vector_value_ = scalar_value_ Op right.vector_value_;       \
                mode_ = VectorMode;                                         \
            }                                                               \
            else if (is_vector() && right.is_vector())                      \
            {                                                               \
                vector_value_ Op##= right.vector_value_;                    \
            }                                                               \
            return *this;                                                   \
        }

        // Assign operations.
        CL_INNER_VECTOR_ASSIGN_OPERATOR(+)
        CL_INNER_VECTOR_ASSIGN_OPERATOR(-)
        CL_INNER_VECTOR_ASSIGN_OPERATOR(*)
        CL_INNER_VECTOR_ASSIGN_OPERATOR(/)
#undef CL_INNER_VECTOR_ASSIGN_OPERATOR

        Mode mode_;
        double scalar_value_;
        vector_type vector_value_;
    };


    inline std::ostream& operator<<(std::ostream& os, const InnerArrayXd& x)
    {
        if (x.is_scalar())
        {
            return os << x.scalar_value_;
        }

        std::stringstream ss;
        ss.precision(os.precision());
        ss << "{ ";
        for (int i = 0; i < x.vector_value_.size(); i++)
        {
            ss << x.vector_value_(i) << ", ";
        }
        ss << "}";
        return os << ss.str();
    }

#define CL_BIN_INNER_VECTOR_OPERATOR(Res, Op)                               \
    inline Res operator##Op(const InnerArrayXd& x, const InnerArrayXd& y)     \
    {                                                                       \
        if (x.is_scalar() && y.is_scalar())                                 \
        {                                                                   \
            return x.scalar_value_ Op y.scalar_value_;                      \
        }                                                                   \
        else if (x.is_vector() && y.is_scalar())                            \
        {                                                                   \
            return x.vector_value_ Op y.scalar_value_;                      \
        }                                                                   \
        else if (x.is_scalar() && y.is_vector())                            \
        {                                                                   \
            return x.scalar_value_ Op y.vector_value_;                      \
        }                                                                   \
        else /* (x.is_vector() && y.is_vector()) */                         \
        {                                                                   \
            return x.vector_value_ Op y.vector_value_;                      \
        }                                                                   \
    }

    // Arithmetic binary operations.
    CL_BIN_INNER_VECTOR_OPERATOR(InnerArrayXd, - )
    CL_BIN_INNER_VECTOR_OPERATOR(InnerArrayXd, * )
    CL_BIN_INNER_VECTOR_OPERATOR(InnerArrayXd, / )
    CL_BIN_INNER_VECTOR_OPERATOR(InnerArrayXd, + )
#undef CL_BIN_INNER_VECTOR_OPERATOR

#define CL_BOOL_INNER_VECTOR_OPERATOR(Op)                                   \
    inline bool operator##Op(const InnerArrayXd& x, const InnerArrayXd& y)    \
    {                                                                       \
        if (x.is_scalar() && y.is_scalar())                                 \
        {                                                                   \
            return x.scalar_value_ Op y.scalar_value_;                      \
        }                                                                   \
        bool result = true;                                                 \
        if (x.is_vector() && y.is_scalar())                                 \
        {                                                                   \
            result = (x.vector_value_ Op y.scalar_value_).all();            \
        }                                                                   \
        else if (x.is_scalar() && y.is_vector())                            \
        {                                                                   \
            result = (x.scalar_value_ Op y.vector_value_).all();            \
        }                                                                   \
        else /* (left.is_vector() && right.is_vector()) */                  \
        {                                                                   \
            result = (x.vector_value_ Op y.vector_value_).all();            \
        }                                                                   \
        return result;                                                      \
    }

    // Logical binary operations.
    CL_BOOL_INNER_VECTOR_OPERATOR(!=)
    CL_BOOL_INNER_VECTOR_OPERATOR(==)
    CL_BOOL_INNER_VECTOR_OPERATOR(> )
    CL_BOOL_INNER_VECTOR_OPERATOR(>=)
    CL_BOOL_INNER_VECTOR_OPERATOR(< )
    CL_BOOL_INNER_VECTOR_OPERATOR(<=)
#undef CL_BOOL_INNER_VECTOR_OPERATOR
}

namespace std
{
#define CL_INNER_VECTOR_FUNCTION(Res, Name)                                 \
    inline Res Name(const cl::InnerArrayXd& x)                               \
    {                                                                       \
        if (x.is_scalar())                                                  \
        {                                                                   \
            return Name(x.scalar_value_);                                   \
        }                                                                   \
        return Name(x.vector_value_);                                       \
    }

    // Standart math functions.
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, abs)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, acos)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, sqrt)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, asin)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, atan)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, cos)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, sin)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, cosh)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, sinh)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, exp)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, log)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, log10)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, tan)
    CL_INNER_VECTOR_FUNCTION(cl::InnerArrayXd, tanh)
#undef CL_INNER_VECTOR_FUNCTION

    // Math power functioon.
    inline cl::InnerArrayXd pow(const cl::InnerArrayXd& left, const cl::InnerArrayXd& right)
    {
        if (left.is_scalar() && right.is_scalar())
        {
            return pow(left.scalar_value_, right.scalar_value_);
        }
        else if (left.is_vector() && right.is_scalar())
        {
            return pow(left.vector_value_, right.scalar_value_);
        }
        else if (left.is_scalar() && right.is_vector())
        {
            return pow(left.scalar_value_, right.vector_value_);
        }
        else // (left.is_vector() && right.is_vector())
        {
            return pow(left.vector_value_, right.vector_value_);
        }
    }


    // CLASS numeric_limits<cl::InnerArrayXd>
    template<>
    class numeric_limits<cl::InnerArrayXd>
    {
        typedef double base_type;
    public:
        typedef cl::InnerArrayXd _Ty;

        static _Ty min() _THROW0()
        {    // return minimum value
            return numeric_limits<base_type>::min();
        }

        static _Ty max() _THROW0()
        {    // return maximum value
            return numeric_limits<base_type>::max();
        }

        static _Ty epsilon() _THROW0()
        {    // return smallest effective increment from 1.0
            return numeric_limits<base_type>::epsilon();
        }
    };
}

#endif
