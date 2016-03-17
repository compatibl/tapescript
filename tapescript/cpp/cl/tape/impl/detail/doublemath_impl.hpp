/*
Copyright (C) 2015-present CompatibL

Performance test results and finance-specific examples are available at:

http://www.tapescript.org

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

#ifndef cl_tape_impl_detail_doublemath_impl_hpp
#define cl_tape_impl_detail_doublemath_impl_hpp

#include <math.h>
#include <complex>

#include <cl/tape/impl/double.hpp>
#include <cl/tape/impl/detail/utils.hpp>

#ifdef CL_TAPE_COMPLEX_ENABLED
#include <cl/tape/impl/complex.hpp>
#endif


namespace checkers
{
    typedef cl::tape_double double_type;

    template <typename value>
    inline double v_(CppAD::AD<value > cppad)
    {
        return ext::Value(cppad);
    }

    inline double v_(cl::tape_double const& cppad)
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

/// <summary>Provides math functions for tape_double.</summary>
namespace cl
{
    namespace tapescript
    {
        template <typename Base>
        inline cl::tape_wrapper<Base> fabs(cl::tape_wrapper<Base> const& x)
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
        inline cl::tape_wrapper<Base> abs(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::abs(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::abs(x.value()));

            return CppAD::abs(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::abs(x.value());
#endif
        }

        template <class Base>
        inline cl::tape_wrapper<Base> floor(cl::tape_wrapper<Base> const& x)
        {
            // Conversion from tape_wrapper to Base is permitted
            // only if tape pointer is null, otherwise exception is thrown
#ifdef CL_TAPE_CPPAD
            CL_CHECK((Base)cl::tape_wrapper<Base>(std::floor((Base)x))
                == std::floor((Base)x));

            return  cl::tape_wrapper<Base>(std::floor((Base)x));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::floor(x.value());
#endif
        }

        template <class Base>
        inline cl::tape_wrapper<Base> ceil(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            // Conversion from tape_wrapper to Base is permitted
            // only if tape pointer is null, otherwise exception is thrown
            CL_CHECK((Base)cl::tape_wrapper<Base>(std::ceil((Base)x))
                == std::ceil((Base)x));

            return cl::tape_wrapper<Base>(std::ceil((Base)x));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::ceil(x.value());
#endif
        }

        inline cl::tape_inner_type<cl::tape_value>
        round(const cl::tape_inner_type<cl::tape_value>& x)
        {
            cl::throw_("Not implemented");
            return x;
        }

        template <class Base>
        CppADCreateDiscrete(Base, round);

        template <typename Base>
        inline cl::tape_wrapper<Base> round(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            // Conversion from tape_double to native double is permitted
            // only if tape pointer is null, otherwise exception is thrown
            CL_CHECK((Base)cl::tape_double(round(x.value()))
                == round(x.value()));

            return  cl::tape_wrapper<Base>(round(x.value()));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::round(x.value());
#endif
        }

        inline cl::tape_inner_type<cl::tape_value> trunc(const cl::tape_inner_type<cl::tape_value>& x)
        {
            cl::throw_("Not implemented"); return x;
        }

        template <class Base>
        CppADCreateDiscrete(Base, trunc);

        template <typename Base>
        inline cl::tape_wrapper<Base> trunc(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            // Conversion from tape_double to native double is permitted
            // only if tape pointer is null, otherwise exception is thrown
            CL_CHECK((Base)cl::tape_double(std::trunc(x.value()))
                == std::trunc(x.value()));

            return  cl::tape_wrapper<Base>(trunc(x.value()));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::trunc(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> sqrt(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            // Conversion from tape_wrapper to Base is able when
            // tape is null, otherwise exception happens
            CL_CHECK(std::sqrt(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::sqrt(x.value()));

            return CppAD::sqrt(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::sqrt(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> log(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::log(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::log(x.value()));

            return CppAD::log(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::log(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> exp(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::exp(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::exp(x.value()));

            return CppAD::exp(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::exp(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> sin(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::sin(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::sin(x.value()));

            return CppAD::sin(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::sin(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> cos(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::cos(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::cos(x.value()));

            return CppAD::cos(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::cos(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> tan(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::tan(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::tan(x.value()));

            return CppAD::tan(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::tan(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> asin(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::asin(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::asin(x.value()));

            return CppAD::asin(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::asin(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> acos(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::acos(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::acos(x.value()));

            return CppAD::acos(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::acos(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> atan(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::atan(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::atan(x.value()));

            return CppAD::atan(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::atan(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> sinh(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::sinh(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::sinh(x.value()));

            return CppAD::sinh(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::sinh(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> cosh(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::cosh(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::cosh(x.value()));

            return CppAD::cosh(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::cosh(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> tanh(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::tanh(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::tanh(x.value()));

            return CppAD::tanh(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::tanh(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> erf(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::erf(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::erf(x.value()));

            return CppAD::erf(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::erf(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> pow(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::pow(v_(x), v_(y))
                == (cl::tape_wrapper<Base>)CppAD::pow(x.value(), y.value()));

            return CppAD::pow(x.value(), y.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::pow(x.value(), y.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> pow(cl::tape_wrapper<Base> const& x, Base const& y)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::pow(v_(x), y)
                == (cl::tape_wrapper<Base>)pow(x, cl::tape_wrapper<Base>(y)));

            return pow(x, cl::tape_wrapper<Base>(y));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::pow(x.value(), y);
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> pow(Base const& x, cl::tape_wrapper<Base> const& y)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::pow(x, v_(y))
                == (cl::tape_wrapper<Base>)pow(cl::tape_wrapper<Base>(x), y));

            return pow(cl::tape_wrapper<Base>(x), y);
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::pow(x, y.value());
#endif
        }

        template <class Base>
        inline cl::tape_wrapper<Base> pow(cl::tape_wrapper<Base> Left, int Right)
        {
            // The error can happen here
            // if we use _Right as argument of pow
            CL_CHECK(_Pow_int(Left, Right)
                == std::pow(v_(Left), Right));

            if (Right == 0)
            {
                cl::tape_wrapper<Base> zero = Left - Left;
                return 1.0 + zero;
            }

            unsigned int N;
            if (Right >= 0)
                N = (unsigned int)Right;
            else
                N = (unsigned int)(-Right);
            for (cl::tape_wrapper<Base> Z = cl::tape_wrapper<Base>(1.0);; Left *= Left)
            {
                if ((N & 1) != 0)
                    Z *= Left;
                if ((N >>= 1) == 0)
                    return (Right < 0 ? cl::tape_wrapper<Base>(1.0) / Z : Z);
            }
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> log(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::log(v_(x), v_(y))
                == (cl::tape_wrapper<Base>)CppAD::log(x.value(), y.value()));

            return CppAD::log(x.value()) / CppAD::log(y.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::log(x.value(), y.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> log10(cl::tape_wrapper<Base> const& x)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(std::log10(v_(x))
                == (cl::tape_wrapper<Base>)CppAD::log10(x.value()));

            return CppAD::log10(x.value());
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::log10(x.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> asinh(cl::tape_wrapper<Base> const& x)
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
        inline cl::tape_wrapper<Base> acosh(cl::tape_wrapper<Base> const& x)
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
        inline cl::tape_wrapper<Base> atanh(cl::tape_wrapper<Base> const& x)
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
        inline bool isnan(cl::tape_wrapper<Base> const& x)
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

        template <class Base, class Right>
        inline cl::tape_wrapper<Base>
        pow_impl(cl::tape_wrapper<Base> left, Right right
            , std::false_type)
        {
            return CL_MATH_QUAL pow(left, right);
        }

        template <class Base, class Right>
        inline cl::tape_wrapper<Base>
        pow_impl(cl::tape_wrapper<Base> left, Right right
            , std::true_type)
        {
            if (!left.is_ptr())
            {
                auto& v = left.double_();
                return std::pow((double&)v, right);
            }

            return CL_MATH_QUAL pow(left, right);
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> min_impl(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
        {
#if defined CL_TAPE_CPPAD
            return CppAD::CondExpLt(get_ad_value(x), get_ad_value(y), get_ad_value(x), get_ad_value(y));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::min(x.value(), y.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> max_impl(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
        {
#ifdef CL_TAPE_CPPAD
            return CppAD::CondExpGt(get_ad_value(x), get_ad_value(y), get_ad_value(x), get_ad_value(y));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::max(x.value(), y.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> atan2(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(v_(CppAD::atan2(get_ad_value(x), get_ad_value(y)))
                == std::atan2(v_(x), v_(y)));

            return CppAD::atan2(get_ad_value(x), get_ad_value(y));

#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::atan2(x.value(), y.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> atan2(cl::tape_wrapper<Base> const& x, Base const& y)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(v_(CppAD::atan2(x.value(), double_type(y).value()))
                == std::atan2(v_(x), v_(y)));

            return atan2(x, cl::tape_wrapper<Base>(y));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::atan2(x.value(), y);
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> atan2(Base const& x, cl::tape_wrapper<Base> const& y)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(v_(CppAD::atan2(double_type(x).value(), y.value()))
                == std::atan2(v_(x), v_(y)));

            return atan2(cl::tape_wrapper<Base>(x), y);
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::atan2(x, y.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> fmod(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
        {
#ifdef CL_TAPE_CPPAD
            CL_CHECK(v_(x - y * ((x / y) > 0) ? floor(x / y) : ceil(x / y))
                == std::fmod(v_(x), v_(y)));

            return x - y * (((x / y) > 0) ? floor(x / y) : ceil(x / y));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::fmod(x.value(), y.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> fmod(cl::tape_wrapper<Base> const& x, Base const& y)
        {
#ifdef CL_TAPE_CPPAD
            return fmod(x, cl::tape_wrapper<Base>(y));
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::fmod(x.value(), y);
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> fmod(Base const& x, cl::tape_wrapper<Base> const& y)
        {
#ifdef CL_TAPE_CPPAD
            return fmod(cl::tape_wrapper<Base>(x), y);
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            return std::fmod(x, y.value());
#endif
        }

        template <typename Base>
        inline cl::tape_wrapper<Base> modf(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base>* iptr)
        {
#ifdef CL_TAPE_CPPAD
            Base fractpart;
            Base intpart;
            fractpart = std::modf((Base)x, &intpart);     //!!! Review
            (*iptr) = cl::tape_wrapper<Base>(intpart);
            return cl::tape_wrapper<Base>(fractpart);
#elif CL_TAPE_ADOLC
            cl::throw_("Not implemented"); return x;
#else
            Base fractpart;
            Base intpart;
            fractpart = std::modf(x.value(), &intpart);
            (*iptr) = intpart;
            return cl::tape_wrapper<Base>(fractpart);
#endif
        }
    }
}

namespace std
{
    //!! Do we need this define if tape_double is always a class?
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
}

#endif  // cl_tape_impl_detail_doublemath_impl_hpp
