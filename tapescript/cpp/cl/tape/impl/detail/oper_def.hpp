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

# if !defined cl_tape_impl_detail_oper_def_hpp
# define cl_tape_impl_detail_oper_def_hpp

#   if !defined NDEBUG
#       define CL_ENABLED_CHECK_DIFFERENTIABLE
#   endif

#   if !defined CL_ENABLED_CHECK_DIFFERENTIABLE
#       define CL_CHECK_ENABLED_DIFF(Base)
#   else
#       define CL_CHECK_ENABLED_DIFF(Base)              \
                cl::tape_wrapper<Base>                  \
            ::differentiation_check<struct Branch>();
#   endif


// namespace qualifier for an implementation.
# define CL_MATH_QUAL cl::tapescript::
# define STD std::

#   define OPBIN(rtype, OPERATOR)                                                                    \
    template <class Base>                                                                            \
    inline rtype                                                                                     \
    operator OPERATOR (cl::tape_wrapper<Base> const& l                                               \
            , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value         \
                    , cl::tape_wrapper<Base>, argument_adapter<cl::tape_wrapper<Base>>               \
                >::type const& rv)                                                                   \
    {                                                                                                \
        CL_CHECK_ENABLED_DIFF(Base);                                                                 \
        cl::tape_wrapper<Base> const& r                                                              \
            = argument_adapter<cl::tape_wrapper<Base>>::get(rv);                                     \
                                                                                                     \
        typedef double_value_traits<cl::tape_wrapper<Base>                                           \
            , cl::is_implemented<cl::compatibl_ad_enabled >::value> traits_type;                     \
                                                                                                     \
        if (!traits_type::is_ptr(l, r))                                                              \
        {                                                                                            \
            return (l.double_()) OPERATOR (r.double_());                                             \
        }                                                                                            \
                                                                                                     \
        char C = ((char)traits_type::is_ptr(l) << 1) | (char)traits_type::is_ptr(r);                 \
                                                                                                     \
        struct oper                                                                                  \
        {                                                                                            \
            typename rtype                                                                           \
            op_1(tape_wrapper<Base> const& this_, tape_wrapper<Base> const& r)                       \
            {                                                                                        \
                return this_.double_() OPERATOR r.tdouble_();                                        \
            }                                                                                        \
                                                                                                     \
            typename rtype                                                                           \
            op_2(tape_wrapper<Base> const &this_, tape_wrapper<Base> const& r)                       \
            {                                                                                        \
                return this_.tdouble_() OPERATOR r.double_();                                        \
            }                                                                                        \
                                                                                                     \
            rtype                                                                                    \
            op_3(tape_wrapper<Base> const &this_, tape_wrapper<Base> const& r)                       \
            {                                                                                        \
                return this_.tdouble_() OPERATOR r.tdouble_();                                       \
            }                                                                                        \
        };                                                                                           \
                                                                                                     \
        typedef rtype                                                                                \
            (oper::*func_ptr)(tape_wrapper<Base> const&, tape_wrapper<Base> const&);                 \
                                                                                                     \
        static func_ptr f[] = { 0, &oper::op_1, &oper::op_2, &oper::op_3 };                          \
                                                                                                     \
        return (oper().*f[C])(l, r);                                                                 \
    }                                                                                                \
                                                                                                     \
    template <typename Base>                                                                         \
    inline rtype                                                                                     \
    operator OPERATOR(tape_wrapper<Base> const& l                                                    \
               , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value      \
                    , double                                                                         \
                    , argument_adapter<double>                                                       \
                 >::type const& rv)                                                                  \
    {                                                                                                \
        return l OPERATOR tape_wrapper<Base>::tape_type(                                             \
            argument_adapter<double>::get(rv));                                                      \
    }                                                                                                \
                                                                                                     \
    template <typename Base>                                                                         \
    inline rtype                                                                                     \
    operator OPERATOR(typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value \
                    , double                                                                         \
                    , argument_adapter<double>                                                       \
                >::type const& lv, tape_wrapper<Base> const& l)                                      \
    {                                                                                                \
        return tape_wrapper<Base>::tape_type(argument_adapter<double>::get(lv)) OPERATOR l;          \
    }                                                                                                \


