/*
Copyright (C) 2015-present CompatibL

Performance test results and finance-specific examples are available at:

http://www.modval.org/adjoint

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

#ifndef cl_tape_impl_detail_complexmath_impl_hpp
#define cl_tape_impl_detail_complexmath_impl_hpp

#include <math.h>
#include <complex>

#include <cl/tape/impl/double.hpp>
#include <cl/tape/impl/complex.hpp>


namespace cl
{
    namespace tapescript
    {
        // Arithmetics for std::complex<cl::tape_double> and cl::tape_double

        inline std::complex<cl::tape_double> operator+(
            const std::complex<cl::tape_double>& lhs, cl::tape_double const& rhs)
        {
            std::complex<cl::tape_double> temp = lhs;
            return temp += rhs;
        }

        inline std::complex<cl::tape_double> operator-(
            const std::complex<cl::tape_double>& lhs, cl::tape_double const& rhs)
        {
            std::complex<cl::tape_double> temp = lhs;
            return temp -= rhs;
        }

        inline std::complex<cl::tape_double> operator*(
            const std::complex<cl::tape_double>& lhs, cl::tape_double const& rhs)
        {
            std::complex<cl::tape_double> temp = lhs;
            return temp *= rhs;
        }

        inline std::complex<cl::tape_double> operator/(
            const std::complex<cl::tape_double>& lhs, cl::tape_double const& rhs)
        {
            std::complex<cl::tape_double> temp = lhs;
            return temp /= rhs;
        }

        inline std::complex<cl::tape_double> operator+(
            cl::tape_double const& lhs, const std::complex<cl::tape_double>& rhs)
        {
            std::complex<cl::tape_double> temp = rhs;
            return temp += lhs;
        }

        inline std::complex<cl::tape_double> operator-(
            cl::tape_double const& lhs, const std::complex<cl::tape_double>& rhs)
        {
            std::complex<cl::tape_double> temp = lhs;
            return temp -= rhs;
        }

        // Arithmetics for std::complex<cl::tape_double> and double
        inline std::complex<cl::tape_double>
            operator*(cl::tape_double const& lhs, const std::complex<cl::tape_double>& rhs)
        {
                std::complex<cl::tape_double> temp = rhs;
                return temp *= lhs;
            }

        inline std::complex<cl::tape_double>
            operator/(cl::tape_double const& lhs, const std::complex<cl::tape_double>& rhs)
        {
#if defined CL_TAPE_COMPLEX_ENABLED
                std::complex<cl::tape_double> temp = lhs;
                return temp /= rhs;
#else
                return std::complex<cl::tape_double>(lhs, 0.0) / rhs;
#endif
            }

        std::complex<cl::tape_double> inline pow_(
            std::complex<cl::tape_double> const &_Left
            , std::complex<cl::tape_double> const &_First
            , int _Right, bool _Even)
        {
            if (_Right <= 1)
                return _Even ? _Left : _Left*_First;

            return pow_(_Left * _Left, _First, _Right / 2, _Even);
        }

        //template <typename Right>
        inline std::complex<cl::tape_double> pow(
            std::complex<cl::tape_double> const & _Left, int _Right)
        {
            if (_Right == 0)
                return std::complex<cl::tape_double>(1.0);

            return _Right == 1 ? _Left : pow_(_Left * _Left, _Left
                , _Right / 2, (_Right / 2) * 2 == _Right);
        }


        inline std::complex<cl::tape_double> operator+(
            const std::complex<cl::tape_double>& lhs, double rhs)
        {
            return lhs + cl::tape_double(rhs);
        }

        inline std::complex<cl::tape_double> operator-(
            const std::complex<cl::tape_double>& lhs, double rhs)
        {
            return lhs - cl::tape_double(rhs);
        }

        inline std::complex<cl::tape_double> operator*(
            const std::complex<cl::tape_double>& lhs, double rhs)
        {
            return lhs * cl::tape_double(rhs);
        }

        inline std::complex<cl::tape_double> operator/(
            const std::complex<cl::tape_double>& lhs, double rhs)
        {
            return lhs / cl::tape_double(rhs);
        }

        inline std::complex<cl::tape_double> operator+(
            double lhs, const std::complex<cl::tape_double>& rhs)
        {
            return cl::tape_double(lhs) + rhs;
        }

        inline std::complex<cl::tape_double> operator-(
            double lhs, const std::complex<cl::tape_double>& rhs)
        {
            return cl::tape_double(lhs) - rhs;
        }

        inline std::complex<cl::tape_double> operator*(
            double lhs, const std::complex<cl::tape_double>& rhs)
        {
            return cl::tape_double(lhs) * rhs;
        }

        inline std::complex<cl::tape_double> operator/(
            double lhs, const std::complex<cl::tape_double>& rhs)
        {
            return cl::tape_double(lhs) / rhs;
        }


#define CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(FUN)                                        \
        template<typename Inner>                                                        \
        inline std::complex<cl::tape_wrapper<Inner>> FUN(                               \
            const std::complex<cl::tape_wrapper<Inner>>& lhs)                           \
        {                                                                               \
            if (lhs.mode_ == std::complex<cl::tape_wrapper<Inner>>::RealBase)           \
            {                                                                           \
                return std::FUN<cl::tape_wrapper<Inner>>(lhs);                          \
            }                                                                           \
            return std::complex<cl::tape_wrapper<Inner>>(FUN(lhs.complex_base_));       \
        }

        CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(sqrt);
        CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(exp);
        CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(log);
        CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(log10);
        CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(sin);
        CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(cos);
        CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(sinh);
        CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(cosh);
        //CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(acos);
        //CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(asin);
        //CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC(atan);

        inline std::complex<cl::tape_double> tan(const std::complex<cl::tape_double>& lhs)
        {
            if (lhs.mode_ == std::complex<cl::tape_double>::RealBase)
            {
                return std::tan<cl::tape_double>(lhs);
            }
            return std::complex<cl::tape_double>(
                sin(lhs.complex_base_) / cos(lhs.complex_base_));
        }

        inline std::complex<cl::tape_double> tanh(const std::complex<cl::tape_double>& lhs)
        {
            if (lhs.mode_ == std::complex<cl::tape_double>::RealBase)
            {
                return std::tanh<cl::tape_double>(lhs);
            }
            return std::complex<cl::tape_double>(
                sinh(lhs.complex_base_) / cosh(lhs.complex_base_));
        }

        inline std::complex<cl::tape_double> pow(
            std::complex<cl::tape_double> const &_Left, cl::tape_double const& _Right)
        {
            return exp(_Right * log(_Left));
        }

        inline std::complex<cl::tape_double> pow(
            cl::tape_double const &_Left, std::complex<cl::tape_double> const &_Right)
        {
            return exp(_Right * log(std::complex<cl::tape_double>(_Left)));
        }

        inline std::complex<cl::tape_double> pow(
            std::complex<cl::tape_double> const &_Left
            , std::complex<cl::tape_double> const &_Right)
        {
            return exp(_Right * log(_Left));
        }


#undef CL_TAPE_DOUBLE_COMPLEX_GENERIC_FUNC
    }
}

#endif  // cl_tape_impl_detail_doublemath_impl_hpp
