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

#ifndef cl_tape_impl_doubleoperators_hpp
#define cl_tape_impl_doubleoperators_hpp

#include <cl/tape/impl/double.hpp>

#if !defined(CL_NO_BOOST_NUMERIC)
#   include <boost/numeric/ublas/fwd.hpp>
#   include <boost/numeric/ublas/matrix_proxy.hpp>
#endif

//!! Certain tape double operators
namespace cl
{
    namespace tapescript
    {
        /// Take value from tape double 
        template <typename TapeType>
        inline typename TapeType::value_type const& cvalue(TapeType const& tv)
        {
            return tv.value_;
        }

        /// Take const value from tape double 
        template <typename TapeType>
        inline typename TapeType::value_type& value(TapeType& tv)
        {
            return tv.value_;
        }
    }

    /// <summary>Returns the result of addition of two TapeDouble objects.</summary>
    inline TapeDouble operator+(const TapeDouble& lhs, const TapeDouble& rhs) { return cl::tapescript::cvalue(lhs) + cl::tapescript::cvalue(rhs); }

    /// <summary>Returns the result of subtraction of two TapeDouble objects.</summary>
    inline TapeDouble operator-(const TapeDouble& lhs, const TapeDouble& rhs) { return cl::tapescript::cvalue(lhs) - cl::tapescript::cvalue(rhs); }

    /// <summary>Returns the result of multiplication of two TapeDouble objects.</summary>
    inline TapeDouble operator*(const TapeDouble& lhs, const TapeDouble& rhs) { return cl::tapescript::cvalue(lhs) * cl::tapescript::cvalue(rhs); }

    /// <summary>Returns the result of division of two TapeDouble objects.</summary>
    inline TapeDouble operator/(const TapeDouble& lhs, const TapeDouble& rhs) { return cl::tapescript::cvalue(lhs) / cl::tapescript::cvalue(rhs); }

    /// <summary>Returns the result of addition of TapeDouble and double.</summary>
    inline TapeDouble operator+(const TapeDouble& lhs, double rhs) { return cl::tapescript::cvalue(lhs) + rhs; }

    /// <summary>Returns the result of subtraction of TapeDouble and double.</summary>
    inline TapeDouble operator-(const TapeDouble& lhs, double rhs) { return cl::tapescript::cvalue(lhs) - rhs; }

    /// <summary>Returns the result of multiplication of TapeDouble and double.</summary>
    inline TapeDouble operator*(const TapeDouble& lhs, double rhs) { return cl::tapescript::cvalue(lhs) * rhs; }

    /// <summary>Returns the result of division of TapeDouble and double.</summary>
    inline TapeDouble operator/(const TapeDouble& lhs, double rhs) { return cl::tapescript::cvalue(lhs) / rhs; }

    /// <summary>Returns the result of addition of double and TapeDouble.</summary>
    inline TapeDouble operator+(double lhs, const TapeDouble& rhs) { return lhs + cl::tapescript::cvalue(rhs); }

    /// <summary>Returns the result of subtraction of double and TapeDouble.</summary>
    inline TapeDouble operator-(double lhs, const TapeDouble& rhs) { return lhs - cl::tapescript::cvalue(rhs); }

    /// <summary>Returns the result of multiplication of double and TapeDouble.</summary>
    inline TapeDouble operator*(double lhs, const TapeDouble& rhs) { return lhs * cl::tapescript::cvalue(rhs); }

    /// <summary>Returns the result of division of double and TapeDouble.</summary>
    inline TapeDouble operator/(double lhs, const TapeDouble& rhs) { return lhs / cl::tapescript::cvalue(rhs); }

    /// <summary>Returns true if lhs is equal to rhs.</summary>
    inline bool operator==(double lhs, const TapeDouble& rhs) { return lhs == cl::tapescript::cvalue(rhs); }

    /// <summary>Returns true if lhs is not equal to rhs.</summary>
    inline bool operator!=(double lhs, const TapeDouble& rhs) { return lhs != cl::tapescript::cvalue(rhs); }

    /// <summary>Returns true if lhs is less than rhs.</summary>
    inline bool operator<(double lhs, const TapeDouble& rhs) { return lhs < cl::tapescript::cvalue(rhs); }