#   define ASSIGMENT_OPER(OPERATOR)                                                                  \
    template <typename Base>                                                                         \
    inline typename tape_wrapper<Base>::tape_type&                                                   \
    operator OPERATOR(tape_wrapper<Base>& l                                                          \
               , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value      \
                    , cl::tape_wrapper<Base>                                                         \
                    , argument_adapter<cl::tape_wrapper<Base>>                                       \
                 >::type const& rv)                                                                  \
    {                                                                                                \
        CL_CHECK_ENABLED_DIFF(Base);                                                                 \
                                                                                                     \
        cl::tape_wrapper<Base> const& r                                                              \
            = argument_adapter<cl::tape_wrapper<Base>>::get(rv);                                     \
                                                                                                     \
        typedef double_value_traits<cl::tape_wrapper<Base>                                           \
            , cl::is_implemented<cl::compatibl_ad_enabled >::value> traits_type;                     \
                                                                                                     \
        if (!traits_type::is_ptr(l, r))                                                              \
        {                                                                                            \
            l.double_() OPERATOR r.double_();                                                        \
            return l;                                                                                \
        }                                                                                            \
                                                                                                     \
        char C = ((char)traits_type::is_ptr(l) << 1) | (char)traits_type::is_ptr(r);                 \
                                                                                                     \
        struct oper                                                                                  \
        {                                                                                            \
            typename tape_wrapper<Base>::tape_type&                                                  \
            oper_1(tape_wrapper<Base>& this_, tape_wrapper<Base> const& r)                           \
            {                                                                                        \
                this_.alloc_(this_.double_());                                                       \
                this_.tdouble_() OPERATOR r.tdouble_();                                              \
                return this_;                                                                        \
            }                                                                                        \
                                                                                                     \
            typename tape_wrapper<Base>::tape_type&                                                  \
                oper_2(tape_wrapper<Base> &this_, tape_wrapper<Base> const& r)                       \
            {                                                                                        \
                this_.tdouble_() OPERATOR r.double_();                                               \
                return this_;                                                                        \
            }                                                                                        \
                                                                                                     \
            typename tape_wrapper<Base>::tape_type&                                                  \
                oper_3(tape_wrapper<Base> &this_, tape_wrapper<Base> const& r)                       \
            {                                                                                        \
                this_.tdouble_() OPERATOR r.tdouble_();                                              \
                return this_;                                                                        \
            }                                                                                        \
        };                                                                                           \
                                                                                                     \
        typedef tape_wrapper<Base>::tape_type&                                                       \
            (oper::*func_ptr)(tape_wrapper<Base>&, tape_wrapper<Base> const&);                       \
                                                                                                     \
        static func_ptr f[] = { 0, &oper::oper_1, &oper::oper_2, &oper::oper_3 };                    \
                                                                                                     \
        return (oper().*f[C])(l, r);                                                                 \
    }                                                                                                \
    template <typename Base>                                                                         \
    inline typename tape_wrapper<Base>::tape_type&                                                   \
    operator OPERATOR(tape_wrapper<Base>& l                                                          \
               , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value      \
                    , double                                                                         \
                    , argument_adapter<double>                                                       \
                 >::type const& rv)                                                                  \
    {                                                                                                \
        return l OPERATOR tape_wrapper<Base>::tape_type(                                             \
            argument_adapter<double>::get(rv));                                                      \
    }                                                                                                \
    template <typename Base>                                                                         \
    inline typename tape_wrapper<Base>::tape_type&                                                   \
    operator OPERATOR(typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value \
                    , double                                                                         \
                    , argument_adapter<double>                                                       \
                >::type const& lv, tape_wrapper<Base>& l)                                            \
    {                                                                                                \
        return argument_adapter<double>::get(lv) OPERATOR l;                                         \
    }                                                                                                \

#  define MATH_FUNC_1(RETURN_TYPE, MATH_FUNC)                                                        \
    template <class Base>                                                                            \
    inline RETURN_TYPE                                                                               \
    MATH_FUNC(cl::tape_wrapper<Base> const& x                                                        \
        , typename std::enable_if<std::is_arithmetic<Base>::value>::type* p = 0)                     \
    {                                                                                                \
        typedef cl::double_value_traits<cl::tape_wrapper<Base>                                       \
            , cl::is_implemented<cl::compatibl_ad_enabled>::value > traits_type;                     \
                                                                                                     \
        if (!traits_type::is_ptr(x))                                                                 \
        {                                                                                            \
            return STD MATH_FUNC(x.double_());                                                       \
        }                                                                                            \
                                                                                                     \
        return CL_MATH_QUAL MATH_FUNC(x);                                                            \
    }                                                                                                \
                                                                                                     \
    template <class Base>                                                                            \
    inline RETURN_TYPE                                                                               \
    MATH_FUNC(cl::tape_wrapper<Base> const& x                                                        \
        , typename std::enable_if<!std::is_arithmetic<Base>::value>::type* p = 0)                    \
    {                                                                                                \
        return CL_MATH_QUAL MATH_FUNC(x);                                                            \
    }

