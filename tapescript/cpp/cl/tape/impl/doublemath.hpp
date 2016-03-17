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

#ifndef cl_tape_impl_doublemath_hpp
#define cl_tape_impl_doublemath_hpp

# include <cl/tape/impl/detail/doublemath_impl.hpp>
# include <cl/tape/impl/detail/oper_def.hpp>

/// <summary>Provides math functions for tape_wrapper.</summary>
namespace std
{
    //MATH_FUNC_1(exp);
    MATH_FUNC_1(cl::tape_wrapper<Base>, abs);
    MATH_FUNC_1(cl::tape_wrapper<Base>, fabs);
    MATH_FUNC_1(cl::tape_wrapper<Base>, floor);
    MATH_FUNC_1(cl::tape_wrapper<Base>, ceil );
    MATH_FUNC_1(cl::tape_wrapper<Base>, round);
    MATH_FUNC_1(cl::tape_wrapper<Base>, trunc);
    MATH_FUNC_1(cl::tape_wrapper<Base>, sqrt );
    MATH_FUNC_1(cl::tape_wrapper<Base>, log);
    MATH_FUNC_1(cl::tape_wrapper<Base>, sin);
    MATH_FUNC_1(cl::tape_wrapper<Base>, cos);
    MATH_FUNC_1(cl::tape_wrapper<Base>, tan);
    MATH_FUNC_1(cl::tape_wrapper<Base>, asin);
    MATH_FUNC_1(cl::tape_wrapper<Base>, acos);
    MATH_FUNC_1(cl::tape_wrapper<Base>, atan);
    MATH_FUNC_1(cl::tape_wrapper<Base>, sinh);
    MATH_FUNC_1(cl::tape_wrapper<Base>, cosh);
    MATH_FUNC_1(cl::tape_wrapper<Base>, tanh);
    MATH_FUNC_1(cl::tape_wrapper<Base>, erf);
    MATH_FUNC_1(cl::tape_wrapper<Base>, log10);
    MATH_FUNC_1(cl::tape_wrapper<Base>, asinh);
    MATH_FUNC_1(cl::tape_wrapper<Base>, acosh);
    MATH_FUNC_1(cl::tape_wrapper<Base>, atanh);
    MATH_FUNC_1(bool, isnan);

#   define CL_USE_UNION

# if defined CL_USE_UNION
#   define MATH_FUNC pow__skip
# else
#   define MATH_FUNC pow__skip
#endif

#   define rtype cl::tape_wrapper<Base>

    template <class Base>
    inline rtype
    MATH_FUNC(cl::tape_wrapper<Base> const& l
            , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value
            , cl::tape_wrapper<Base>, cl::argument_adapter<cl::tape_wrapper<Base>>
        >::type const& rv, typename std::enable_if<!std::is_arithmetic<Base>::value>::type* p = 0)
    {
        cl::tape_wrapper<Base> const& r
            = cl::argument_adapter<cl::tape_wrapper<Base>>::get(rv);

        return CL_MATH_QUAL MATH_FUNC(l, r);
    }

    template <class Base>
    inline rtype
    MATH_FUNC (cl::tape_wrapper<Base> const& l
            , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value
                    , cl::tape_wrapper<Base>, cl::argument_adapter<cl::tape_wrapper<Base>>
         >::type const& rv, typename std::enable_if<std::is_arithmetic<Base>::value>::type* p = 0)
    {
        typedef typename cl::tape_wrapper<Base>::value_type value_type;

        cl::tape_wrapper<Base> const& r
            = cl::argument_adapter<cl::tape_wrapper<Base>>::get(rv);

        typedef cl::double_value_traits<cl::tape_wrapper<Base>
            , cl::is_implemented<cl::compatibl_ad_enabled >::value> traits_type;

        if (!traits_type::is_ptr(l, r))
        {
            return STD MATH_FUNC (l.double_(), r.double_());
        }

        char C = ((char)traits_type::is_ptr(l) << 1) | (char)traits_type::is_ptr(r);

        struct oper
        {
            rtype
            op_1(cl::tape_wrapper<Base> const& this_, cl::tape_wrapper<Base> const& r)
            {
                return CL_MATH_QUAL MATH_FUNC(cl::tape_wrapper<Base>(value_type(this_.double_())), r);
            }

            rtype
            op_2(cl::tape_wrapper<Base> const &this_, cl::tape_wrapper<Base> const& r)
            {
                return CL_MATH_QUAL MATH_FUNC(this_, cl::tape_wrapper<Base>((value_type(r.double_()))));
            }

            rtype
            op_3(cl::tape_wrapper<Base> const &this_, cl::tape_wrapper<Base> const& r)
            {
                return CL_MATH_QUAL MATH_FUNC(this_, r);
            }
        };

        typedef rtype
            (oper::*func_ptr)(cl::tape_wrapper<Base> const&, cl::tape_wrapper<Base> const&);

        static func_ptr f[] = { 0, &oper::op_1, &oper::op_2, &oper::op_3 };

        return (oper().*f[C])(l, r);
    }

