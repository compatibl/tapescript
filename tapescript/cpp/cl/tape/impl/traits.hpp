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

#ifndef cl_tape_impl_traits_hpp
#define cl_tape_impl_traits_hpp

namespace std
{
    template<typename Type>
    class _Ctraits;

    // TEMPLATE CLASS _Ctraits
    template<typename Base>
    class _Ctraits<cl::tape_wrapper<Base> >
    {    // complex traits for _Ty
    public:

        typedef cl::tape_wrapper<Base> _Ty;
        typedef typename 
            _Ty::value_type value_type;

        static _Ty _Flt_eps()
        {    // get epsilon
            return (numeric_limits<_Ty>::epsilon());
        }

        static _Ty _Flt_max()
        {    // get max
            return ((numeric_limits<_Ty>::max)());
        }

        static _Ty _Cosh(_Ty _Left, _Ty _Right)
        {    // return cosh(_Left) * _Right
            return (cosh(_Left) * _Right);
        }

        static _Ty _Exp(_Ty *_Pleft, _Ty _Right, short _Exponent)
        {    // compute exp(*_Pleft) * _Right * 2 ^ _Exponent
            *_Pleft = CppAD::exp((*_Pleft).value())* _Right * pow(2, _Exponent);
            return *_Pleft;
        }

        static _Ty _Infv(_Ty)
        {    // return infinity
            return value_type(_Inf._Double);
        }

        static bool _Isinf(_Ty _Left)
        {    // test for infinity
            double* _Tmp = reinterpret_cast<double*>(&_Left);
            return (_CSTD _Dtest(_Tmp) == _INFCODE);
        }

        static bool _Isnan(_Ty _Left)
        {    // test for NaN
            double* _Tmp = reinterpret_cast<double*>(&_Left);
            return (_CSTD _Dtest(_Tmp) == _NANCODE);
        }

        static _Ty _Nanv(_Ty)
        {    // return NaN
            return value_type(_Nan._Double);
        }

        static _Ty _Sinh(_Ty _Left, _Ty _Right)
        {    // return sinh(_Left) * _Right
            return (sinh(_Left) * _Right);
        }

        static _Ty asinh(_Ty _Left)
        {    // return asinh(_Left)
            static const _Ty _Ln2 = 0.69314718055994530941723212145817658L;

            bool _Neg = _Left < 0;
            _Ty _Ans;

            if (_Neg)
                _Left = -_Left;
            if (_Left < 2 / _Flt_eps())
                _Ans = log1p(_Left
                + _Left * _Left / (1 + sqrt(_Left * _Left + 1)));
            else
                _Ans = log(_Left) + _Ln2;
            return (_Neg ? -_Ans : _Ans);
        }

        static _Ty atan2(_Ty _Yval, _Ty _Xval)
        {    // return atan(_Yval / _Xval)
            typedef _Ty::ImplType ad_type;

            ad_type y = _Yval.value();
            ad_type x = _Xval.value();

            ad_type zero = 0.;
            ad_type pi = 3.1415926535897932384626433832795029L;

            ad_type ax = abs(x);
            ad_type ay = abs(y);

            ad_type alpha = atan(ay / ax);
            ad_type beta = pi / 2 - atan(ax / ay);
            ad_type theta = CondExpGt(ax, ay, alpha, beta);

            theta = CondExpLe(x, zero, pi - theta, theta);

            theta = CondExpLt(y, zero, -theta, theta);

            return theta;
        }

        static _Ty cos(_Ty _Left)
        {    // return cos(_Left)
            return (_CSTD CppAD::cos(_Left.value()));
        }

        static _Ty exp(_Ty _Left)
        {    // return exp(_Left)
            return (_CSTD CppAD::exp(_Left.value()));
        }

        static _Ty ldexp(_Ty _Left, int _Exponent)
        {    // return _Left * 2 ^ _Exponent
            return _Left * std::pow(2, _Exponent);
        }

        static _Ty log(_Ty _Left)
        {    // return log(_Left)
            return CppAD::log(_Left.value());
        }

        static _Ty log1p(_Ty _Left)
        {    // return log(1 + _Left)
            if (_Left.value() < -1)
                return (_Nanv(_Left));
            else if (_Left.value() == 0)
                return (_Left);
            else
            {    // compute log(1 + _Left) with fixup for small _Left
                _Ty _Leftp1 = 1 + _Left;
                return (log(_Leftp1) - ((_Leftp1 - 1) - _Left) / _Leftp1);
            }
        }

        static _Ty pow(_Ty _Left, _Ty _Right)
        {    // return _Left ^ _Right
            return (_CSTD CppAD::pow(_Left.value(), _Right.value()));
        }

        static _Ty sin(_Ty _Left)
        {    // return sin(_Left)
            return (_CSTD CppAD::sin(_Left.value()));
        }

        static _Ty sqrt(_Ty _Left)
        {    // return sqrt(_Left)
            return CppAD::sqrt(cl::tapescript::value(_Left));
        }

        static _Ty tan(_Ty _Left)
        {    // return tan(_Left)
            return (_CSTD CppAD::tan(_Left.value()));
        }
    };
}


#endif //cl_tape_impl_traits_hpp