    /// <summary>Returns true if lhs is less than or equal to rhs.</summary>
    inline bool operator<=(double lhs, const TapeDouble& rhs) { return lhs <= cl::tapescript::cvalue(rhs); }

    /// <summary>Returns true if lhs is more than rhs.</summary>
    inline bool operator>(double lhs, const TapeDouble& rhs) { return lhs > cl::tapescript::cvalue(rhs); }

    /// <summary>Returns true if lhs is more than or equal to rhs.</summary>
    inline bool operator>=(double lhs, const TapeDouble& rhs) { return lhs >= cl::tapescript::cvalue(rhs); }

    /// <summary>Serialize to stream.</summary>
    inline std::ostream& operator<<(std::ostream& output, const TapeDouble& v) { output << cl::tapescript::cvalue(v); return output; }

    /// <summary>Deserialize from stream.</summary>
    inline std::istream& operator>>(std::istream& input, TapeDouble& v) { input >> cl::tapescript::value(v); return input; }

}

//!! Supporting code for double operators, in progress
namespace cl_ext
{
    template <typename _Ty>
    inline void print_type()
    {
#pragma message (__FUNCSIG__)
    }

    struct basic_operators;

    template <typename Left, typename Right>
    struct custom_operator
    {
        typedef basic_operators type;
    };

    /// Base template for operator override
    template <typename Left, typename Right, typename Operator
        , typename left_convertible = typename cl::tapescript::is_has_operator_real<Left>::type
        , typename right_convertible = typename cl::tapescript::is_has_operator_real<Right>::type
        , typename Custom = typename custom_operator<Left, Right>::type >
    struct operator_traits
    {
    };