    template <typename Base>
    inline typename cl::tape_wrapper<Base>::tape_type
        MATH_FUNC(cl::tape_wrapper<Base> const& l
               , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value
                    , double
                    , cl::argument_adapter<double>
                 >::type const& rv)
    {
        return MATH_FUNC(l, cl::tape_wrapper<Base>::tape_type(
            cl::argument_adapter<double>::get(rv)));
    }

    template <typename Base>
    inline typename cl::tape_wrapper<Base>::tape_type
    MATH_FUNC(typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value
                    , double
                    , cl::argument_adapter<double>
                >::type const& lv, cl::tape_wrapper<Base> const& l)
    {
        return MATH_FUNC(cl::tape_wrapper<Base>::tape_type(cl::argument_adapter<double>::get(lv)), l);
    }

#   undef MATH_FUNC
#   undef rtype

#       define MATH_FUNC_2(rtype, MATH_FUNC)                                                                            \
        template <class Base>                                                                                           \
        inline rtype                                                                                                    \
        MATH_FUNC(cl::tape_wrapper<Base> const& l                                                                       \
                , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value                        \
                , cl::tape_wrapper<Base>, cl::argument_adapter<cl::tape_wrapper<Base>>                                  \
            >::type const& rv, typename std::enable_if<!std::is_arithmetic<Base>::value>::type* p = 0)                  \
        {                                                                                                               \
            cl::tape_wrapper<Base> const& r                                                                             \
                = cl::argument_adapter<cl::tape_wrapper<Base>>::get(rv);                                                \
                                                                                                                        \
            return CL_MATH_QUAL MATH_FUNC(l, r);                                                                        \
        }                                                                                                               \
                                                                                                                        \
        template <class Base>                                                                                           \
        inline rtype                                                                                                    \
        MATH_FUNC (cl::tape_wrapper<Base> const& l                                                                      \
                , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value                        \
                        , cl::tape_wrapper<Base>, cl::argument_adapter<cl::tape_wrapper<Base>>                          \
             >::type const& rv, typename std::enable_if<std::is_arithmetic<Base>::value>::type* p = 0)                  \
        {                                                                                                               \
            typedef typename cl::tape_wrapper<Base>::value_type value_type;                                             \
                                                                                                                        \
            cl::tape_wrapper<Base> const& r                                                                             \
                = cl::argument_adapter<cl::tape_wrapper<Base>>::get(rv);                                                \
                                                                                                                        \
            typedef cl::double_value_traits<cl::tape_wrapper<Base>                                                      \
                , cl::is_implemented<cl::compatibl_ad_enabled >::value> traits_type;                                    \
                                                                                                                        \
            if (!traits_type::is_ptr(l, r))                                                                             \
            {                                                                                                           \
                return STD MATH_FUNC (l.double_(), r.double_());                                                        \
            }                                                                                                           \
                                                                                                                        \
            char C = ((char)traits_type::is_ptr(l) << 1) | (char)traits_type::is_ptr(r);                                \
                                                                                                                        \
            struct oper                                                                                                 \
            {                                                                                                           \
                rtype                                                                                                   \
                op_1(cl::tape_wrapper<Base> const& this_, cl::tape_wrapper<Base> const& r)                              \
                {                                                                                                       \
                    return CL_MATH_QUAL MATH_FUNC(cl::tape_wrapper<Base>(value_type(this_.double_())), r);              \
                }                                                                                                       \
                                                                                                                        \
                rtype                                                                                                   \
                op_2(cl::tape_wrapper<Base> const &this_, cl::tape_wrapper<Base> const& r)                              \
                {                                                                                                       \
                    return CL_MATH_QUAL MATH_FUNC(this_, cl::tape_wrapper<Base>((value_type(r.double_()))));            \
                }                                                                                                       \
                                                                                                                        \
                rtype                                                                                                   \
                op_3(cl::tape_wrapper<Base> const &this_, cl::tape_wrapper<Base> const& r)                              \
                {                                                                                                       \
                    return CL_MATH_QUAL MATH_FUNC(this_, r);                                                            \
                }                                                                                                       \
            };                                                                                                          \
                                                                                                                        \
            typedef rtype                                                                                               \
                (oper::*func_ptr)(cl::tape_wrapper<Base> const&, cl::tape_wrapper<Base> const&);                        \
                                                                                                                        \
            static func_ptr f[] = { 0, &oper::op_1, &oper::op_2, &oper::op_3 };                                         \
                                                                                                                        \
            return (oper().*f[C])(l, r);                                                                                \
        }                                                                                                               \
                                                                                                                        \
        template <typename Base>                                                                                        \
        inline typename cl::tape_wrapper<Base>::tape_type                                                               \
            MATH_FUNC(cl::tape_wrapper<Base> const& l                                                                   \
                   , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value                     \
                        , double                                                                                        \
                        , cl::argument_adapter<double>                                                                  \
                     >::type const& rv)                                                                                 \
        {                                                                                                               \
            return MATH_FUNC(l, cl::tape_wrapper<Base>::tape_type(                                                      \
                cl::argument_adapter<double>::get(rv)));                                                                \
        }                                                                                                               \
                                                                                                                        \
        template <typename Base>                                                                                        \
        inline typename cl::tape_wrapper<Base>::tape_type                                                               \
        MATH_FUNC(typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value                        \
                        , double                                                                                        \
                        , cl::argument_adapter<double>                                                                  \
                    >::type const& lv, cl::tape_wrapper<Base> const& l)                                                 \
        {                                                                                                               \
            return MATH_FUNC(cl::tape_wrapper<Base>::tape_type(cl::argument_adapter<double>::get(lv)), l);              \
        }

# if defined CL_USE_UNION

