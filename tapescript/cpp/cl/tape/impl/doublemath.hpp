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

#include <math.h>
#include <cl/tape/impl/double.hpp>
#include <complex>

#ifdef CL_TAPE_COMPLEX_ENABLED
#include <cl/tape/impl/complex.hpp>
#endif

/// Math in AD mode, in progress
namespace cl
{
    inline void throw_(char const* what)
    {
        throw std::exception(what);
    }
}

namespace checkers
{
    typedef cl::TapeDouble double_type;

    template <typename value>
    inline double v_(CppAD::AD<value > cppad)
    {
        return ext::Value(cppad);
    }

    inline double v_(cl::TapeDouble const& cppad)
    {
        return v_(cppad.value());
    }

    inline double v_(double v)
    {
        return v;
    }
}

using namespace checkers;

#if defined CL_CHECKER_MATH_ENABLE

#   define CL_CHECK(expr) if (expr == 0) cl::throw_("error in: " __FILE__);

#else

#   define CL_CHECK(expr)

#endif //CL_CHECKER_MATH_ENABLE

/// <summary>Provides math functions for TapeDouble.</summary>
namespace std
{
    template <typename Base>
    inline cl::tape_double<Base> fabs(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::fabs(ext::Value(x.value()))
            == ext::Value(CppAD::fabs(x.value())));

