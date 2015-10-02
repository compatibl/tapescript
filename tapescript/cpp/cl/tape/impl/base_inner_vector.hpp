#pragma once

# ifndef CL_BASE_INNER_VECTOR_INCLUDED
# define CL_BASE_INNER_VECTOR_INCLUDED

# include <cppad/configure.hpp>
# include <limits>

namespace CppAD {
	inline cl::InnerVector CondExpOp( 
		enum CompareOp               cop          ,
		const cl::InnerVector&       left         ,
		const cl::InnerVector&       right        , 
		const cl::InnerVector&       exp_if_true  , 
		const cl::InnerVector&       exp_if_false )
	{	return CondExpTemplate(cop, left, right, exp_if_true, exp_if_false);
	}
}

namespace CppAD {
	CPPAD_COND_EXP_REL(cl::InnerVector)
}

namespace CppAD {
	inline bool EqualOpSeq(const cl::InnerVector& x, const cl::InnerVector& y)
	{	return x == y; }
}

namespace CppAD {
	inline bool IdenticalPar(const cl::InnerVector& x)
	{	return true; }
	inline bool IdenticalZero(const cl::InnerVector& x)
	{	return (x == 0.); }
	inline bool IdenticalOne(const cl::InnerVector& x)
	{	return (x == 1.); }
	inline bool IdenticalEqualPar(const cl::InnerVector& x, const cl::InnerVector& y)
	{	return (x == y); }
}

namespace CppAD {
	inline int Integer(const cl::InnerVector& x)
	{	return static_cast<int>(x.to_double()); }
}

namespace CppAD {
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
}

namespace CppAD {
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, acos)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, asin)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, atan)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, cos)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, cosh)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, exp)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerVector, fabs)
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
}

namespace CppAD {
	inline cl::InnerVector abs(const cl::InnerVector& x)
	{	return std::fabs(x); }
}

namespace CppAD {
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
}

namespace CppAD {
	inline cl::InnerVector pow(const cl::InnerVector& x, const cl::InnerVector& y)
	{ return std::pow(x, y); }
}

namespace CppAD {
	template <>
	class numeric_limits<cl::InnerVector> {
	public:
		// machine epsilon
		static cl::InnerVector epsilon(void)
		{	return std::numeric_limits<cl::InnerVector>::epsilon(); }
		// minimum positive normalized value
		static cl::InnerVector min(void)
		{	return std::numeric_limits<cl::InnerVector>::min(); }
		// maximum finite value
		static cl::InnerVector max(void)
		{	return std::numeric_limits<cl::InnerVector>::max(); }
	};
	// deprecated machine epsilon
	template <> 
	inline cl::InnerVector epsilon<cl::InnerVector>(void)
	{	return numeric_limits<cl::InnerVector>::epsilon(); }
}


# endif
