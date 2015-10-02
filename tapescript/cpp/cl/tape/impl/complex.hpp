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
        typedef cl::TapeInnerType<complex_double > complex_based_type;
        typedef std::complex<cl::TapeInnerType<double> > real_based_type;

        enum Complex_Mode { None = 0 , RealBase = (1 << 1), ComplBase = (1 << 2) };
        
        //  If we initialized by certain values this is real base type 
        // and type of valaue is 
        complex(real_type const& real, real_type const& imag = 0.0, Complex_Mode mode = ComplBase)
            : real_base_()
            , complex_base_()
            , mode_(mode)
        {
            if (mode_ == RealBase)
            {
                real_base_ = real_based_type(real.value(), imag.value());
            }
            if (mode_ == ComplBase)
            {
                bool is_variable = ext::Variable(cl::tapescript::cvalue(real))
                    || ext::Variable(cl::tapescript::cvalue(imag));

                if (is_variable)
                {
                    mode_ = RealBase;
                    real_base_ = real_based_type(real.value(), imag.value());
                }
                else
                {
                    complex_base_ = complex_double(ext::Value(real.value()), ext::Value(imag.value()));
                }
            }
        }

        // This call whn resize vector
        complex() 
            : real_base_()
            , complex_base_()
            , mode_(ComplBase)
        {    }

        complex(complex const& other) 
            : real_base_(other.real_base_)
            , complex_base_(other.complex_base_)
            , mode_(other.mode_)
        {    }

        template<class Ty>
        explicit complex(complex<Ty> const & other, Complex_Mode mode = ComplBase)
            : complex(other.real(), other.imag(), mode)
        {	 }

        explicit complex(const complex_based_type & other)
            : real_base_()
            , complex_base_(other)
            , mode_(ComplBase)
        {    }

        inline void check_mode(Complex_Mode mode) const
        {
            if (mode != mode_)
                cl::throw_("Wrong tape mode.");
        }

        inline real_type real(real_type const& right)
        {
            if (mode_ == RealBase)
            {
                real_base_.real(right.value());
            }
            if (mode_ == ComplBase)
            {
                if (ext::Variable(right.value()))
                {
                    cl::CheckParameter(complex_base_);
                    mode_ = RealBase;
                    real_base_ = real_based_type(right.value(), ext::Value(complex_base_).imag());
                }
                else
                {
                    complex_base_ = complex<double>(ext::Value(right.value()), ext::Value(complex_base_).imag());
                }
            }
            return right;
        }

        inline real_type imag(const real_type& right)
        {
            if (mode_ == RealBase)
            {
                real_base_.imag(right.value());
            }
            if (mode_ == ComplBase)
            {
                if (ext::Variable(right.value()))
                {
                    cl::CheckParameter(complex_base_);
                    mode_ = RealBase;
                    real_base_ = real_based_type(ext::Value(complex_base_).real(), right.value());
                }
                else
                {
                    complex_base_ = complex<double>(ext::Value(complex_base_).real(), ext::Value(right.value()));
                }
            }
            return right;
        }

        inline real_type real() const
        {
            if (mode_ == RealBase)
            {
                return real_base_.real();
            }
            return ext::Value(complex_base_).real();
        }

        inline real_type imag() const
        {
            if (mode_ == RealBase)
            {
                return real_base_.imag();
            }
            return ext::Value(complex_base_).imag();
        }

        inline complex_type& operator=(complex_type const& right)
        {
            if (mode_ == RealBase)
            {
                if (right.mode_ == RealBase)
                {
                    real_base_ = right.real_base_;
                }
                else
                {
                    if (ext::Variable(right.complex_base_))
                    {
                        cl::CheckParameter(real_base_.real());
                        cl::CheckParameter(real_base_.imag());
                        mode_ = ComplBase;
                        complex_base_ = right.complex_base_;
                    }
                    else
                    {
                        real_base_ = real_based_type(right.real(), right.imag());
                    }
                }
            }
            if (mode_ == ComplBase)
            {
                if (right.mode_ == RealBase)
                {
                    if (ext::Variable(right.real_base_.real()) || ext::Variable(right.real_base_.imag()))
                    {
                        cl::CheckParameter(complex_base_);
                        mode_ = RealBase;
                        real_base_ = real_based_type(right.real(), right.imag());                        
                    }
                    else
                    {
                        complex_base_ = complex_double(ext::Value(right.real_base_.real()), ext::Value(right.real_base_.imag()));
                    }
                }
                else
                {
                    complex_base_ = right.complex_base_;
                }
            }

            return *this;
        }

        inline complex_type& operator=(double const& right)
        {
            if (mode_ == RealBase)
            {
                real_base_ = right;
            }
            if (mode_ == ComplBase)
            {
                complex_base_ = right;
            }
            return (*this);
        }

        inline complex_type& operator=(const real_type& right)
        {
            if (mode_ == RealBase)
            {
                real_base_ = right.value();
            }
            if (mode_ == ComplBase)
            {
                if (ext::Variable(right.value()))
                {
                    cl::CheckParameter(complex_base_);
                    mode_ = RealBase;
                    real_base_ = right.value();                    
                }
                else
                {
                    complex_base_ = complex<double>(ext::Value(right.value()), 0);
                }
            }

            return (*this);
        }

        inline complex_type& operator+=(const real_type& right)
        {
            if (mode_ == RealBase)
            {
                real_base_ += right.value();
            }
            if (mode_ == ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                complex_base_ += complex<double>(ext::Value(right.value()), 0);
            }
            return (*this);
        }

        inline complex_type& operator-=(const real_type& right)
        {
            if (mode_ == RealBase)
            {
                real_base_ -= right.value();
            }

            if (mode_ == ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                complex_base_ -= complex<double>(ext::Value(right.value()), 0);
            }

            return (*this);
        }

        inline complex_type& operator*=(const real_type& right)
        {
            if (mode_ == RealBase)
            {
                real_base_ *= right.value();
            }
            if (mode_ == ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                complex_base_ *= complex<double>(ext::Value(right.value()), 0);
            }
            return (*this);
        }

        inline complex_type& operator/=(const real_type& right)
        {
            if (mode_ == RealBase)
            {
                real_base_ /= right.value();
            }
            if (mode_ == ComplBase)
            {
                // !!!
                cl::CheckParameter(right.value());
                complex_base_ /= complex<double>(ext::Value(right.value()), 0);
            }
            return (*this);
        }

        inline complex_type& operator+=(const complex_type& right)
        {
            if (mode_ == RealBase)
            {
                if (right.mode_ == RealBase)
                {
                    real_base_ += right.real_base_;
                }
                else
                {
                    if (ext::Variable(right.complex_base_))
                    {
                        cl::CheckParameter(real_base_.real());
                        cl::CheckParameter(real_base_.imag());
                        mode_ = ComplBase;
                        complex_base_ = complex_double(ext::Value(real_base_.real()), ext::Value(real_base_.imag())) + right.complex_base_;
                    }
                    else
                    {
                        real_base_ += real_based_type(right.real(), right.imag());
                    }
                }
            }
            if (mode_ == ComplBase)
            {
                if (right.mode_ == RealBase)
                {
                    if (ext::Variable(right.real_base_.real()) || ext::Variable(right.real_base_.imag()))
                    {
                        cl::CheckParameter(complex_base_);
                        mode_ = RealBase;
                        real_base_ = real_based_type(ext::Value(complex_base_).real(), ext::Value(complex_base_).imag()) + right.real_base_;
                    }
                    else
                    {
                        complex_base_ += complex_double(ext::Value(right.real_base_.real()), ext::Value(right.real_base_.imag()));
                    }
                }
                else
                {
                    complex_base_ += right.complex_base_;
                }
            }
            return (*this);
        }

        inline complex_type& operator-=(const complex_type& right)
        {
            if (mode_ == RealBase)
            {
                real_base_ -= right.real_base_;
            }
            if (mode_ == ComplBase)
            {
                complex_base_ -= right.complex_base_;
            }
            return (*this);
        }

        inline complex_type& operator*=(const complex_type& right)
        {
            if (mode_ == RealBase)
            {
                real_base_ *= right.real_base_;
            }
            if (mode_ == ComplBase)
            {
                complex_base_ *= right.complex_base_;
            }
            return (*this);
        }

        inline complex_type& operator/=(const complex_type& right)
        {
            if (mode_ == RealBase)
            {
#if defined CL_OPERATOR_DIV_EQ_FIXED
                real_base_ /= right.real_base_;
#else
                cl::TapeInnerType<double> x1 = real_base_.real()
                    , y1 = real_base_.imag()
                    , x2 = right.real_base_.real()
                    , y2 = right.real_base_.imag()
                    , norm2 = pow(x2, 2) + pow(y2, 2);
                real_base_ = std::complex<cl::TapeInnerType<double> >(
                    (x1 * x2 + y1 * y2) / norm2
                    , (y1 * x2 - x1 * y2) / norm2);
                // cl::throw_("Can't use operator: " __FUNCSIG__);
#endif
            }
            if (mode_ == ComplBase)
            {
                complex_base_ /= right.complex_base_;
            }
            return (*this);
        }

        complex_based_type& complex_base()
        {
            check_mode(ComplBase);
            return complex_base_;
        }

        const complex_based_type& complex_base() const
        {
            check_mode(ComplBase);
            return complex_base_;
        }

        // Used when tape is recording for real or image part of complex value.
        real_based_type real_base_;
        // Used when tape is recording for the complex value as one variable.
        complex_based_type complex_base_;
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

