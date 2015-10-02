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

#ifndef CL_INNER_DOUBLE_INCLUDED
#define CL_INNER_DOUBLE_INCLUDED

#include <limits>

namespace cl
{
    struct InnerDouble
    {
        InnerDouble(double v = 0.)
            : value_(v)
        {}

#define CL_INNER_DOUBLE_ASSIGN_OPERATOR(Op)                                 \
        inline InnerDouble& operator##Op(const InnerDouble& y)              \
        {                                                                   \
            value_ Op y.value_;                                             \
            return *this;                                                   \
        }

        CL_INNER_DOUBLE_ASSIGN_OPERATOR(= )
        CL_INNER_DOUBLE_ASSIGN_OPERATOR(+= )
        CL_INNER_DOUBLE_ASSIGN_OPERATOR(-= )
        CL_INNER_DOUBLE_ASSIGN_OPERATOR(*= )
        CL_INNER_DOUBLE_ASSIGN_OPERATOR(/= )
#undef CL_INNER_DOUBLE_ASSIGN_OPERATOR

        inline InnerDouble operator-()
        {
            return -value_;
        }

        double value_;
    };


    inline std::ostream& operator<<(std::ostream& o, const InnerDouble& x)
    {
        return o << x.value_;
    }

#define CL_BIN_INNER_DOUBLE_OPERATOR(Res, Op)                               \
    inline Res operator##Op(const InnerDouble& x, const InnerDouble& y)     \
    {                                                                       \
        return x.value_ Op y.value_;                                        \
    }

    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , != )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , == )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , >  )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , >= )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , <  )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , <= )
    CL_BIN_INNER_DOUBLE_OPERATOR(InnerDouble, -  )
    CL_BIN_INNER_DOUBLE_OPERATOR(InnerDouble, *  )
    CL_BIN_INNER_DOUBLE_OPERATOR(InnerDouble, /  )
    CL_BIN_INNER_DOUBLE_OPERATOR(InnerDouble, +)
#undef CL_BIN_INNER_DOUBLE_OPERATOR
}

namespace std
{
#define CL_INNER_DOUBLE_FUNCTION(Res, Name)                                 \
    inline Res Name(const cl::InnerDouble& x)                               \
    {                                                                       \
        return Name(x.value_);                                              \
    }

    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, abs)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, fabs)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, acos)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, sqrt)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, asin)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, atan)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, cos)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, sin)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, cosh)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, sinh)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, exp)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, log)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, log10)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, tan)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, tanh)
#undef CL_INNER_DOUBLE_FUNCTION

    inline cl::InnerDouble pow(const cl::InnerDouble& x, const cl::InnerDouble& y)
    {
        return std::pow(x.value_, y.value_);
    }

    // CLASS numeric_limits<cl::InnerDouble>
    template<>
    class numeric_limits<cl::InnerDouble>
    {
        typedef double base_type;
    public:
        typedef cl::InnerDouble _Ty;

        static _Ty min()
        {	// return minimum value
            return numeric_limits<base_type>::min();
        }

        static _Ty max()
        {	// return maximum value
            return numeric_limits<base_type>::max();
        }

        static _Ty epsilon()
        {	// return smallest effective increment from 1.0
            return numeric_limits<base_type>::epsilon();
        }
    };
}


#endif