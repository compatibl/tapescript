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

#ifndef __cl_tape_impl_complex_hpp__
#define __cl_tape_impl_complex_hpp__

#include <complex>

namespace ext = CppAD;

namespace std
{
    // The complex partial specialization
    template<>
    class complex<cl::CppDouble>
    {
    public:

        typedef cl::CppDouble real_type;
        typedef complex<cl::CppDouble> complex_type;
        typedef ext::AD<complex<double> > value_type;

        enum en_complex_type { None = 0 , RealBase = 1 >> 1, ComplBase = 1 >> 2, BothBase = 1 >> 3 };

        //  If we initialized by certain values this is real base type
        // and type of valaue is
        complex(real_type const& real, real_type const& imag)
            : complex_(real.value(), imag.value())
            , value_()
            , mode_(RealBase)
        {
        }

        complex(double v)//real_type const& real, real_type const& imag)
            : complex_(v, 0.0)
            , value_()
            , mode_(RealBase)
        {
        }

        // This call whn resize vector
        complex() : complex_()
            , value_()
            , mode_(None)
        {    }

        inline real_type real(real_type const& right)
        {	
            if (mode_ & RealBase)
            {
                complex_.real(right.value());
            }
            if (mode_ & ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                value_ = complex<double>(ext::Value(right.value()), 0);
            }
            return right;
        }

        inline real_type imag(const real_type& right)
        {
            if (mode_ & RealBase)
            {
                complex_.imag(right.value());
            }
            if (mode_ & ComplBase)
            {
                cl::CheckParameter(right.value());
                value_ = complex<double>(0, ext::Value(right.value()));
            }
            return right;
        }

        inline real_type real() const
        {	
            if (mode_ & RealBase)
            {
                return complex_.real();
            }
            return ext::Value(value_).real();
        }

        inline real_type imag() const
        {
            if (mode_ & RealBase)
            {
                return complex_.imag();
            }
            return ext::Value(value_).imag();
        }

        inline complex_type& operator=(const real_type& right)
        {	
            if (mode_ & RealBase)
            {
                complex_.real(right.value());
            }
            if (mode_ & ComplBase)
            {
                cl::CheckParameter(right.value());
                value_ = complex<double>(ext::Value(right.value()), 0);
            }
            return (*this);
        }

        inline complex_type& operator+=(const real_type& right)
        {
            if (mode_ & RealBase)
            {
                complex_ += right.value();
            }
            if (mode_ & ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                value_ += complex<double>(ext::Value(right.value()), 0);
            }
            return (*this);
        }

        inline complex_type& operator-=(const real_type& right)
        {
            if (mode_ & RealBase)
            {
                complex_ -= right.value();
            }

            if (mode_ & ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                value_ -= complex<double>(ext::Value(right.value()), 0);
            }

            return (*this);
        }

        inline complex_type& operator*=(const real_type& right)
        {
            if (mode_ & RealBase)
            {
                complex_ *= right.value();
            }
            if (mode_ & ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                value_ *= complex<double>(ext::Value(right.value()), 0);
            }
            return (*this);
        }

        inline complex_type& operator/=(const real_type& right)
        {
            if (mode_ & RealBase)
            {
                complex_ /= right.value();
            }
            if (mode_ & ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                value_ /= complex<double>(ext::Value(right.value()), 0);
            }
            return (*this);
        }

        inline complex_type& operator+=(const complex_type& right)
        {
            if (mode_ & RealBase)
            {
                complex_ += right.complex_;
            }
            if (mode_ & ComplBase)
            {
                value_ += right.value_;
            }
            return (*this);
        }

        inline complex_type& operator-=(const complex_type& right)
        {
            if (mode_ & RealBase)
            {
                complex_ -= right.complex_;
            }
            if (mode_ & ComplBase)
            {
                value_ -= right.value_;
            }
            return (*this);
        }

        inline complex_type& operator*=(const complex_type& right)
        {
            if (mode_ & RealBase)
            {
                complex_ *= right.complex_;
            }
            if (mode_ & ComplBase)
            {
                value_ *= right.value_;
            }
            return (*this);
        }

        inline complex_type& operator/=(const complex_type& right)
        {
            if (mode_ & RealBase)
            {
                // complex_ /= right.complex_;
            }
            if (mode_ & ComplBase)
            {
                value_ /= right.value_;
            }
            return (*this);
        }

        std::complex<ext::AD<double> > complex_;
        value_type value_;
        en_complex_type mode_;
    };

    complex<cl::CppDouble> inline
    pow(complex<cl::CppDouble> const & _Left, int _Right)
    {
        cl::throw_("Not implemented.");
        return _Left;
    }

}

#endif // __cl_tape_impl_complex_hpp__