    /// Operator + for class which has conversion to double
    template <typename Left, typename Right>
    struct operator_traits<Left, Right, struct oper_plus, std::true_type, std::true_type, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left.operator type() + right.operator type();
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right& right)
        {
            return left.operator type() + right.operator type();
        }
    };

    /// Operator - for class which has conversion to double
    template <typename Left, typename Right>
    struct operator_traits<Left, Right, struct oper_minus, std::true_type, std::true_type, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left.operator type() - right.operator type();
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right& right)
        {
            return left.operator type() - right.operator type();
        }
    };

    /// Operator + for class which has conversion to double for right hand side argument
    template <typename Left, typename RC>
    struct operator_traits<Left, cl::TapeDouble, struct oper_plus, std::true_type, RC, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left.operator type() + right;
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right const& right)
        {
            return left.operator type() + right;
        }
    };

    /// Operator + for class which has conversion to double for right hand side argument
    template <typename Right, typename LC>
    struct operator_traits<cl::TapeDouble, Right, struct oper_plus, LC, std::true_type, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left + right.operator type();
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right const& right)
        {
            return left + right.operator type();
        }
    };

    /// Operator - for class which has conversion to double for left hand side argument
    template <typename Left, typename RC>
    struct operator_traits<Left, cl::TapeDouble, struct oper_minus, std::true_type, RC, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left.operator type() - right;
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right const& right)
        {
            return left.operator type() - right;
        }
    };

    /// Operator - for class which has conversion to double for right hand side argument
    template <typename Right, typename LC>
    struct operator_traits<cl::TapeDouble, Right, struct oper_minus, LC, std::true_type, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left - right.operator type();
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right const& right)
        {
            return left - right.operator type();
        }
    };

    template <typename Left, typename Right>
    inline typename operator_traits<typename std::remove_const<Left>::type
        , typename std::remove_const<Right>::type, oper_minus>::type
        operator - (Left left, Right right)
    {

#if defined CL_COMPILE_TIME_DEBUG
#	pragma message ("overload operator - : " __FUNCSIG__)
#endif

            operator_traits<typename std::remove_const<Left>::type
                , typename std::remove_const<Right>::type, oper_minus> op;

            return op(left, right);
    }

    template <typename Left, typename Right>
    inline typename operator_traits<typename std::remove_const<Left>::type
        , typename std::remove_const<Right>::type, oper_plus>::type
        operator + (Left left, Right right)
    {

#if defined CL_COMPILE_TIME_DEBUG
#	pragma message ("overload operator + : " __FUNCSIG__)
#endif

        operator_traits<typename std::remove_const<Left>::type
            , typename std::remove_const<Right>::type, oper_plus> op;

        return op(left, right);
    }

    // Operator /
    template <typename Left, typename Right>
    struct operator_traits<Left, Right, struct oper_div, std::true_type, std::true_type, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left.operator type() / right.operator type();
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right& right)
        {
            return left.operator type() / right.operator type();
        }
    };

    template <typename Left, typename RC>
    struct operator_traits<Left, cl::TapeDouble, struct oper_div, std::true_type, RC, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left.operator type() / right;
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right const& right)
        {
            return left.operator type() / right;
        }
    };

    template <typename Right, typename LC>
    struct operator_traits<cl::TapeDouble, Right, struct oper_div, LC, std::true_type, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left / right.operator type();
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right const& right)
        {
            return left / right.operator type();
        }
    };

    template <typename Left, typename Right>
    inline typename operator_traits<typename std::remove_const<Left>::type
        , typename std::remove_const<Right>::type, struct oper_div>::type
        operator / (Left left, Right right)
    {

#if defined CL_COMPILE_TIME_DEBUG
#	pragma message ("overload operator / : " __FUNCSIG__)
#endif

        operator_traits<typename std::remove_const<Left>::type
            , typename std::remove_const<Right>::type, oper_div> op;

        return op(left, right);
    }

    // Operator *
    template <typename Left, typename Right>
    struct operator_traits<Left, Right, struct oper_mult, std::true_type, std::true_type, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left.operator type() * right.operator type();
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right& right)
        {
            return left.operator type() * right.operator type();
        }
    };

    template <typename Left, typename RC>
    struct operator_traits<Left, cl::TapeDouble, struct oper_mult, std::true_type, RC, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left.operator type() * right;
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right const& right)
        {
            return left.operator type() * right;
        }
    };

    template <typename Right, typename LC>
    struct operator_traits<cl::TapeDouble, Right, struct oper_mult, LC, std::true_type, basic_operators>
    {
        // This ensures correct behavior for classes convertible to double
        typedef cl::TapeDouble type;

        template <typename Left, typename Right>
        inline type operator()(Left const& left, Right const& right)
        {
            return left * right.operator type();
        }

        template <typename Left, typename Right>
        inline type operator()(Left& left, Right const& right)
        {
            return left * right.operator type();
        }
    };

    template <typename Left, typename Right>
    inline typename operator_traits<typename std::remove_const<Left>::type
        , typename std::remove_const<Right>::type, struct oper_mult>::type
        operator * (Left left, Right right)
    {
#if defined CL_COMPILE_TIME_DEBUG
#	pragma message ("overload operator * : " __FUNCSIG__)
#endif
        operator_traits<typename std::remove_const<Left>::type
            , typename std::remove_const<Right>::type, oper_mult> op;

        return op(left, right);
    }
}

// Operators in AD mode, in progress
namespace std
{
    template <typename _Ty>
    struct plus;

    template<>
    struct plus<cl::TapeDouble >
        : public binary_function<cl::TapeDouble, cl::TapeDouble, cl::TapeDouble >
    {   // functor for operator+
        template<class _Ty1, class _Ty2>
        cl::TapeDouble operator()(_Ty1&& _Left, _Ty2&& _Right) const
        {   // transparently apply operator+ to operands
            typedef decltype(cl::get_instance<cl::TapeDouble>() + cl::get_instance<cl::TapeDouble>()) real_type;
            return _Left + _Right;
        }
    };

    template <typename > struct multiplies;

    template <>
    struct multiplies<cl::TapeDouble >
        : public binary_function<cl::TapeDouble, cl::TapeDouble, typename cl::TapeDouble::value_type>
    {
        template <typename Left, typename Right>
        typename cl::TapeDouble::value_type operator () (Left const& left, Right const& right) const
		{
#if defined CL_TAPE_CPPAD
            return (left * right).value();
#else
			return (cl::TapeDouble::value_type)(left * right);
#endif
        }
    };

}

