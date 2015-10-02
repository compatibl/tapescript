#pragma once

#ifndef CL_INNER_DOUBLE_INCLUDED
#define CL_INNER_DOUBLE_INCLUDED

# include <limits>

namespace cl
{
    struct InnerDouble
    {
        InnerDouble(double v = 0.)
            : value_(v)
        {}

#define CL_INNER_DOUBLE_ASSIGN_OPERATOR(Op)                                 \
        inline InnerDouble& operator##Op(const InnerDouble& y)              \
        {                                                                   \
            value_ Op y.value_;                                             \
            return *this;                                                   \
        }

        CL_INNER_DOUBLE_ASSIGN_OPERATOR(= )
        CL_INNER_DOUBLE_ASSIGN_OPERATOR(+= )
        CL_INNER_DOUBLE_ASSIGN_OPERATOR(-= )
        CL_INNER_DOUBLE_ASSIGN_OPERATOR(*= )
        CL_INNER_DOUBLE_ASSIGN_OPERATOR(/= )
#undef CL_INNER_DOUBLE_ASSIGN_OPERATOR

        inline InnerDouble operator-()
        {
            return -value_;
        }

        double value_;
    };

    template <class Ostream>
    inline Ostream& operator<<(Ostream& o, const InnerDouble& x)
    {
        return o << x.value_;
    }

#define CL_BIN_INNER_DOUBLE_OPERATOR(Res, Op)                               \
    inline Res operator##Op(const InnerDouble& x, const InnerDouble& y)     \
    {                                                                       \
        return x.value_ Op y.value_;                                        \
    }

    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , != )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , == )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , >  )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , >= )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , <  )
    CL_BIN_INNER_DOUBLE_OPERATOR(bool  , <= )
    CL_BIN_INNER_DOUBLE_OPERATOR(InnerDouble, -  )
    CL_BIN_INNER_DOUBLE_OPERATOR(InnerDouble, *  )
    CL_BIN_INNER_DOUBLE_OPERATOR(InnerDouble, /  )
    CL_BIN_INNER_DOUBLE_OPERATOR(InnerDouble, +)
#undef CL_BIN_INNER_DOUBLE_OPERATOR
}

namespace std
{
#define CL_INNER_DOUBLE_FUNCTION(Res, Name)                                 \
    inline Res Name(const cl::InnerDouble& x)                               \
    {                                                                       \
        return Name(x.value_);                                              \
    }

    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, abs)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, fabs)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, acos)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, sqrt)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, asin)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, atan)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, cos)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, sin)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, cosh)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, sinh)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, exp)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, log)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, log10)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, tan)
    CL_INNER_DOUBLE_FUNCTION(cl::InnerDouble, tanh)
#undef CL_INNER_DOUBLE_FUNCTION
    
    inline cl::InnerDouble pow(const cl::InnerDouble& x, const cl::InnerDouble& y)
    {
        return std::pow(x.value_, y.value_);
    }
    

    // CLASS numeric_limits<cl::InnerDouble>
    template<> 
    class numeric_limits<cl::InnerDouble>
        : public _Num_float_base
    {
    public:
        typedef cl::InnerDouble _Ty;

        static _Ty(min)() _THROW0()
        {	// return minimum value
            return (DBL_MIN);
        }

        static _Ty(max)() _THROW0()
        {	// return maximum value
            return (DBL_MAX);
        }

        static _Ty lowest() _THROW0()
        {	// return most negative value
            return (-(max)());
        }

        static _Ty epsilon() _THROW0()
        {	// return smallest effective increment from 1.0
            return (DBL_EPSILON);
        }

        static _Ty round_error() _THROW0()
        {	// return largest rounding error
            return (0.5);
        }

        static _Ty denorm_min() _THROW0()
        {	// return minimum denormalized value
            return (_CSTD _Denorm._Double);
        }

        static _Ty infinity() _THROW0()
        {	// return positive infinity
            return (_CSTD _Inf._Double);
        }

        static _Ty quiet_NaN() _THROW0()
        {	// return non-signaling NaN
            return (_CSTD _Nan._Double);
        }

        static _Ty signaling_NaN() _THROW0()
        {	// return signaling NaN
            return (_CSTD _Snan._Double);
        }

        _STCONS(int, digits, DBL_MANT_DIG);
        _STCONS(int, digits10, DBL_DIG);

        _STCONS(int, max_digits10, 2 + DBL_MANT_DIG * 301L / 1000);

        _STCONS(int, max_exponent, (int)DBL_MAX_EXP);
        _STCONS(int, max_exponent10, (int)DBL_MAX_10_EXP);
        _STCONS(int, min_exponent, (int)DBL_MIN_EXP);
        _STCONS(int, min_exponent10, (int)DBL_MIN_10_EXP);
    };
}


#endif