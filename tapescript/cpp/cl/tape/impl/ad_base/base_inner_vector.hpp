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

#ifndef CL_BASE_INNER_VECTOR_INCLUDED
#define CL_BASE_INNER_VECTOR_INCLUDED

#include <limits>

#include <cppad/configure.hpp>
#include <cppad/local/ad.hpp>
#include <cppad/local/hash_code.hpp>
#include <cl/tape/impl/ad_base/inner_vector.hpp>

namespace CppAD
{
    inline cl::InnerVector CondExpOpEq(
        const cl::InnerVector&       left,
        const cl::InnerVector&       right,
        const cl::InnerVector&       exp_if_true,
        const cl::InnerVector&       exp_if_false)
    {
        if (left.is_scalar() && right.is_scalar())
        {
            return (left.scalar_value_ == right.scalar_value_)
                ? exp_if_true : exp_if_false;
        }

        std::valarray<bool> mask;
        size_t size;
        if (left.is_scalar() && right.is_vector())
        {
            mask = left.scalar_value_ == right.vector_value_;
            size = right.vector_value_.size();
        }
        else if (left.is_vector() && right.is_scalar())
        {
            mask = left.vector_value_ == right.scalar_value_;
            size = left.vector_value_.size();
        }
        else /* (left.is_vector() && right.is_vector()) */
        {
            mask = left.vector_value_ == right.vector_value_;
            size = left.vector_value_.size();
        }

        cl::InnerVector::vector_type result(size);
        if (exp_if_true.is_scalar())
        {
            result[mask] = exp_if_true.scalar_value_;
        }
        else
        {
            result[mask] = exp_if_true.vector_value_[mask];
        }
        if (exp_if_false.is_scalar())
        {
            result[!mask] = exp_if_false.scalar_value_;
        }
        else
        {
            result[!mask] = exp_if_false.vector_value_[!mask];
        }
        
        return result;
    }

    inline cl::InnerVector CondExpOpLt(
        const cl::InnerVector&       left,
        const cl::InnerVector&       right,
        const cl::InnerVector&       exp_if_true,
        const cl::InnerVector&       exp_if_false)
    {
        if (left.is_scalar() && right.is_scalar())
        {
            return (left.scalar_value_ < right.scalar_value_)
                ? exp_if_true : exp_if_false;
        }

        std::valarray<bool> mask;
        size_t size;
        if (left.is_scalar() && right.is_vector())
        {
            mask = left.scalar_value_ < right.vector_value_;
            size = right.vector_value_.size();
        }
        else if (left.is_vector() && right.is_scalar())
        {
            mask = left.vector_value_ < right.scalar_value_;
            size = left.vector_value_.size();
        }
        else /* (left.is_vector() && right.is_vector()) */
        {
            mask = left.vector_value_ < right.vector_value_;
            size = left.vector_value_.size();
        }

        cl::InnerVector::vector_type result(size);
        if (exp_if_true.is_scalar())
        {
            result[mask] = exp_if_true.scalar_value_;
        }
        else
        {
            result[mask] = exp_if_true.vector_value_[mask];
        }
        if (exp_if_false.is_scalar())
        {
            result[!mask] = exp_if_false.scalar_value_;
        }
        else
        {
            result[!mask] = exp_if_false.vector_value_[!mask];
        }

        return result;
    }
    