namespace boost_connectors
{
    template <bool If, typename Then, typename Else>
    struct IF {};

    template <typename Then, typename Else>
    struct IF<true, Then, Else>
    {
        typedef Then type;
    };

    template <typename Then, typename Else>
    struct IF<false, Then, Else>
    {
        typedef Else type;
    };
}

namespace cl
{
    template <typename Type
        , int DefaultValue
        , typename HasValue = cl::has_value<Type>::type >
    struct take_value
    {
        static const int value = DefaultValue;
    };

    template <typename Type
        , int DefaultValue>
    struct take_value<Type, DefaultValue, std::true_type >
    {
        static const int value = Type::value;
    };
}

namespace boost { namespace lambda { namespace detail
{
    // foreign declaration of promote code
    template <typename > struct promote_code;

    //!! Translates to native type (currently may cause a runtime error, to be fixed)
    template <>
    struct promote_code<cl::TapeDouble >
    {
        static const int value = cl::take_value<
                                        boost::lambda::detail::promote_code
                                        <
                                            typename cl::remove_ad<typename cl::TapeDouble::value_type>::type
                                        >
                                        , 600 //!!! See boost promote_code for double, avoid hardcode
                                   >::value;
    };
}}}

namespace boost {  namespace detail
{
    template<class B>
    struct return_type_2_arithmetic_phase_3;
    template<class> class return_type_deduction_failure;

    template<>
    struct return_type_2_arithmetic_phase_3 <cl::TapeDouble >
    {
        typedef cl::TapeDouble ad_type;
        typedef ad_type A;

        return_type_2_arithmetic_phase_3(ad_type const& ad_v) {   }
    };
}}

namespace boost { namespace numeric { namespace ublas
{
    template<class M>
    class sparse_matrix_element;

    template<class T, class L, std::size_t IB, class IA, class TA>
    class compressed_matrix;

    template<class T, class ALLOC>
    class unbounded_array;

    namespace detail {

    template <typename Left>
    inline bool operator < (Left const& left, cl::TapeDouble const& right)
    {
#if defined CL_COMPILE_TIME_DEBUG
#pragma message ("overload operator < : " __FUNCSIG__)
#endif
        return (cl::TapeDouble)left < right;
    }