        return CppAD::fabs(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::fabs(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> abs(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::abs(v_(x))
            == (cl::tape_double<Base>)CppAD::abs(x.value()));

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
        // only if tape pointer is null, otherwise exception is thrown
#ifdef CL_TAPE_CPPAD
        CL_CHECK((double)cl::TapeDouble(std::floor((double)x))
            == std::floor((double)x));

        return  cl::TapeDouble(std::floor((double)x));
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
        // only if tape pointer is null, otherwise exception is thrown
        CL_CHECK((double)cl::TapeDouble(std::ceil((double)x))
            == std::ceil((double)x));

        return cl::TapeDouble(std::ceil((double)x));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::ceil(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> sqrt(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        //  Conversion from TapeDouble to double is able when
        // tape is null, otherwise exception happens
        CL_CHECK(std::sqrt(v_(x))
            == (cl::tape_double<Base>)CppAD::sqrt(x.value()));

        return CppAD::sqrt(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::sqrt(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> log(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::log(v_(x))
            == (cl::tape_double<Base>)CppAD::log(x.value()));

        return CppAD::log(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::log(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> exp(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::exp(v_(x))
            == (cl::tape_double<Base>)CppAD::exp(x.value()));

        return CppAD::exp(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::exp(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> sin(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::sin(v_(x))
            == (cl::tape_double<Base>)CppAD::sin(x.value()));

        return CppAD::sin(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::sin(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> cos(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::cos(v_(x))
            == (cl::tape_double<Base>)CppAD::cos(x.value()));

        return CppAD::cos(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::cos(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> tan(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::tan(v_(x))
            == (cl::tape_double<Base>)CppAD::tan(x.value()));

        return CppAD::tan(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::tan(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> asin(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::asin(v_(x))
            == (cl::tape_double<Base>)CppAD::asin(x.value()));

        return CppAD::asin(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::asin(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> acos(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::acos(v_(x))
            == (cl::tape_double<Base>)CppAD::acos(x.value()));

        return CppAD::acos(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::acos(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> atan(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::atan(v_(x))
            == (cl::tape_double<Base>)CppAD::atan(x.value()));

        return CppAD::atan(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atan(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> sinh(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::sinh(v_(x))
            == (cl::tape_double<Base>)CppAD::sinh(x.value()));

        return CppAD::sinh(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::sinh(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> cosh(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::cosh(v_(x))
            == (cl::tape_double<Base>)CppAD::cosh(x.value()));

        return CppAD::cosh(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::cosh(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> tanh(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::tanh(v_(x))
            == (cl::tape_double<Base>)CppAD::tanh(x.value()));

        return CppAD::tanh(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::tanh(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> pow(cl::tape_double<Base> x, cl::tape_double<Base> y)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::pow(v_(x), v_(y))
            == (cl::tape_double<Base>)CppAD::pow(x.value(), y.value()));

        return CppAD::pow(x.value(), y.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::pow(x.value(), y.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> pow(cl::tape_double<Base> x, double y)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::pow(v_(x), y)
            == (cl::tape_double<Base>)pow(x, cl::tape_double<Base>(y)));

        return pow(x, cl::tape_double<Base>(y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::pow(x.value(), y);
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> pow(Base x, cl::tape_double<Base> y)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(std::pow(x, v_(y))
            == (cl::tape_double<Base>)pow(cl::tape_double<Base>(x), y));

        return pow(cl::tape_double<Base>(x), y);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::pow(x, y.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> asinh(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CppAD::AD<Base> _x = x.value(), _x2 = _x * _x;
        return CppAD::CondExpGt(CppAD::abs(_x), CppAD::AD<Base>(0.001)
            , (_x > 0) ? CppAD::log(_x) + CppAD::log(1.0 + CppAD::sqrt(1.0 + (1 / _x) * (1 / _x))) :
            -CppAD::log(-_x) - CppAD::log(1.0 + CppAD::sqrt(1.0 + (1 / _x) * (1 / _x)))
            , _x * (1.0 + _x2 * (-1 / 6.0 + _x2 * 3 / 40)));


#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::asinh(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> acosh(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        return log(x) + log(1.0 + sqrt(1.0 - (1 / x) * (1 / x)));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::acosh(x.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> atanh(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD
        CppAD::AD<Base> _x = x.value(), _x2 = _x * _x;
        return CppAD::CondExpGt(CppAD::abs(_x), CppAD::AD<Base>(0.001)
            , 0.5 * (CppAD::log(1.0 + _x) - CppAD::log(1.0 - _x))
            , _x * (1.0 + _x2 * (1 / 3.0 + _x2 * 0.2)));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atanh(x.value());
#endif
    }

    template <typename Base>
    inline bool isnan(cl::tape_double<Base> x)
    {
#ifdef CL_TAPE_CPPAD

        CL_CHECK(std::isnan(v_(x))
            == CppAD::isnan(x.value()));

        return CppAD::isnan(x.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::isnan(x.value());
#endif
    }

} // namespace std

namespace cl
{
    template <class Ty>
    struct is_tape_type
        : std::false_type
    {};

    template <class Base>
    struct is_tape_type<cl::tape_double<Base>>
        : std::true_type
    {};

    template <typename Base>
    inline cl::tape_double<Base> min_impl(cl::tape_double<Base> const& x, Base const& y)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::CondExpLt(x.value(), CppAD::AD<Base>(y), x.value(), CppAD::AD<Base>(y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::min(x.value(), y);
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> min_impl(Base const& x, cl::tape_double<Base> const& y)
    {
        return min_impl(y, x);
    }

    template <typename Base>
    inline cl::tape_double<Base> max_impl(cl::tape_double<Base> const& x, Base const& y)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::CondExpGt(x.value(), CppAD::AD<Base>(y), x.value(), CppAD::AD<Base>(y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::max(x.value(), y);
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> max_impl(Base const& x, cl::tape_double<Base> const& y)
    {
        return max_impl(y, x);
    }
} // namespace cl

namespace std
{
    template <typename Base>
    inline cl::tape_double<Base> min(cl::tape_double<Base> const& x, cl::tape_double<Base> const& y)
    {
#if defined CL_TAPE_CPPAD
        return CppAD::CondExpLt(x.value(), y.value(), x.value(), y.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::min(x.value(), y.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> max(cl::tape_double<Base> const& x, cl::tape_double<Base> const& y)
    {
#ifdef CL_TAPE_CPPAD
        return CppAD::CondExpGt(x.value(), y.value(), x.value(), y.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::max(x.value(), y.value());
#endif
    }

    template <class Ty>
    inline typename std::enable_if<
        cl::is_tape_type<Ty>::value
        , Ty
    >::type max(Ty const& x, typename Ty::base_type const& y)
    {
        return cl::max_impl(x, y);
    }

    template <class Ty>
    inline typename std::enable_if<
        cl::is_tape_type<Ty>::value
        , Ty
    >::type max(typename Ty::base_type const& x, Ty const& y)
    {
        return cl::max_impl(x, y);
    }

    template <class Ty>
    inline typename std::enable_if<
        cl::is_tape_type<Ty>::value
        , Ty
    >::type min(Ty const& x, typename Ty::base_type const& y)
    {
        return cl::min_impl(x, y);
    }

    template <class Ty>
    inline typename std::enable_if<
        cl::is_tape_type<Ty>::value
        , Ty
    >::type min(typename Ty::base_type const& x, Ty const& y)
    {
        return cl::min_impl(x, y);
    }

    template <typename Base>
    inline cl::tape_double<Base> atan2(cl::tape_double<Base> x, cl::tape_double<Base> y)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(v_(CppAD::atan2(x.value(), y.value()))
            == std::atan2(v_(x), v_(y)));

        return CppAD::atan2(x.value(), y.value());
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atan2(x.value(), y.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> atan2(cl::tape_double<Base> x, Base y)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(v_(CppAD::atan2(x.value(), double_type(y).value()))
            == std::atan2(v_(x), v_(y)));

        return atan2(x, cl::tape_double<Base>(y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atan2(x.value(), y);
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> atan2(Base x, cl::tape_double<Base> y)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(v_(CppAD::atan2(double_type(x).value(), y.value()))
            == std::atan2(v_(x), v_(y)));

        return atan2(cl::tape_double<Base>(x), y);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::atan2(x, y.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> fmod(cl::tape_double<Base> x, cl::tape_double<Base> y)
    {
#ifdef CL_TAPE_CPPAD
        CL_CHECK(v_(x - y * (x / y > 0 ? floor(x / y) : ceil(x / y)))
            == std::fmod(v_(x), v_(y)));

        return x - y * (x / y > 0 ? floor(x / y) : ceil(x / y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::fmod(x.value(), y.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> fmod(cl::tape_double<Base> x, Base y)
    {
#ifdef CL_TAPE_CPPAD
        return fmod(x, cl::tape_double<Base>(y));
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::fmod(x.value(), y);
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> fmod(Base x, cl::tape_double<Base> y)
    {
#ifdef CL_TAPE_CPPAD
        return fmod(cl::tape_double<Base>(x), y);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        return std::fmod(x, y.value());
#endif
    }

    template <typename Base>
    inline cl::tape_double<Base> modf(cl::tape_double<Base> x, cl::tape_double<Base>* iptr)
    {
#ifdef CL_TAPE_CPPAD
        Base fractpart;
        Base intpart;
        fractpart = std::modf((Base)x, &intpart);     //!!! Review
        (*iptr) = cl::tape_double<Base>(intpart);
        return cl::tape_double<Base>(fractpart);
#elif CL_TAPE_ADOLC
        cl::throw_("Not implemented"); return x;
#else
        Base fractpart;
        Base intpart;
        fractpart = std::modf(x.value(), &intpart);
        (*iptr) = intpart;
        return cl::tape_double<Base>( fractpart);
#endif
    }

} // namespace std

namespace cl
{
    template <class It1, class It2, class Meta = void>
    struct tape_type_from_iter {};

    template <class It1, class It2>
    struct tape_type_from_iter<
        It1, It2
        , typename std::enable_if<
            cl::is_tape_type<
                typename std::iterator_traits<It1>::value_type
            >::value
            || cl::is_tape_type<
                typename std::iterator_traits<It2>::value_type
            >::value
        >::type
    >
    {
        typedef typename std::iterator_traits<It1>::value_type type1;
        typedef typename std::iterator_traits<It2>::value_type type2;
        typedef typename
            std::conditional<
                cl::is_tape_type<type1>::value
                , type1
                , type2
            >::type type;
    };
}

namespace std
{
//!! Do we need this define if TapeDouble is always a class?
#if defined(CL_TAPE_NOAD) || defined(CL_TAPE_CPPAD) || defined(CL_TAPE_ADOLC)
    template<class In1, class In2>
    inline typename cl::tape_type_from_iter<In1, In2>::type inner_product(In1 first1, In1 last1, In2 first2, double value)
    {
        typedef typename cl::tape_type_from_iter<In1, In2>::type tape_type;
        tape_type prod(value);
        while (first1 != last1)
        {
            prod += (*first1) * (*first2);
            ++first1; ++first2;
        }
        return prod;
    }
#endif

    // Arithmetics for std::complex<cl::TapeDouble> and cl::TapeDouble

    inline std::complex<cl::TapeDouble> operator+(
        const std::complex<cl::TapeDouble>& lhs, cl::TapeDouble rhs)
    {
        complex<cl::TapeDouble> temp = lhs;
        return temp += rhs;
    }

    inline std::complex<cl::TapeDouble> operator-(
        const std::complex<cl::TapeDouble>& lhs, cl::TapeDouble rhs)
    {
        complex<cl::TapeDouble> temp = lhs;
        return temp -= rhs;
    }

    inline std::complex<cl::TapeDouble> operator*(
        const std::complex<cl::TapeDouble>& lhs, cl::TapeDouble rhs)
    {
        complex<cl::TapeDouble> temp = lhs;
        return temp *= rhs;
    }

    inline std::complex<cl::TapeDouble> operator/(
        const std::complex<cl::TapeDouble>& lhs, cl::TapeDouble rhs)
    {
        complex<cl::TapeDouble> temp = lhs;
        return temp /= rhs;
    }

    inline std::complex<cl::TapeDouble> operator+(
        cl::TapeDouble lhs, const std::complex<cl::TapeDouble>& rhs)
    {
        complex<cl::TapeDouble> temp = rhs;
        return temp += lhs;
    }

    inline std::complex<cl::TapeDouble> operator-(
        cl::TapeDouble lhs, const std::complex<cl::TapeDouble>& rhs)
    {
        complex<cl::TapeDouble> temp = lhs;
        return temp -= rhs;
    }

    // Arithmetics for std::complex<cl::TapeDouble> and double
    inline std::complex<cl::TapeDouble>
    operator*(cl::TapeDouble lhs, const std::complex<cl::TapeDouble>& rhs)
    {
        complex<cl::TapeDouble> temp = rhs;
        return temp *= lhs;
    }

    inline cl::TapeDouble
    pow(cl::TapeDouble const &_Left, int _Right)
    {
        //   The error can happen here
        // if we use _Right as argument of pow
        CL_CHECK(_Pow_int(_Left, _Right)
            == std::pow(v_(_Left), _Right));

        return _Pow_int(_Left, _Right);
    }

    inline std::complex<cl::TapeDouble>
    operator/(cl::TapeDouble lhs, const std::complex<cl::TapeDouble>& rhs)
    {
#if defined CL_TAPE_COMPLEX_ENABLED
        complex<cl::TapeDouble> temp = lhs;
        return temp /= rhs;
#else
        return std::complex<cl::TapeDouble>(lhs, 0.0) / rhs;
#endif
    }

    // Arithmetics for std::complex<cl::TapeDouble> and double
#if defined CL_TAPE_COMPLEX_ENABLED
    complex<cl::TapeDouble> inline pow_(
        complex<cl::TapeDouble> const &_Left
        , complex<cl::TapeDouble> const &_First
        , int _Right, bool _Even)
    {
        if (_Right <= 1)
            return _Even ? _Left : _Left*_First;

        return pow_(_Left * _Left, _First, _Right / 2, _Even);
    }

    //template <typename Right>
    inline complex<cl::TapeDouble>
    pow(complex<cl::TapeDouble> const & _Left, int _Right)
    {
        if (_Right == 0)
            return complex<cl::TapeDouble>(1.0);

        return _Right == 1 ? _Left : pow_(_Left * _Left, _Left
            , _Right / 2, (_Right / 2) * 2 == _Right);
    }

#endif

    inline std::complex<cl::TapeDouble>
    operator+(const std::complex<cl::TapeDouble>& lhs, double rhs)
    { return lhs + cl::TapeDouble(rhs); }

    inline std::complex<cl::TapeDouble>
    operator-(const std::complex<cl::TapeDouble>& lhs, double rhs)
    { return lhs - cl::TapeDouble(rhs); }

    inline std::complex<cl::TapeDouble>
    operator*(const std::complex<cl::TapeDouble>& lhs, double rhs)
    { return lhs * cl::TapeDouble(rhs); }

    inline std::complex<cl::TapeDouble>
    operator/(const std::complex<cl::TapeDouble>& lhs, double rhs)
    { return lhs / cl::TapeDouble(rhs); }

    inline std::complex<cl::TapeDouble>
    operator+(double lhs, const std::complex<cl::TapeDouble>& rhs)
    { return cl::TapeDouble(lhs) + rhs; }

    inline std::complex<cl::TapeDouble>
    operator-(double lhs, const std::complex<cl::TapeDouble>& rhs)
    { return cl::TapeDouble(lhs) - rhs; }

    inline std::complex<cl::TapeDouble>
    operator*(double lhs, const std::complex<cl::TapeDouble>& rhs)
    { return cl::TapeDouble(lhs) * rhs; }

    inline std::complex<cl::TapeDouble>
    operator/(double lhs, const std::complex<cl::TapeDouble>& rhs)
    { return cl::TapeDouble(lhs) / rhs; }


#if defined CL_TAPE_COMPLEX_ENABLED

#define CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(FUN)                                        \
    inline std::complex<cl::TapeDouble> FUN(const std::complex<cl::TapeDouble>& lhs)    \
    {                                                                                   \
        if (lhs.mode_ == std::complex<cl::TapeDouble>::RealBase)                        \
        {                                                                               \
            return std::FUN<cl::TapeDouble>(lhs);                                       \
        }                                                                               \
        return std::complex<cl::TapeDouble>(FUN(lhs.complex_base_));                    \
    }

    CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(sqrt)
    CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(exp)
    CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(log)
    CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(log10)
    CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(sin)
    CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(cos)
    CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(sinh)
    CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(cosh)
    //CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(acos)
    //CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(asin)
    //CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(atan)

    inline std::complex<cl::TapeDouble> tan(const std::complex<cl::TapeDouble>& lhs)
    {
        if (lhs.mode_ == std::complex<cl::TapeDouble>::RealBase)
        {
            return std::tan<cl::TapeDouble>(lhs);
        }
        return std::complex<cl::TapeDouble>(
            sin(lhs.complex_base_) / cos(lhs.complex_base_));
    }

    inline std::complex<cl::TapeDouble> tanh(const std::complex<cl::TapeDouble>& lhs)
    {
        if (lhs.mode_ == std::complex<cl::TapeDouble>::RealBase)
        {
            return std::tanh<cl::TapeDouble>(lhs);
        }
        return std::complex<cl::TapeDouble>(
            sinh(lhs.complex_base_) / cosh(lhs.complex_base_));
    }

    inline complex<cl::TapeDouble> pow(
        complex<cl::TapeDouble> const &_Left, cl::TapeDouble _Right)
    {
        return exp(_Right * log(_Left));
    }

    inline complex<cl::TapeDouble> pow(
        cl::TapeDouble const &_Left, complex<cl::TapeDouble> const &_Right)
    {
        return exp(_Right * log(complex<cl::TapeDouble>(_Left)));
    }

    inline complex<cl::TapeDouble> pow(
        complex<cl::TapeDouble> const &_Left
        , complex<cl::TapeDouble> const &_Right)
    {
        return exp(_Right * log(_Left));
    }


#undef CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC

#endif

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