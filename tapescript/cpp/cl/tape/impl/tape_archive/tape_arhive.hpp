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

# if !defined cl_tape_impl_tape_archive_tape_archive_hpp
# define cl_tape_impl_tape_archive_tape_archive_hpp

# define BOOST_SERIALIZATION_NO_LIB

# include <iostream>
# include <fstream>

# include <boost/archive/binary_oarchive.hpp>
# include <boost/archive/binary_iarchive.hpp>
# include <boost/archive/impl/basic_binary_oprimitive.ipp>
# include <boost/archive/impl/archive_serializer_map.ipp>
# include <boost/archive/impl/basic_binary_iarchive.ipp>
# include <boost/archive/impl/basic_binary_iprimitive.ipp>
# include <boost/archive/impl/basic_binary_oarchive.ipp>

# include <cppad/vector.hpp>
# include <cppad/local/pod_vector.hpp>

# include <cl/tape/impl/tape_fwd.hpp>

namespace cl
{
    template <typename >
    struct tape_inner;

    /// <summary>Tape function forward declaration.</summary>
    template <typename Base>
    class tape_function;

    template <typename Branch = struct tapearchive_tag_branch>
    struct traits { typedef struct {} type; };

    struct empty_text_tape_archive
    {
        template <typename... Args >
        void saveOpField(Args... fake)
        {}

        template <typename... Args >
        void saveOpResult(Args... fake)
        {}

        template <typename... Args >
        void saveOp(Args... fake)
        {}
    };

    template <typename Archive, typename Ty_>
    inline void extern_io(Archive &s, Ty_& v)
    {
# if !defined NDEBUG
#   pragma message ("Please implement serialization for: " __FUNCSIG__)
# endif
    }

    template <class Base>
    struct serializer : std::ostream
       , std::conditional < cl::is_implemented<traits<>::type>::value
                , typename traits<>::type
                , empty_text_tape_archive >::type
    {
        struct impl {};
        struct io_typed {};

        serializer()
            : std::ostream(0)
        {}


        template <class... Members, class Class, class Serializer>
        inline void io__(Class obj, Serializer& ss, std::false_type)
        {
#           if defined CL_COMPILE_TIME_DEBUG
#               pragma message (__FUNCSIG__)
#           endif
        }

        template <class Member, class... Members, class Class, class Serializer>
        inline void io__(Class obj, Serializer& ss, std::true_type)
        {
#           if defined CL_COMPILE_TIME_DEBUG
            cl::print_type<typename Member::type>();
#           endif

            ss(obj->*(Member()()));

#           if !defined NDEBUG
            debug_mem(Member(), *obj);
#           endif

            io__<Members...>(obj, ss, std::integral_constant<bool, (bool)sizeof...(Members)>());
        }

        template <template <class... > class Saver
            , class Class, class Serializer, class... Members>
            inline void io(Class cl, Serializer& ss, Saver<Members...> s)
        {
            io__<Members...>(cl, ss, std::integral_constant<bool, (bool)sizeof...(Members)>());
        }

        virtual unsigned int io_type()
        {
            return 0;
        }
    };

    template <class Archive>
    struct stream_type_trait;

    template <>
    struct stream_type_trait<boost::archive::binary_iarchive>
    {
        enum { is_out = 0 };
        typedef std::ifstream type;
    };

    template <>
    struct stream_type_trait<boost::archive::binary_oarchive>
    {
        enum { is_out = 1 };
        typedef std::ofstream type;
    };

    template <class Base
        , class Serializer>
    struct tape_archive : serializer<Base>
    {
        enum {
            io_type_value = cl::serializer_type::io_binary
            | cl::serializer_type::io_text
            , io_binary = cl::serializer_type::io_binary
        };

        template <typename... Args>
        tape_archive(std::string const& path
            , int archive_type = io_binary, Args&... args)
            : serializer<Base>()
            , os_(path)
            , ss_(os_)
            , archive_type_(archive_type)
        {}

        struct serialize__
        {
            template <typename Ty_>
            serialize__& io__(Ty_& v, std::true_type)
            {
                typedef typename
                    std::remove_const<Ty_>::type nc_type;

                nc_type& nc = const_cast<nc_type&>(v);
                ta_->ss_ & nc;

# if !defined NDEBUG && defined CL_TRACE_ENABLE
                std::cout << v << " ";
# endif
                return *this;
            }

            template <typename Ty_>
            serialize__& io__(Ty_& v, std::false_type)
            {
                typedef typename
                    class_fields<ho<Ty_>>::type class_fields_type;

                ta_->io(&v, *this, class_fields_type());
                return *this;
            }

            template <typename Ty_>
            serialize__& io(Ty_& v)
            {
                this->io__(v, std::is_pod<Ty_>());
                return *this;
            }

            template <typename Ty_>
            serialize__& operator ()(Ty_& v)
            {
                return this->io(v);
            }

            template <typename Ty_>
            serialize__& operator ()(Ty_* v)
            {
                this->operator ()(*const_cast<typename std::remove_const<Ty_>::type*>(v));
                return *this;
            }

            template <typename Ty_>
            serialize__& io_v(CppAD::pod_vector<Ty_> const& v, std::true_type)
            {
                size_t ix = v.size();
                this->io(ix);

                for (; --ix >= 0;)
                    this->operator ()(v[ix]);

                return *this;
            }