        MATH_FUNC_2(cl::tape_wrapper<Base>, atan2);
        MATH_FUNC_2(cl::tape_wrapper<Base>, fmod );
        MATH_FUNC_2(cl::tape_wrapper<Base>, modf);
        MATH_FUNC_2(cl::tape_wrapper<Base>, log);
        MATH_FUNC_2(cl::tape_wrapper<Base>, pow);

#  endif

        template <class Base, class Right>
        inline cl::tape_wrapper<Base>
        pow(cl::tape_wrapper<Base> left, Right right
            , typename std::enable_if<std::is_same<int
                        , typename std::remove_reference<
                                    typename std::remove_const<Right>::type
                             >::type>::value
                       >::type* p = 0)
        {
            return CL_MATH_QUAL pow_impl(left, right, std::is_arithmetic<Base>());
        }

        template <class Base>
        inline cl::tape_wrapper<Base>
        min(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
        {
            return cl::min_impl(x, y);
        }

        template <class Base>
        inline cl::tape_wrapper<Base>
        max(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
        {
            return cl::max_impl(x, y);
        }

        template <class Ty>
        inline typename cl::enable_if_tape_wrapper<Ty>::type
        min(Ty const& x, double y)
        {
            return cl::min_impl(x, y);
        }

        template <class Ty>
        inline typename cl::enable_if_tape_wrapper<Ty>::type
        min(double x, Ty const& y)
        {
            return cl::min_impl(x, y);
        }

        template <class Ty>
        inline typename cl::enable_if_tape_wrapper<Ty>::type
        max(Ty const& x, double y)
        {
            return cl::max_impl(x, y);
        }

        template <class Ty>
        inline typename cl::enable_if_tape_wrapper<Ty>::type
        max(double x, Ty const& y)
        {
            return cl::max_impl(x, y);
        }

# if !defined CL_USE_UNION
    template <class Base>
    inline cl::tape_wrapper<Base> pow(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
    {
        return CL_MATH_QUAL pow(x, y);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> pow(cl::tape_wrapper<Base> const& x, double y)
    {
        return CL_MATH_QUAL pow(x, y);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> pow(double x, cl::tape_wrapper<Base> const& y)
    {
        return CL_MATH_QUAL pow(x, y);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> pow(cl::tape_wrapper<Base> const& x, int y)
    {
        return CL_MATH_QUAL pow(x, y);
    }

    // atan2 implementation
    template <class Base>
    inline cl::tape_wrapper<Base> atan2(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
    {
        return CL_MATH_QUAL atan2(x, y);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> atan2(cl::tape_wrapper<Base> const& x, double y)
    {
        return CL_MATH_QUAL atan2(x, y);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> atan2(double x, cl::tape_wrapper<Base> const& y)
    {
        return CL_MATH_QUAL atan2(x, y);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> fmod(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base> const& y)
    {
        return CL_MATH_QUAL fmod(x, y);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> fmod(cl::tape_wrapper<Base> const& x, double y)
    {
        return CL_MATH_QUAL fmod(x, y);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> fmod(double x, cl::tape_wrapper<Base> const& y)
    {
        return CL_MATH_QUAL fmod(x, y);
    }

    template <class Base>
    inline bool isnan(cl::tape_wrapper<Base> const& x)
    {
        return CL_MATH_QUAL isnan(x);
    }

    template <class Base>
    inline cl::tape_wrapper<Base> modf(cl::tape_wrapper<Base> const& x, cl::tape_wrapper<Base>* iptr)
    {
        return CL_MATH_QUAL modf(x, y);
    }

#   endif

}

namespace cl
{
    MATH_FUNC_2(cl::tape_wrapper<Base>, log);

    namespace std_proxy
    {
        template <class T>
        T max_impl(const T& x, const T& y)
        {
            return std::max(x, y);
        }

        template <class T>
        T min_impl(const T& x, const T& y)
        {
            return std::min(x, y);
        }
    }

#pragma push_macro("STD")
#undef STD
#define STD cl::std_proxy::

    MATH_FUNC_2(cl::tape_wrapper<Base>, max_impl);
    MATH_FUNC_2(cl::tape_wrapper<Base>, min_impl);

#pragma pop_macro("STD")
}

#if defined CL_EXTERNAL_NAMESPACE && defined CL_USE_USING_STD
namespace CL_EXTERNAL_NAMESPACE
{
    using std::exp;
    using std::sqrt;
}
#endif

#endif  // cl_tape_impl_doublemath_hpp

