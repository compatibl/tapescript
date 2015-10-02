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

#ifndef CL_INNER_VECTOR_INCLUDED
#define CL_INNER_VECTOR_INCLUDED

#include <limits>
#include <vector>
#include <valarray>
#include <iterator>
#include <sstream>

namespace cl
{
    struct InnerVector
    {
        typedef std::valarray<double> vector_type;

        enum Mode
        {
            ScalarMode
            , VectorMode
        };

        InnerVector(double val = 0.)
            : mode_(ScalarMode)
            , scalar_value_(val)
            , vector_value_()
        {}

        InnerVector(const InnerVector&) = default;
        
        InnerVector(const vector_type& v)
            : mode_(VectorMode)
            , scalar_value_()
            , vector_value_(v)
        {}

        InnerVector(vector_type&& v)
            : mode_(VectorMode)
            , scalar_value_()
            , vector_value_(std::move(v))
        {}

        InnerVector(double val, size_t n)
            : mode_(VectorMode)
            , scalar_value_()
            , vector_value_(val, n)
        {}

        InnerVector(std::initializer_list<double> il)
            : mode_(VectorMode)
            , scalar_value_()
            , vector_value_(il)
        {}

        bool is_scalar() const
        {
            return mode_ == ScalarMode;
        }

        bool is_vector() const
        {
            return !is_scalar();
        }

        double to_scalar() const
        {
            if (is_scalar())
            {
                return scalar_value_;
            }

            throw std::exception("Not a scalar.");
        }

        inline InnerVector operator-() const
        {
            if (is_scalar())
            {
                return -scalar_value_;
            }
            return -vector_value_;
        }

        // Returns a new InnerVector of the same size with values which are acquired
        // by applying function func to the previous values of the elements.
        InnerVector apply(double func(double)) const
        {
            if (is_scalar())
            {
                return func(scalar_value_);
            }
            return vector_value_.apply(func);
        }

#define CL_INNER_VECTOR_ASSIGN_OPERATOR(Op)                                 \
    inline InnerVector& operator##Op##=(const InnerVector& right)           \
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

        CL_INNER_VECTOR_ASSIGN_OPERATOR(+)
        CL_INNER_VECTOR_ASSIGN_OPERATOR(-)
        CL_INNER_VECTOR_ASSIGN_OPERATOR(*)
        CL_INNER_VECTOR_ASSIGN_OPERATOR(/)
#undef CL_INNER_VECTOR_ASSIGN_OPERATOR

        Mode mode_;
        double scalar_value_;
        vector_type vector_value_;
    };


    inline std::ostream& operator<<(std::ostream& os, const InnerVector& x)
    {
        if (x.is_scalar())
        {
            return os << x.scalar_value_;
        }

        std::stringstream ss;
        ss.precision(os.precision());
        ss << "{ ";
        std::ostream_iterator<double> out(ss, ", ");
        std::copy(std::begin(x.vector_value_), std::end(x.vector_value_), out);
        ss << "}";
        return os << ss.str();
    }

#define CL_BIN_INNER_VECTOR_OPERATOR(Res, Op)                               \
    inline Res operator##Op(const InnerVector& x, const InnerVector& y)     \
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
        else /* (left.is_vector() && right.is_vector()) */                  \
        {                                                                   \
            return x.vector_value_ Op y.vector_value_;                      \
        }                                                                   \
    }

    CL_BIN_INNER_VECTOR_OPERATOR(InnerVector, - )
    CL_BIN_INNER_VECTOR_OPERATOR(InnerVector, * )
    CL_BIN_INNER_VECTOR_OPERATOR(InnerVector, / )
    CL_BIN_INNER_VECTOR_OPERATOR(InnerVector, + )
#undef CL_BIN_INNER_VECTOR_OPERATOR
        
#define CL_BOOL_INNER_VECTOR_OPERATOR(Op)                                   \
    inline bool operator##Op(const InnerVector& x, const InnerVector& y)    \
    {                                                                       \
        if (x.is_scalar() && y.is_scalar())                                 \
        {                                                                   \
            return x.scalar_value_ Op y.scalar_value_;                      \
        }                                                                   \
        std::valarray<bool> b;                                              \
        if (x.is_vector() && y.is_scalar())                                 \
        {                                                                   \
            b = x.vector_value_ Op y.scalar_value_;                         \
        }                                                                   \
        else if (x.is_scalar() && y.is_vector())                            \
        {                                                                   \
            b = x.scalar_value_ Op y.vector_value_;                         \
        }                                                                   \
        else /* (left.is_vector() && right.is_vector()) */                  \
        {                                                                   \
            b = x.vector_value_ Op y.vector_value_;                         \
        }                                                                   \
        bool result = true;                                                 \
        for (bool item : b)                                                 \
        {                                                                   \
            result &= item;                                                 \
        }                                                                   \
        return result;                                                      \
    }

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
    inline Res Name(const cl::InnerVector& x)                               \
    {                                                                       \
        if (x.is_scalar())                                                  \
        {                                                                   \
            return Name(x.scalar_value_);                                   \
        }                                                                   \
        return Name(x.vector_value_);                                       \
    }

    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, abs)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, fabs)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, acos)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, sqrt)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, asin)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, atan)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, cos)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, sin)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, cosh)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, sinh)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, exp)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, log)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, log10)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, tan)
    CL_INNER_VECTOR_FUNCTION(cl::InnerVector, tanh)
#undef CL_INNER_VECTOR_FUNCTION

    inline cl::InnerVector pow(const cl::InnerVector& left, const cl::InnerVector& right)
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


    // CLASS numeric_limits<cl::InnerVector>
    template<> 
    class numeric_limits<cl::InnerVector>
    {
        typedef double base_type;
    public:
        typedef cl::InnerVector _Ty;

        static _Ty min() _THROW0()
        {	// return minimum value
            return numeric_limits<base_type>::min();
        }

        static _Ty max() _THROW0()
        {	// return maximum value
            return numeric_limits<base_type>::max();
        }

        static _Ty epsilon() _THROW0()
        {	// return smallest effective increment from 1.0
            return numeric_limits<base_type>::epsilon();
        }
    };
}

#endif
