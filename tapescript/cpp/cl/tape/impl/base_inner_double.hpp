#pragma once

# ifndef CL_BASE_INNER_DOUBLE_INCLUDED
# define CL_BASE_INNER_DOUBLE_INCLUDED

# include <cppad/configure.hpp>
# include <limits>

namespace CppAD {
	inline cl::InnerDouble CondExpOp( 
		enum CompareOp               cop          ,
		const cl::InnerDouble&       left         ,
		const cl::InnerDouble&       right        , 
		const cl::InnerDouble&       exp_if_true  , 
		const cl::InnerDouble&       exp_if_false )
	{	return CondExpTemplate(cop, left, right, exp_if_true, exp_if_false);
	}
}

namespace CppAD {
	CPPAD_COND_EXP_REL(cl::InnerDouble)
}

namespace CppAD {
	inline bool EqualOpSeq(const cl::InnerDouble& x, const cl::InnerDouble& y)
	{	return x == y; }
}

namespace CppAD {
	inline bool IdenticalPar(const cl::InnerDouble& x)
	{	return true; }
	inline bool IdenticalZero(const cl::InnerDouble& x)
	{	return (x == 0.); }
	inline bool IdenticalOne(const cl::InnerDouble& x)
	{	return (x == 1.); }
	inline bool IdenticalEqualPar(const cl::InnerDouble& x, const cl::InnerDouble& y)
	{	return (x == y); }
}

namespace CppAD {
	inline int Integer(const cl::InnerDouble& x)
	{	return static_cast<int>(x.value_); }
}

namespace CppAD {
	inline bool GreaterThanZero(const cl::InnerDouble& x)
	{	return x > 0.; }
	inline bool GreaterThanOrZero(const cl::InnerDouble& x)
	{	return x >= 0.; }
	inline bool LessThanZero(const cl::InnerDouble& x)
	{	return x < 0.; }
	inline bool LessThanOrZero(const cl::InnerDouble& x)
	{	return x <= 0.; }
	inline bool abs_geq(const cl::InnerDouble& x, const cl::InnerDouble& y)
	{	return std::abs(x) >= std::abs(y); }
}

namespace CppAD {
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, acos)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, asin)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, atan)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, cos)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, cosh)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, exp)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, fabs)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, log)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, log10)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, sin)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, sinh)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, sqrt)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, tan)
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, tanh)
# if CPPAD_COMPILER_HAS_ERF
	CPPAD_STANDARD_MATH_UNARY(cl::InnerDouble, erf)
# endif
}

namespace CppAD {
	inline cl::InnerDouble abs(const cl::InnerDouble& x)
	{	return std::fabs(x); }
}

namespace CppAD {
	inline cl::InnerDouble sign(const cl::InnerDouble& x)
	{	if( x > 0. )
			return 1.;
		if( x == 0. )
			return 0.;
		return -1.;
	}
}

namespace CppAD {
	inline cl::InnerDouble pow(const cl::InnerDouble& x, const cl::InnerDouble& y)
	{ return std::pow(x, y); }
}

namespace CppAD {
	template <>
	class numeric_limits<cl::InnerDouble> {
	public:
		// machine epsilon
		static cl::InnerDouble epsilon(void)
		{	return std::numeric_limits<cl::InnerDouble>::epsilon(); }
		// minimum positive normalized value
		static cl::InnerDouble min(void)
		{	return std::numeric_limits<cl::InnerDouble>::min(); }
		// maximum finite value
		static cl::InnerDouble max(void)
		{	return std::numeric_limits<cl::InnerDouble>::max(); }
	};
	// deprecated machine epsilon
	template <> 
	inline cl::InnerDouble epsilon<cl::InnerDouble>(void)
	{	return numeric_limits<cl::InnerDouble>::epsilon(); }
}


# endif