namespace std
{
#   define CL_DEBUG_FUNC exp

    template <class Base>
    inline cl::tape_wrapper<Base>
    CL_DEBUG_FUNC (cl::tape_wrapper<Base> const& x
        , typename std::enable_if<std::is_arithmetic<Base>::value>::type* p = 0)
    {
        typedef cl::double_value_traits<cl::tape_wrapper<Base>
            , cl::is_implemented<cl::compatibl_ad_enabled>::value > traits_type;

        if (!traits_type::is_ptr(x))
        {
            return STD CL_DEBUG_FUNC (x.double_());
        }

        return CL_MATH_QUAL CL_DEBUG_FUNC (x);
    }

    template <class Base>
    inline cl::tape_wrapper<Base>
    CL_DEBUG_FUNC (cl::tape_wrapper<Base> const& x
        , typename std::enable_if<!std::is_arithmetic<Base>::value>::type* p = 0)
    {
        return CL_MATH_QUAL CL_DEBUG_FUNC (x);
    }

}

# define CL_DEBUG_OPERATORS

# if defined CL_DEBUG_OPERATORS
namespace cl
{
    template <class T, int IsImpl = cl::is_implemented<cl::compatibl_ad_enabled>::value>
    struct argument_adapter;

# define OPERATOR /

    template <class Base>
    inline cl::tape_wrapper<Base>
    operator OPERATOR (cl::tape_wrapper<Base> const& l
        , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value
                , cl::tape_wrapper<Base>, argument_adapter<cl::tape_wrapper<Base>>
            >::type const& rv)
    {
        cl::tape_wrapper<Base> const& r
            = argument_adapter<cl::tape_wrapper<Base>>::get(rv);

        typedef double_value_traits<cl::tape_wrapper<Base>
            , cl::is_implemented<cl::compatibl_ad_enabled >::value> traits_type;

        if (!traits_type::is_ptr(l, r))
        {
            return (l.double_()) OPERATOR (r.double_());
        }

        char C = ((char)traits_type::is_ptr(l) << 1) | (char)traits_type::is_ptr(r);

        struct oper
        {
            typename tape_wrapper<Base>::tape_type
            op_1(tape_wrapper<Base> const& this_, tape_wrapper<Base> const& r)
            {
                return this_.double_() OPERATOR r.tdouble_();
            }

            typename tape_wrapper<Base>::tape_type
            op_2(tape_wrapper<Base> const &this_, tape_wrapper<Base> const& r)
            {
                return this_.tdouble_() OPERATOR r.double_();
            }

            typename tape_wrapper<Base>::tape_type
            op_3(tape_wrapper<Base> const &this_, tape_wrapper<Base> const& r)
            {
                return this_.tdouble_() OPERATOR r.tdouble_();
            }
        };

        typedef tape_wrapper<Base>::tape_type
            (oper::*func_ptr)(tape_wrapper<Base> const&, tape_wrapper<Base> const&);

        static func_ptr f[] = { 0, &oper::op_1, &oper::op_2, &oper::op_3 };

        return (oper().*f[C])(l, r);
    }

    template <typename Base>
    inline typename tape_wrapper<Base>::tape_type
    operator OPERATOR(tape_wrapper<Base> const& l
               , typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value
                    , double
                    , argument_adapter<double>
                 >::type const& rv)
    {
        return l OPERATOR tape_wrapper<Base>::tape_type(
            argument_adapter<double>::get(rv));
    }

    template <typename Base>
    inline typename tape_wrapper<Base>::tape_type
    operator OPERATOR(typename std::conditional<cl::is_implemented<cl::compatibl_ad_enabled >::value
                    , double
                    , argument_adapter<double>
                >::type const& lv, tape_wrapper<Base> const& l)
    {
        return tape_wrapper<Base>::tape_type(argument_adapter<double>::get(lv)) OPERATOR l;
    }
}
# undef OPERATOR
# endif

# endif //cl_tape_impl_detail_oper_def_hpp