    // This is hooked operator <
    template <typename DoubleType>
    inline bool operator <
            (boost::numeric::ublas::matrix_scalar_unary<
               boost::numeric::ublas::matrix_binary<
                    boost::numeric::ublas::matrix_matrix_binary<
                        boost::numeric::ublas::triangular_adaptor<
                            boost::numeric::ublas::matrix<
                                cl::TapeDouble
                                , boost::numeric::ublas::basic_row_major<unsigned int, int>
                                , boost::numeric::ublas::unbounded_array<
                                        DoubleType
                                        , std::allocator<cl::TapeDouble>
                                  >
                            >
                            , boost::numeric::ublas::basic_unit_lower<unsigned int>
                        >
                        , boost::numeric::ublas::triangular_adaptor<
                             boost::numeric::ublas::matrix<
                                cl::TapeDouble
                                , boost::numeric::ublas::basic_row_major<unsigned int, int>
                                , boost::numeric::ublas::unbounded_array<
                                      cl::TapeDouble
                                      , std::allocator<cl::TapeDouble>
                                  > >, struct boost::numeric::ublas::basic_upper<unsigned int>
                          >
                        , boost::numeric::ublas::matrix_matrix_prod<
                                boost::numeric::ublas::triangular_adaptor<
                                    boost::numeric::ublas::matrix<
                                        cl::TapeDouble
                                        , boost::numeric::ublas::basic_row_major<unsigned int, int>
                                        , class boost::numeric::ublas::unbounded_array<class cl::TapeDouble, class std::allocator<class cl::TapeDouble> >
                                    >
                                    , boost::numeric::ublas::basic_unit_lower<unsigned int>
                                >
                                , boost::numeric::ublas::triangular_adaptor<
                                    class boost::numeric::ublas::matrix<
                                        class cl::TapeDouble
                                        , struct boost::numeric::ublas::basic_row_major<unsigned int, int>
                                        , class boost::numeric::ublas::unbounded_array<class cl::TapeDouble, class std::allocator<class cl::TapeDouble> >
                                    >, struct boost::numeric::ublas::basic_upper<unsigned int>
                                  >
                                , cl::TapeDouble
                            >
                    >
                    , boost::numeric::ublas::matrix<
                        cl::TapeDouble, struct boost::numeric::ublas::basic_row_major<unsigned int, int>, class boost::numeric::ublas::unbounded_array<class cl::TapeDouble, class std::allocator<class cl::TapeDouble> >
                      >
                    , boost::numeric::ublas::scalar_minus<
                            cl::TapeDouble, class cl::TapeDouble
                        >
                >
                , boost::numeric::ublas::matrix_norm_inf<
                        boost::numeric::ublas::matrix_binary<
                            boost::numeric::ublas::matrix_matrix_binary<
                                boost::numeric::ublas::triangular_adaptor<
                                        boost::numeric::ublas::matrix<
                                            cl::TapeDouble
                                            , boost::numeric::ublas::basic_row_major<unsigned int, int>
                                            , boost::numeric::ublas::unbounded_array<cl::TapeDouble, std::allocator<cl::TapeDouble> >
                                        >
                                    , struct boost::numeric::ublas::basic_unit_lower<unsigned int>
                                >
                                , boost::numeric::ublas::triangular_adaptor<
                                        boost::numeric::ublas::matrix<
                                            cl::TapeDouble
                                            , boost::numeric::ublas::basic_row_major<unsigned int, int>
                                            , boost::numeric::ublas::unbounded_array<cl::TapeDouble, std::allocator<class cl::TapeDouble> >
                                        >
                                        , struct boost::numeric::ublas::basic_upper<unsigned int>
                                  >
                                  , boost::numeric::ublas::matrix_matrix_prod<
                                        boost::numeric::ublas::triangular_adaptor<
                                            boost::numeric::ublas::matrix<
                                                cl::TapeDouble
                                                , boost::numeric::ublas::basic_row_major<unsigned int, int>
                                                , boost::numeric::ublas::unbounded_array<cl::TapeDouble, std::allocator<class cl::TapeDouble> >
                                            >
                                            , boost::numeric::ublas::basic_unit_lower<unsigned int>
                                        >
                                        , boost::numeric::ublas::triangular_adaptor<
                                            boost::numeric::ublas::matrix<
                                                cl::TapeDouble
                                                , boost::numeric::ublas::basic_row_major<unsigned int, int>
                                                , boost::numeric::ublas::unbounded_array<cl::TapeDouble, std::allocator<cl::TapeDouble> >
                                            >
                                            , boost::numeric::ublas::basic_upper<unsigned int>
                                        >
                                        , class cl::TapeDouble
                                  >
                            >
                            , boost::numeric::ublas::matrix<
                                    cl::TapeDouble
                                    , boost::numeric::ublas::basic_row_major<unsigned int, int>
                                    , boost::numeric::ublas::unbounded_array<cl::TapeDouble, std::allocator<cl::TapeDouble> >
                              >
                            , boost::numeric::ublas::scalar_minus<cl::TapeDouble, cl::TapeDouble>
                        >
                >
           > const& left
        , cl::TapeDouble const& right)
    {
#if defined CL_COMPILE_TIME_DEBUG
#pragma message ("overload operator < : " __FUNCSIG__)
#endif
        return (cl::TapeDouble)left < right;
    }
}
}}}

namespace boost
{
    namespace detail {
        template <typename Type> struct is_arithmetic_impl;

        template< >
        struct is_arithmetic_impl<cl::TapeDouble>
        {
            typedef cl::remove_ad<cl::TapeDouble::value_type>::type value_type;
#if defined BOOST_STATIC_CONSTANT
            BOOST_STATIC_CONSTANT(bool, value =
                (::boost::type_traits::ice_or<
                    ::boost::is_integral<value_type>::value,
                    ::boost::is_float<value_type>::value
                >::value));
#else
            static const bool value = std::is_arithmetic<value_type>::value;
#endif
            typedef std::is_arithmetic<value_type>::type type;
        };
    }
}

#endif  // cl_tape_impl_doubleoperators_hpp
