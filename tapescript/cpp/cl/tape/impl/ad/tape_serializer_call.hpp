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

This file includes code from CppAD, a C++ algorithmic differentiation library
distributed under multiple licenses. This distribution is under the terms of
the Eclipse Public License Version 1.0, a copy of which is available at:

https://www.eclipse.org/legal/epl-v10.html

CppAD code included in this file is subject to copyright:

Copyright (C) 2003-15 Bradley M. Bell
*/

#ifndef cl_tape_impl_ad_tape_serializer_call_hpp
#define cl_tape_impl_ad_tape_serializer_call_hpp

#if defined _MSC_VER
    __if_not_exists(CppAD::serializers_list)
{
    namespace boost {
        namespace archive {
            class binary_oarchive;
            class binary_iarchive;
        }
    }

    namespace CppAD
    {
        template <class... > struct serializers_list;

        template <template <typename, typename> class Serializer, typename Archive>
        struct ho_serializer2;

        template <typename >
        struct serializer_traits;
    }
}
#elif __GNUC__
    namespace boost {
        namespace archive {
            class binary_oarchive;
            class binary_iarchive;
        }
    }

    namespace CppAD
    {
        template <class... > struct serializers_list;

        template <template <typename, typename> class Serializer, typename Archive>
        struct ho_serializer2;

        template <typename >
        struct serializer_traits;
    }

#endif

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

    template <typename Serializer, typename Stream, typename Base, typename... Args>
    inline void serialize__
        (std::true_type
        , Stream                   &archiver
        , /*const */bool            print
        , /*const */size_t          p
        , /*const */size_t          q
        , /*const */size_t          n
        , /*const */size_t          numvar
        , player<Base>            *&play
        , /*const */size_t          J
        , Base                    *&taylor
        , bool                    *&cskip_op
        , pod_vector<addr_t>       &var_by_load_op          // result
        , size_t                   &compare_change_count
        , size_t                   &compare_change_number   // result
        , size_t                   &compare_change_op_index // result
        , Args&...                  args)
    {
#       pragma message ("Not implemented " __FUNCSIG__)
    }


    template <typename Serializer, typename Stream, typename Base, typename... Rest>
    inline void io_ss__(Stream &archiver, bool&, size_t&, size_t&, size_t&, size_t&, player<Base>*&, size_t&
        , Base*&, bool*&, pod_vector<addr_t>&, size_t&, size_t&, size_t&, CppAD::serializers_list<Rest...>)
    {
	throw "Not serializable.";
    }

    template <typename Serializer, typename Stream, typename Base, typename S, typename... Rest>
    inline void io_ss__
        (Stream                   &archiver
        , /*const */bool           &print
        , /*const */size_t         &p
        , /*const */size_t         &q
        , /*const */size_t         &n
        , /*const */size_t         &numvar
        , player<Base>            *&play
        , /*const */size_t         &J
        , Base                    *&taylor
        , bool                    *&cskip_op
        , pod_vector<addr_t>       &var_by_load_op          // result
        , size_t                   &compare_change_count
        , size_t                   &compare_change_number   // result
        , size_t                   &compare_change_op_index // result
        , CppAD::serializers_list<S, Rest...>)
    {
        typedef typename
            S::template apply<Base>::type io_archive;

        //Serializer& sa = static_cast<Serializer& >(archiver);
        if (typeid(io_archive) == typeid(*&archiver))
        {
            io_archive& ar = static_cast<io_archive&>(archiver);
                   ar & print
                      & p
                      & q
                      & n
                      & numvar
                      & (*play)
                      //& J
                      //& taylor //TODO: make_array_archiver(taylor, J)
                      & cskip_op
                      & var_by_load_op
                      & compare_change_count;
            return ;
        }

        io_ss__<Serializer>(
            archiver
            , print
            , p
            , q
            , n
            , numvar
            , play
            , J
            , taylor
            , cskip_op
            , var_by_load_op
            , compare_change_count
            , compare_change_number
            , compare_change_op_index, CppAD::serializers_list<Rest...>());
    }

    template <typename Serializer, typename Stream, typename Base, typename... Args>
    inline void serialize__
        (std::true_type
        , Stream                   &archiver
        , /*const */bool           &print
        , /*const */size_t         &p
        , /*const */size_t         &q
        , /*const */size_t         &n
        , /*const */size_t         &numvar
        , player<Base>            *&play
        , /*const */size_t         &J
        , Base                    *&taylor
        , bool                    *&cskip_op
        , pod_vector<addr_t>       &var_by_load_op          // result
        , size_t                   &compare_change_count
        , size_t                   &compare_change_number   // result
        , size_t                   &compare_change_op_index // result
        , typename
                CppAD::serializer_traits<Base>::certain_archivers* ptr
        , Args&...                  args)
    {
        typedef typename
            CppAD::serializer_traits<Base>::certain_archivers serializers_list;

        // Find and make io operation from certain arhive
        io_ss__<Serializer>(
            archiver
            , print
            , p
            , q
            , n
            , numvar
            , play
            , J
            , taylor
            , cskip_op
            , var_by_load_op
            , compare_change_count
            , compare_change_number
            , compare_change_op_index, serializers_list());

#       pragma message ("recursive - " __FUNCSIG__)
    }



    /// <summary>Implementation serialization call
    /// if we have implemented serializer.</summary>
    template <typename Serializer, typename Stream, typename Base, typename... Args>
    inline void serialize__
        (std::true_type
        , Stream                             &s_out
        , CppAD::vector<size_t>              &user_iy
        , CppAD::player<Base>*                play
        , Base                               *taylor
        , enum CppAD::OpCode                  op
        , int                                 user_state
        , unsigned long long                  i_op
        , unsigned long long                  q
        , size_t                              user_m
        , const size_t                        J
        , const CppAD::addr_t                *arg
        , size_t                              i_var
        , Args&...                            args)
    {
        CPPAD_ASSERT_UNKNOWN(&s_out != &std::cout);

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
                ss << std::endl;
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

namespace cl
{
    namespace tapescript
    {
#if defined _MSC_VER
        __if_not_exists(serialize_accessor)
        {
            template <class Base>
            class serialize_accessor
            {
            public:
                serialize_accessor()
                {}

                template <class Vector>
                serialize_accessor(Vector vc) //: acc_(vc)
                {
                }
            };
        }
#elif __GNUC__
            template <class Base>
            class serialize_accessor
            {
            public:
                serialize_accessor()
                {}

                template <class Vector>
                serialize_accessor(Vector vc) //: acc_(vc)
                {
                }
            };

#endif        
    } // namespace tapescript

} // namespace cl

#endif // cl_tape_impl_ad_tape_serializer_call_hpp
