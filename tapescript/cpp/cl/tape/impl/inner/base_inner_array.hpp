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

#ifndef cl_tape_impl_inner_base_tape_inner_hpp
#define cl_tape_impl_inner_base_tape_inner_hpp

#include <limits>
#include <cl/tape/impl/inner/tape_inner.hpp>

namespace CppAD
{
    // Conditional equal expression.
    template <class Array>
    inline cl::tape_inner<Array> CondExpOpEq(
        const cl::tape_inner<Array>&       left,
        const cl::tape_inner<Array>&       right,
        const cl::tape_inner<Array>&       exp_if_true,
        const cl::tape_inner<Array>&       exp_if_false)
    {
        if (left.is_scalar() && right.is_scalar())
        {
            return (left.scalar_value_ == right.scalar_value_)
                ? exp_if_true : exp_if_false;
        }

        size_t size = left.is_array() ? left.size() : right.size();

        typename cl::tape_inner<Array>::array_type result(size);

        for (size_t i = 0; i < size; i++)
        {
            result[i] = left.element_at(i) == right.element_at(i)
                ? exp_if_true.element_at(i) : exp_if_false.element_at(i);
        }

        return result;
    }

    // Conditional less expression.
    template <class Array>
    inline cl::tape_inner<Array> CondExpOpLt(
        const cl::tape_inner<Array>&       left,
        const cl::tape_inner<Array>&       right,
        const cl::tape_inner<Array>&       exp_if_true,
        const cl::tape_inner<Array>&       exp_if_false)
    {
        if (left.is_scalar() && right.is_scalar())
        {
            return (left.scalar_value_ < right.scalar_value_)
                ? exp_if_true : exp_if_false;
        }
        
        size_t size = left.is_array() ? left.size() : right.size();

        typename cl::tape_inner<Array>::array_type result(size);
        
        for (size_t i = 0; i < size; i++)
        {
            result[i] = left.element_at(i) < right.element_at(i)
                ? exp_if_true.element_at(i) : exp_if_false.element_at(i);
        }

        return result;
    }
    
    // Conditional expression.
    template <class Array>
    inline cl::tape_inner<Array> CondExpOp(
        enum CompareOp                      cop          ,
        const cl::tape_inner<Array>&       left         ,
        const cl::tape_inner<Array>&       right        ,
        const cl::tape_inner<Array>&       exp_if_true  ,
        const cl::tape_inner<Array>&       exp_if_false )
    {
        switch (cop)
        {
        case CompareLt:
            return CondExpOpLt(left, right, exp_if_true, exp_if_false);

        case CompareLe:
            return CondExpOpLt(right, left, exp_if_false, exp_if_true);

        case CompareGe:
            return CondExpOpLt(left, right, exp_if_false, exp_if_true);

        case CompareGt:
            return CondExpOpLt(right, left, exp_if_true, exp_if_false);

        case CompareEq:
            return CondExpOpEq(left, right, exp_if_true, exp_if_false);

        default:
            cl::throw_("Unknown compare operation.");
            return exp_if_true;
        }
    }

	template <class Array> CPPAD_COND_EXP_BASE_REL(cl::tape_inner<Array>, Lt, CompareLt)
	template <class Array> CPPAD_COND_EXP_BASE_REL(cl::tape_inner<Array>, Le, CompareLe)
	template <class Array> CPPAD_COND_EXP_BASE_REL(cl::tape_inner<Array>, Eq, CompareEq)
	template <class Array> CPPAD_COND_EXP_BASE_REL(cl::tape_inner<Array>, Ge, CompareGe)
	template <class Array> CPPAD_COND_EXP_BASE_REL(cl::tape_inner<Array>, Gt, CompareGt)


    template <class Array>
    inline bool IdenticalPar(const cl::tape_inner<Array>& x)
    {
        return false;
    }

    template <class Array>
    inline bool IdenticalZero(const cl::tape_inner<Array>& x)
    {
        return x.is_scalar() && x == 0.0;
    }

    template <class Array>
    inline bool IdenticalOne(const cl::tape_inner<Array>& x)
    {
        return x.is_scalar() && x == 1.0;
    }

    template <class Array>
    inline bool IdenticalEqualPar(const cl::tape_inner<Array>& x, const cl::tape_inner<Array>& y)
    {
        if (x.is_scalar() && y.is_scalar())
        {
            return x == y;
        }
        if (x.is_array() && y.is_array())
        {
            return x.size() == y.size()
                && x == y;
        }
        return false;
    }

    template <class Array>
    inline int Integer(const cl::tape_inner<Array>& x)
    {
        return static_cast<int>(x.to_scalar());
    }

    template <class Array>
    inline bool GreaterThanZero(const cl::tape_inner<Array>& x)
    {
        return x > 0.;
    }

    template <class Array>
    inline bool GreaterThanOrZero(const cl::tape_inner<Array>& x)
    {
        return x >= 0.;
    }

    template <class Array>
    inline bool LessThanZero(const cl::tape_inner<Array>& x)
    {
        return x < 0.;
    }

    template <class Array>
    inline bool LessThanOrZero(const cl::tape_inner<Array>& x)
    {
        return x <= 0.;
    }

    template <class Array>
    inline bool abs_geq(const cl::tape_inner<Array>& x, const cl::tape_inner<Array>& y)
    {
        return std::abs(x) >= std::abs(y);
    }

#define CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(Fun) \
    template <class Array>                      \
    inline cl::tape_inner<Array> Fun(          \
        const cl::tape_inner<Array>& x)        \
    {	return cl::tapescript::Fun(x); }

    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(acos)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(asin)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(atan)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(cos)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(cosh)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(exp)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(abs)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(log)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(sin)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(sinh)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(sqrt)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(tan)
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(tanh)
#undef CL_ARRAY_CPPAD_STANDARD_MATH_UNARY

# if CPPAD_COMPILER_HAS_ERF
    CL_ARRAY_CPPAD_STANDARD_MATH_UNARY(erf)
# endif

    template <class Array>
    inline cl::tape_inner<Array> fabs(const cl::tape_inner<Array>& x)
    {
        return cl::tapescript::abs(x);
    }

    template <class Array>
    inline cl::tape_inner<Array> sign(const cl::tape_inner<Array>& x)
    {
        auto sign_func = [](double v)
        {
            if (v > 0.)
                return 1.;
            if (v == 0.)
                return 0.;
            return -1.;
        };

        return x.apply(sign_func);
    }

    template <class Array>
    inline cl::tape_inner<Array> pow(const cl::tape_inner<Array>& x, const cl::tape_inner<Array>& y)
    {
        return cl::tapescript::pow(x, y);
    }

    template <class Array>
    class numeric_limits<cl::tape_inner<Array>>
    {
    public:
        // machine epsilon
        static cl::tape_inner<Array> epsilon(void)
        {
            return std::numeric_limits<cl::tape_inner<Array>>::epsilon();
        }
        // minimum positive normalized value
        static cl::tape_inner<Array> min(void)
        {
            return std::numeric_limits<cl::tape_inner<Array>>::min();
        }
        // maximum finite value
        static cl::tape_inner<Array> max(void)
        {
            return std::numeric_limits<cl::tape_inner<Array>>::max();
        }
    };

    template <class Array>
    inline unsigned short hash_code(const cl::tape_inner<Array>& value)
    {
        if (value.is_scalar())
        {
            return hash_code(value.scalar_value_);
        }
        return hash_code(value.array_value_[0]);
    }
}

# endif // cl_tape_impl_inner_base_tape_inner_hpp
