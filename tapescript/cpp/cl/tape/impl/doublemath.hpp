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

#ifndef cl_tape_impl_doublemath_hpp
#define cl_tape_impl_doublemath_hpp

#include <cl/tape/impl/double.hpp>
#include <complex>

/// Math in AD mode, in progress
namespace cl
{
    inline void throw_(char const* what)
    {
        throw std::exception(what);
    }
}

/// <summary>Provides math functions for TapeDouble.</summary>
namespace std
{
    inline cl::TapeDouble fabs(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::fabs(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::fabs(x.value());
#endif
    }

    inline cl::TapeDouble abs(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::abs(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::abs(x.value());
#endif
    }

    inline cl::TapeDouble floor(cl::TapeDouble x)
    {
        // Conversion from TapeDouble to native double is permitted
        // only when tape pointer is null, otherwise exception is thrown
#ifdef CL_TAPE_CPPAD
        return  cl::TapeDouble(std::floor(CppAD::Value(x.value())));  //!!! Review
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::floor(x.value());
#endif
    }

    inline cl::TapeDouble ceil(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        // Conversion from TapeDouble to native double is permitted
        // only when tape pointer is null, otherwise exception is thrown
        return cl::TapeDouble(std::ceil(CppAD::Value(x.value())));  //!!! Review
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::ceil(x.value());
#endif
    }

    inline cl::TapeDouble sqrt(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::sqrt(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::sqrt(x.value());
#endif
    }

    inline cl::TapeDouble log(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::log(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::log(x.value());
#endif
    }

    inline cl::TapeDouble exp(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::exp(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::exp(x.value());
#endif
    }

    inline cl::TapeDouble sin(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::sin(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::sin(x.value());
#endif
    }

    inline cl::TapeDouble cos(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::cos(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::cos(x.value());
#endif
    }

    inline cl::TapeDouble tan(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::tan(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::tan(x.value());
#endif
    }

    inline cl::TapeDouble asin(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::asin(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::asin(x.value());
#endif
    }

    inline cl::TapeDouble acos(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::acos(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::acos(x.value());
#endif
    }

    inline cl::TapeDouble atan(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::atan(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atan(x.value());
#endif
    }

    inline cl::TapeDouble sinh(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::sinh(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::sinh(x.value());
#endif
    }

    inline cl::TapeDouble cosh(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::cosh(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::cosh(x.value());
#endif
    }

    inline cl::TapeDouble tanh(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::tanh(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::tanh(x.value());
#endif
    }

    inline cl::TapeDouble pow(cl::TapeDouble x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::pow(x.value(), y.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::pow(x.value(), y.value());
#endif
    }

    inline cl::TapeDouble pow(cl::TapeDouble x, double y)
    {
#ifdef CL_TAPE_CPPAD
        return pow(x, cl::TapeDouble(y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::pow(x.value(), y);
#endif
    }

    inline cl::TapeDouble pow(double x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return pow(cl::TapeDouble(x), y);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::pow(x, y.value());
#endif
    }

    inline cl::TapeDouble asinh(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return log(x + sqrt(cl::TapeDouble(1.0) + pow(x, 2.0)));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented");return x;
#else
        return std::asinh(x.value());
#endif
    }

    inline cl::TapeDouble acosh(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return log(x + sqrt(cl::TapeDouble(-1.0) + pow(x, 2.0)));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::acosh(x.value());
#endif
    }

    inline cl::TapeDouble atanh(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return 0.5* log((cl::TapeDouble(1.0) + x) / (cl::TapeDouble(1.0) - x));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atanh(x.value());
#endif
    }

    inline bool isnan(cl::TapeDouble x)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::isnan(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::isnan(x.value());
#endif
    }


    inline cl::TapeDouble min(cl::TapeDouble x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return x < y ? x : y;
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::min(x.value(),y.value());
#endif
    }

    inline cl::TapeDouble min(cl::TapeDouble x, double y)
    {
#ifdef CL_TAPE_CPPAD
        return x < y ? x : y;
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::min(x.value(), y);
#endif
    }

    inline cl::TapeDouble min(double x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return x < y ? x : y;
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented");return x;
#else
        return std::min(x, y.value());
#endif
    }

    inline cl::TapeDouble max(cl::TapeDouble x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return x > y ? x : y;
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::max(x.value(), y.value());
#endif
    }

    inline cl::TapeDouble max(cl::TapeDouble x, double y)
    {
#ifdef CL_TAPE_CPPAD
        return x > y ? x : cl::TapeDouble(y);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::max(x.value(), y);
#endif
    }

    inline cl::TapeDouble max(double x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return x > y ? cl::TapeDouble(x) : y;
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::max(x, y.value());
#endif
    }

    inline cl::TapeDouble atan2(cl::TapeDouble x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::atan2(x.value(), y.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atan2(x.value(), y.value());
#endif
    }

    inline cl::TapeDouble atan2(cl::TapeDouble x, double y)
    {
#ifdef CL_TAPE_CPPAD
        return atan2(x, cl::TapeDouble(y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atan2(x.value(), y);
#endif
    }

    inline cl::TapeDouble atan2(double x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return atan2(cl::TapeDouble(x), y);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atan2(x, y.value());
#endif
    }

    inline cl::TapeDouble fmod(cl::TapeDouble x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return x - y * ( x / y > 0 ? floor(x / y) : ceil (x / y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::fmod(x.value(), y.value());
#endif
    }

    inline cl::TapeDouble fmod(cl::TapeDouble x, double y)
    {
#ifdef CL_TAPE_CPPAD
        return fmod(x, cl::TapeDouble(y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::fmod(x.value(), y);
#endif
    }

    inline cl::TapeDouble fmod(double x, cl::TapeDouble y)
    {
#ifdef CL_TAPE_CPPAD
        return fmod(cl::TapeDouble(x), y);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::fmod(x, y.value());
#endif
    }

    inline cl::TapeDouble modf(cl::TapeDouble x, cl::TapeDouble* iptr)
    {
#ifdef CL_TAPE_CPPAD
        double fractpart;
        double intpart;
        fractpart = std::modf(CppAD::Value(x.value()), &intpart);     //!!! Review
        (*iptr) = cl::TapeDouble(intpart);
        return cl::TapeDouble( fractpart);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        double fractpart;
        double intpart;
        fractpart = std::modf(x.value(), &intpart);
        (*iptr) = intpart;
        return cl::TapeDouble( fractpart);
#endif
    }

//!! Do we need this define if TapeDouble is always a class?
#if defined(CL_TAPE_NOAD) || defined(CL_TAPE_CPPAD) || defined(CL_TAPE_ADOLC)
    template<class T1, class T2>
    inline cl::TapeDouble inner_product(T1 first1, T1 last1, T2 first2, double value)
    {
        cl::TapeDouble prod(value);
        while (first1 != last1)
        {
            prod += (*first1) * (*first2);
            ++first1; ++first2;
        }
        return prod;
    }
#endif

    // Arithmetics for std::complex<cl::TapeDouble> and cl::TapeDouble

    inline std::complex<cl::TapeDouble> operator+(const std::complex<cl::TapeDouble>& lhs, cl::TapeDouble rhs){
        return complex<cl::TapeDouble>(lhs.real() + rhs, lhs.imag());
    }

    inline std::complex<cl::TapeDouble> operator-(const std::complex<cl::TapeDouble>& lhs, cl::TapeDouble rhs){
        return complex<cl::TapeDouble>(lhs.real() - rhs, lhs.imag());
    }

    inline std::complex<cl::TapeDouble> operator*(const std::complex<cl::TapeDouble>& lhs, cl::TapeDouble rhs){
        return complex<cl::TapeDouble>(lhs.real() *rhs, lhs.imag()*rhs);
    }

    inline std::complex<cl::TapeDouble> operator/(const std::complex<cl::TapeDouble>& lhs, cl::TapeDouble rhs){
        return complex<cl::TapeDouble>(lhs.real() / rhs, lhs.imag() / rhs);
    }

    inline std::complex<cl::TapeDouble> operator+(cl::TapeDouble lhs, const std::complex<cl::TapeDouble>& rhs){
        return complex<cl::TapeDouble>(lhs + rhs.real(), rhs.imag());
    }

    inline std::complex<cl::TapeDouble> operator-(cl::TapeDouble lhs, const std::complex<cl::TapeDouble>& rhs){
        return complex<cl::TapeDouble>(lhs - rhs.real(), -rhs.imag());
    }

    inline std::complex<cl::TapeDouble> operator*(cl::TapeDouble lhs, const std::complex<cl::TapeDouble>& rhs){
        return complex<cl::TapeDouble>(rhs.real() *lhs, rhs.imag()*lhs);
    }

    inline std::complex<cl::TapeDouble> operator/(cl::TapeDouble lhs, const std::complex<cl::TapeDouble>& rhs){
        return std::complex<cl::TapeDouble>(lhs, 0.0) / rhs;
    }

    // Arithmetics for std::complex<cl::TapeDouble> and double

    inline std::complex<cl::TapeDouble> operator+(const std::complex<cl::TapeDouble>& lhs, double rhs) { return lhs + cl::TapeDouble(rhs); }

    inline std::complex<cl::TapeDouble> operator-(const std::complex<cl::TapeDouble>& lhs, double rhs) { return lhs - cl::TapeDouble(rhs); }

    inline std::complex<cl::TapeDouble> operator*(const std::complex<cl::TapeDouble>& lhs, double rhs) { return lhs * cl::TapeDouble(rhs); }

    inline std::complex<cl::TapeDouble> operator/(const std::complex<cl::TapeDouble>& lhs, double rhs) { return lhs / cl::TapeDouble(rhs); }

    inline std::complex<cl::TapeDouble> operator+(double lhs, const std::complex<cl::TapeDouble>& rhs) { return cl::TapeDouble(lhs) + rhs; }

    inline std::complex<cl::TapeDouble> operator-(double lhs, const std::complex<cl::TapeDouble>& rhs) { return cl::TapeDouble(lhs) - rhs; }

    inline std::complex<cl::TapeDouble> operator*(double lhs, const std::complex<cl::TapeDouble>& rhs) { return cl::TapeDouble(lhs) * rhs; }

    inline std::complex<cl::TapeDouble> operator/(double lhs, const std::complex<cl::TapeDouble>& rhs) { return cl::TapeDouble(lhs) / rhs; }

    //!! Providing implementation causes compilation error due to NaN not being defined for Real, to be resolved
}

#if defined CL_EXTERNAL_NAMESPACE && defined CL_USE_USING_STD
namespace CL_EXTERNAL_NAMESPACE
{
	using std::exp;
	using std::sqrt;
}
#endif

#endif  // cl_tape_impl_doublemath_hpp