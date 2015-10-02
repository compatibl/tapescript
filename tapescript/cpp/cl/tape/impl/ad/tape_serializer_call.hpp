# ifndef TAPE_SERIALIZER_CALL_INCLUDED
# define TAPE_SERIALIZER_CALL_INCLUDED

/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

#include "tape_serializer_fwd.hpp"

namespace CppAD
{
    template <typename Ty_> struct remove_template
        : std::false_type {
        typedef Ty_ base;
    };

    template <template <class> class Template
        , class Ty_>
    struct remove_template<Template<Ty_> >
        : std::true_type{ typedef Ty_ base; };

    /// <summary>Serialization start implementation if impl_start is not
    /// defined in Serializer.</summary>
    template <typename Serializer, typename Meta = void>
    struct serializer_start_impl
    {
        template <typename... Args>
        static void start(Serializer&, Args&...)
        {}
    };

    /// <summary>Serialization start implementation if impl_start is
    /// defined in Serializer.</summary>
    template <typename Serializer>
    struct serializer_start_impl<Serializer
        , std::conditional_t<false, typename Serializer::impl_start, void>
    >
    {
        template <typename... Args>
        static void start(Serializer& ss, Args&... args)
        {
            ss.start(args...);
        }
    };

    /// <summary> Start serialization call. </summary>
    template <typename Serializer, typename Stream, typename... Args>
    inline void serialize__
        (std::true_type
        , Stream                             &s_out
        , serializer_start&
        , Args&...                            args)
    {
        Serializer& ss = static_cast<Serializer&>(s_out);
        serializer_start_impl<Serializer>::start(ss, args...);
    }

    /// <summary> Implementation serialization call
    /// if we have implemented serializer </summary>
    template <typename Serializer, typename Stream, typename Base, typename... Args>
    inline void serialize__
        (std::true_type
        , Stream                             &s_out
        , CppAD::vector<unsigned __int64>     user_iy
        , CppAD::player<Base>*                play
        , Base                               *taylor
        , enum CppAD::OpCode                  op
        , int                                 user_state
        , unsigned __int64                    i_op
        , unsigned __int64                    q
        , size_t                              user_m
        , const size_t                        J
        , const CppAD::addr_t                *arg
        , size_t                              i_var
        , Args&...                            args)
    {
        CPPAD_ASSERT_UNKNOWN(&s_out != &std::cout);

        typedef typename
            remove_template<Serializer>::base _Base_type;

        enum { user_start, user_arg, user_ret, user_end, user_trace };

        size_t i = 0;

        Serializer& ss = static_cast<Serializer&>(s_out);
        if (user_state == user_start && op == UserOp)
        {
            CPPAD_ASSERT_UNKNOWN(op == UserOp);
            CPPAD_ASSERT_UNKNOWN(NumArg(UsrrvOp) == 0);
            for (i = 0; i < user_m; i++) if (user_iy[i] > 0)
            {
                size_t i_tmp = (i_op + i) - user_m;
                ss.saveOp(
                    play,
                    i_tmp,
                    user_iy[i],
                    UsrrvOp,
                    CPPAD_NULL
                    );

                Base* Z_tmp = taylor + user_iy[i] * J;
                ss.saveOpResult(
                    q + 1,
                    Z_tmp,
                    0,
                    (Base *)CPPAD_NULL
                    );
            }
        }

        Base*           Z_tmp = taylor + J * i_var;
        const addr_t*   arg_tmp = arg;
        if (op == CSumOp)
            arg_tmp = arg - arg[-1] - 4;
        if (op == CSkipOp)
            arg_tmp = arg - arg[-1] - 7;
        if (op != UsrrvOp)
        {
            ss.saveOp(
                play,
                i_op,
                i_var,
                op,
                arg_tmp
                );
            if (NumRes(op) > 0) ss.saveOpResult(
                q + 1,
                Z_tmp,
                0,
                (Base *)CPPAD_NULL
                );
            ss << std::endl;
        }
    }
} // namespace CppAD

# endif
