# ifndef TAPE_SERIALIZER_FWD_INCLUDED
# define TAPE_SERIALIZER_FWD_INCLUDED
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

namespace CppAD
{
    template <typename T>
    struct tape_serializer;

    template <typename Stream>
    inline bool is_cout(Stream& stg)
    {
        __if_exists(std::cout)
        {
            return (&std::cout == &stg);
        }

        return false;
    }

    template <typename Serializer, typename Stream, typename... Args>
    inline void serialize__(std::false_type, Stream& stg, Args...)
    {
# if defined CL_COMPILE_TIME_DEBUG_ENABLED
#   pragma message ("Empty serializer in: " __FUNCSIG__)
#   pragma message ("To implement serialize method plaese use:")

#   pragma message ("template <typename Serializer, typename Stream, typename... Args>")
#   pragma message ("inline void serialize__0"                              )
#   pragma message ("    (std::true_type"                                   )
#   pragma message ("     , Stream                             &s_out"      )
#   pragma message ("     , CppAD::vector<unsigned __int64>     user_iy"    )
#   pragma message ("     , CppAD::player<double>*              play"       )
#   pragma message ("     , double                             *taylor"     )
#   pragma message ("     , enum CppAD::OpCode                  op"         )
#   pragma message ("     , int                                 user_state" )
#   pragma message ("     , unsigned __int64                    i_op"       )
#   pragma message ("     , unsigned __int64                    q"          )
#   pragma message ("     , size_t                              user_m"     )
#   pragma message ("     , const size_t                        J"          )
#   pragma message ("     , const CppAD::addr_t                *arg"        )
#   pragma message ("     , size_t                              i_var"      )
#   pragma message ("     , Args&...                            args)"      )

# endif
    }

    template <typename Base>
    struct serializer_traits
    {
        typedef tape_serializer<Base> type;
    };

    template <typename Base, typename Stream, typename... Args>
    inline void serialize(Stream &stg, Args&... args)
    {
        typedef typename
            serializer_traits<Base>::type Serializer;

        serialize__<Serializer>(CppAD::is_implemented<Serializer>(), stg, args...);
    }
}

# endif