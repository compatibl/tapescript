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

#ifndef cl_tape_impl_complex_hpp
#define cl_tape_impl_complex_hpp

#include <complex>


namespace ext = CppAD;

namespace std
{
    /// <summary> 
    ///    The complex type based on tape double.
    /// </summary>
    template<>
    class complex<cl::TapeDouble>
    {
    public:
        typedef cl::TapeDouble real_type;
        typedef complex<cl::TapeDouble> complex_type;
        typedef complex<double> complex_double;
        typedef cl::TapeInnerType<complex_double > value_type;
        typedef std::complex<cl::TapeInnerType<double> > real_based_type;

        enum Complex_Mode { None = 0 , RealBase = (1 << 1), ComplBase = (1 << 2) };
        
        //  If we initialized by certain values this is real base type 
        // and type of valaue is 
        complex(real_type const& real, real_type const& imag = 0.0, Complex_Mode mode = ComplBase)
            : complex_()
            , value_()
            , mode_(mode)
        {
            if (mode_ == RealBase)
                complex_ = real_based_type(real.value(), imag.value());

            if (mode_ == ComplBase)
                value_ = complex_double((double)real, (double)imag);
        }

        /*complex(double real, double imag = 0.0, Complex_Mode en = ComplBase)
            : complex_(en == RealBase ? complex_type(real, imag) 
                                      : complex_type())
            , value_(en == ComplBase ? complex_double(real, imag) 
                                     : complex_double())
            , mode_(en)
        {
        }*/

        // This call whn resize vector
        complex() 
            : complex_()
            , value_()
            , mode_(None)
        {    }

        complex(complex const& other) 
            : complex_(other.complex_)
            , value_(other.value_)
            , mode_(other.mode_)
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

        inline complex_type& operator=(complex_type const& right)
        {
            this->value_ = right.value_;
            this->complex_ = right.complex_;
            this->mode_ = right.mode_;
            return *this;
        }

        inline complex_type& operator=(double const& right)
        {
            if (mode_ & RealBase)
            {
                complex_.real(right);
            }
            if (mode_ & ComplBase)
            {
                value_ = complex<double>(right, 0);
            }
            return (*this);
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
#if defined CL_OPERATOR_DIV_EQ_FIXED
                complex_ /= right.complex_;
#else
            cl::throw_("Can't use operator: " __FUNCSIG__);

#   pragma message ("Can't use operator: " __FUNCSIG__)
#endif
            }
            if (mode_ & ComplBase)
            {
                value_ /= right.value_;
            }
            return (*this);
        }

        std::complex<cl::TapeInnerType<double> > complex_;
        value_type value_;
        Complex_Mode mode_;
    };

    /*complex<cl::TapeDouble> inline
    pow(complex<cl::TapeDouble> const & _Left, int _Right)
    {
        cl::throw_("Not implemented.");
        return _Left;
    }*/

}

#endif // cl_tape_impl_complex_hpp