            template <typename Ty_>
            serialize__& io_v(CppAD::pod_vector<Ty_>& v, std::false_type)
            {
                size_t ix = 0;
                this->io(ix);
                if (ix > 0)
                {
                    v.extend(ix);

                    for (; --ix >= 0;)
                        this->operator ()(v[ix]);
                }

                return *this;
            }

            template <typename Ty_>
            serialize__& io_v(CppAD::vector<Ty_> const& v, std::true_type)
            {
                size_t ix = v.size();
                this->io(ix);

                for (; --ix >= 0;)
                    this->operator ()(v[ix]);

                return *this;
            }

            template <typename Ty_>
            serialize__& io_v(CppAD::vector<Ty_>& v, std::false_type)
            {
                int ix = 0;
                this->io(ix);
                if (ix > 0)
                {
                    v.resize(ix);

                    for (; --ix >= 0;)
                        this->operator ()(v[ix]);
                }

                return *this;
            }

            template <typename Ty_>
            serialize__& operator ()(CppAD::pod_vector<cl::tape_inner<Ty_> >& v)
            {
                extern_io(ta_->ss_, v);
                return *this;
            }

            template <typename Ty_>
            serialize__& operator ()(CppAD::pod_vector<cl::tape_inner<Ty_> > const& v)
            {
                extern_io(ta_->ss_, v);
                return *this;
            }

            template <typename Ty_>
            serialize__& operator ()(CppAD::pod_vector<Ty_ > const& v)
            {
                static_assert(std::is_pod<Ty_>::value
                    , "Is not pod type, please use overload semantic");

                return io_v(v, std::integral_constant<bool
                    , stream_type_trait<Serializer>::is_out>());
            }

            template <typename Ty_>
            serialize__& operator ()(CppAD::pod_vector<Ty_ >& v)
            {
                static_assert(std::is_pod<Ty_>::value
                     , "Is not pod type, please use overload semantic");

                return io_v(v, std::integral_constant<bool
                    , stream_type_trait<Serializer>::is_out>());
            }

            template <typename Ty_>
            serialize__& operator ()(CppAD::vector<Ty_ > const& v)
            {
                static_assert(std::is_pod<Ty_>::value
                    , "Is not pod type, please use overload semantic");

                return io_v(v, std::integral_constant<bool
                    , stream_type_trait<Serializer>::is_out>());
            }

            template <typename Ty_>
            serialize__& operator ()(CppAD::vector<Ty_ >& v)
            {
                static_assert(std::is_pod<Ty_>::value
                    , "Is not pod type, please use overload semantic");

                return io_v(v, std::integral_constant<bool
                    , stream_type_trait<Serializer>::is_out>());
            }

            tape_archive<Base, Serializer>* ta_;
        };

        template <typename Ty_>
        tape_archive<Base, Serializer>& io_(Ty_* v, std::true_type)
        {
            io_(*v, std::is_pod<Ty_>());
            return *this;
        }

        template <typename Ty_>
        tape_archive<Base, Serializer>& io_(Ty_& v, std::true_type)
        {
            ss_ & v;
# if !defined NDEBUG
            std::cout << v << " ";
# endif
            return *this;
        }

        template <typename Ty_>
        tape_archive<Base, Serializer>& io_(CppAD::pod_vector<Ty_>& v, std::false_type)
        {
            serialize__ ss_imp{ this };
            ss_imp(v);

            return *this;
        }

        template <typename Ty_>
        tape_archive<Base, Serializer>& io_(Ty_& v, std::false_type)
        {
            serialize__ ss_imp{ this };

            typedef typename
                class_fields<ho<Ty_>>::type class_fields_type;

            io(&v, ss_imp, class_fields_type());

            return *this;
        }

        template <typename Ty_>
        friend tape_archive<Base, Serializer>& operator
            & (tape_archive<Base, Serializer>& s, Ty_& v)
        {
            s.io_(v, std::is_pod<Ty_>());
            return s;
        }

        // type of seria
        virtual unsigned int io_type()
        {
            return (archive_type_);
        }

        template <typename > struct ho{};

        typename
            stream_type_trait<Serializer>::type os_;

        Serializer ss_;
        int        archive_type_;
    };
}


/// traits to implementation certain classes
namespace cl
{
    template <class Base>
    struct serializer;
}

namespace cl
{
    template <class Base
        , class Serializer>
    struct tape_archive;
}

namespace CppAD
{
    template <typename >
    class vector;

    template <class... > struct serializers_list
    {};

    template <template <typename, typename> class Serializer, typename Archive>
    struct ho_serializer2
    {
        template <typename Base>
        struct apply{ typedef Serializer<Base, Archive> type; };
    };

    template <typename >
    struct serializer_traits;

#   if defined CL_USE_ARCHIVE_IN_FORWARD
        template <>
        struct serializer_traits <double>
        {
            typedef cl::serializer<double> type;

            typedef serializers_list <
                    ho_serializer2<cl::tape_archive
                        , boost::archive::binary_iarchive>
                    , ho_serializer2<cl::tape_archive
                        , boost::archive::binary_oarchive>
                > certain_archivers;
        };
#   endif

    template <typename Base>
    struct serializer_traits <std::complex<Base>>
    {
        typedef cl::serializer<double> type;

        typedef serializers_list <
            ho_serializer2<cl::tape_archive
            , boost::archive::binary_iarchive>
            , ho_serializer2<cl::tape_archive
            , boost::archive::binary_oarchive>
        > certain_archivers;
    };

}

# endif