	inline cl::InnerVector CondExpOp( 
		enum CompareOp               cop          ,
		const cl::InnerVector&       left         ,
		const cl::InnerVector&       right        , 
		const cl::InnerVector&       exp_if_true  , 
		const cl::InnerVector&       exp_if_false )
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
            throw std::exception("Unknown compare operation.");
        }
	}

    // We are hacking into CppAD::AD friend function.
    template <>
    inline AD<cl::InnerVector> CondExpOp<cl::InnerVector>(
        enum CompareOp                   cop,
        const AD<cl::InnerVector>&       left,
        const AD<cl::InnerVector>&       right,
        const AD<cl::InnerVector>&       if_true,
        const AD<cl::InnerVector>&       if_false)
    {
        typedef cl::InnerVector Base;
        AD<Base> returnValue;
        CPPAD_ASSERT_UNKNOWN(Parameter(returnValue));
    
    
        // must use CondExp incase Base is an AD type and recording
        returnValue.value_ = CondExpOp(cop,
            left.value_, right.value_, if_true.value_, if_false.value_);
    
        // check first case where do not need to tape
        if (IdenticalPar(left) & IdenticalPar(right))
        {
            return returnValue;
        }
    
        ADTape<Base> *tape = CPPAD_NULL;
        if (Variable(left))
            tape = left.tape_this();
        if (Variable(right))
            tape = right.tape_this();
        if (Variable(if_true))
            tape = if_true.tape_this();
        if (Variable(if_false))
            tape = if_false.tape_this();
    
        // add this operation to the tape
        if (tape != CPPAD_NULL)
            tape->RecordCondExp(cop,
            returnValue, left, right, if_true, if_false);
    
        return returnValue;
    }

	CPPAD_COND_EXP_REL(cl::InnerVector)

//	inline bool EqualOpSeq(const cl::InnerVector& x, const cl::InnerVector& y)
//	{	return x == y; }

	inline bool IdenticalPar(const cl::InnerVector& x)
	{
	    return true;
	}
	inline bool IdenticalZero(const cl::InnerVector& x)
	{
	    return x.is_scalar() && x == 0.0;
	}
	inline bool IdenticalOne(const cl::InnerVector& x)
	{
	    return x.is_scalar() && x == 1.0;
	}
	inline bool IdenticalEqualPar(const cl::InnerVector& x, const cl::InnerVector& y)
	{
        if (x.is_scalar() && y.is_scalar())
        {
	        return x == y;
        }
        if (x.is_vector() && y.is_vector())
        {
            return x.vector_value_.size() == y.vector_value_.size()
                && x == y;
        }
        return false;
	}

	inline int Integer(const cl::InnerVector& x)
	{
	    return static_cast<int>(x.to_scalar());
	}

	inline bool GreaterThanZero(const cl::InnerVector& x)
	{	return x > 0.; }
	inline bool GreaterThanOrZero(const cl::InnerVector& x)
	{	return x >= 0.; }
	inline bool LessThanZero(const cl::InnerVector& x)
	{	return x < 0.; }
	inline bool LessThanOrZero(const cl::InnerVector& x)
	{	return x <= 0.; }
	inline bool abs_geq(const cl::InnerVector& x, const cl::InnerVector& y)
	{	return std::abs(x) >= std::abs(y); }

	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, acos)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, asin)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, atan)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, cos)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, cosh)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, exp)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, abs)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, log)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, log10)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, sin)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, sinh)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, sqrt)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, tan)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, tanh)
# if CPPAD_COMPILER_HAS_ERF
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, erf)
# endif

	inline cl::InnerVector fabs(const cl::InnerVector& x)
    {
        return std::abs(x);
    }
    
	inline cl::InnerVector sign(const cl::InnerVector& x)
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

	inline cl::InnerVector pow(const cl::InnerVector& x, const cl::InnerVector& y)
	{
	    return std::pow(x, y);
	}

	template <>
	class numeric_limits<cl::InnerVector>
    {
	public:
		// machine epsilon
		static cl::InnerVector epsilon(void)
		{
		    return std::numeric_limits<cl::InnerVector>::epsilon();
		}
		// minimum positive normalized value
		static cl::InnerVector min(void)
		{
		    return std::numeric_limits<cl::InnerVector>::min();
		}
		// maximum finite value
		static cl::InnerVector max(void)
		{
		    return std::numeric_limits<cl::InnerVector>::max();
		}
	};
	// deprecated machine epsilon
	template <> 
	inline cl::InnerVector epsilon<cl::InnerVector>(void)
	{
        return numeric_limits<cl::InnerVector>::epsilon();
    }


    template <>
    inline unsigned short hash_code<cl::InnerVector>(const cl::InnerVector& value)
    {
        if (value.is_scalar())
        {
            return hash_code(value.scalar_value_);
        }
        return hash_code(value.vector_value_[0]);
    }
}


# endif
