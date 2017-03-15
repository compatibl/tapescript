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

# if !defined cl_tape_impl_detail_double_type_value_hpp
# define cl_tape_impl_detail_double_type_value_hpp

#   if defined CL_CHECK_INSTANCE_COUNTER
#   include <map>
#   endif

namespace cl
{

    struct mask
    {
        static const uint64_t
            max_double = 0xFFF8000000000000
            , ptr_mask64 = 0xFFFA000000000000
            , double_mask32 = 0xFFF80000
            , ptr_mask32 = 0xFFFA0000
            , double_mask16 = 0xFFF8
            , ptr_mask16 = 0xFFFA;
    };

    template <class W
        , bool Is_union_able>
    struct double_traits_base
    {
        template <class Request, class Value>
        inline static Request&
        tdouble_(Value& value)
        {
            return *reinterpret_cast<Request *>
                (value.bits_ & ~mask::ptr_mask64);
        }

        template <class Request, class Value>
        inline static Request&
        double_(Value& value)
        {
            return (Request&)value.dbl_;
        }
    };

    template <class W>
    struct double_traits_base<W, false>
    {
        template <class Request, class Value>
        inline static Request&
        tdouble_(Value& value)
        {
            return value;
        }

        template <class Request, class Value>
        inline static Request&
        double_(Value& value)
        {
            return value;
        }
    };

    template <class W
         , bool Is_ad = cl::is_implemented<compatibl_ad_enabled>::value
         , bool Is_union_able = std::is_arithmetic<typename W::base_type>::value>
    struct double_value_traits : double_traits_base<W, Is_union_able>
    {
        enum { is_union_able = Is_union_able };

        template <class T>
        inline static bool
        is_ptr(T const& this_)
        {
            typedef typename
            W::double_value_type Switch;

            return (this_.value_.template bits2<3>() & mask::ptr_mask16)
                == mask::ptr_mask16;
        }

        template <class T>
        inline static char
        is_ptr(T const& l, T const& r)
        {
            typedef typename
                W::double_value_type Switch;

            return ((l.value_.template bits2<3>() & mask::ptr_mask16) == mask::ptr_mask16) ||
                ((r.value_.template bits2<3>() & mask::ptr_mask16) == mask::ptr_mask16);
        }

        template <typename T> inline static void free(T& v)
        {
            T::free(v);
        }
    };

    template <class W>
    struct double_value_traits<W, false, true> : double_traits_base<W, true>
    {
        enum { is_union_able = true };

        template <class T>
        inline static bool
        is_ptr(T const&)
        {
            return false;
        }

        template <class T>
        inline static char
        is_ptr(T const&, T const&)
        {
            return false;
        }

        template <typename T> inline static void free(T) {}
    };

    template <class W, bool Is_ad_impl>
    struct double_value_traits<W, Is_ad_impl, false>
    {
        enum { is_union_able = false };

        template <class T>
        inline static bool is_ptr(T const&)
        { return true; }

        template <class T>
        inline static char
        is_ptr(T const&, T const&)
        {
            return true;
        }

        template <typename T> inline static void free(T) {}

        template <class Request, class Value>
        inline static Request& tdouble_(Value& w) { return w; }

        template <class Request, class Value>
        inline static Request& double_(Value& w) { return w; }
    };

    template <class Value, class Mirror, class Bits/* = uint64_t*/>
    union double_value
    {
        typedef typename
        std::remove_pointer<Mirror>::type mirror_type;

        double_value()
            : dbl_(0.0)
        {
        }

        double_value(Value d)
            : dbl_(d)
        {}

        double_value(Bits b)
            : bits_(b)
        {}

        double_value(mirror_type const& v)
            : bits_(alloc_(v))
        {}

        double_value(double_value const& v) : bits_(v.bits_)
        {}

        inline static void free(double_value& v)
        {
            mirror_type* ptr = reinterpret_cast<mirror_type*>(v.bits_ & ~mask::ptr_mask64);

#           if defined CL_CHECK_INSTANCE_COUNTER
                std::map<void*, bool >& m = ll();
                CL_ASSERT(m[ptr], "Shouldn't be zero.");
                m[ptr] = false;
#           endif
            delete ptr;
        }

        template <typename... Args>
        inline static Bits
        alloc_(Args const&... args)
        {
            mirror_type* ptr = new mirror_type(args...);

#           if defined CL_CHECK_INSTANCE_COUNTER
                std::map<void*, bool >& m = ll();
                CL_ASSERT(!m[ptr], "Should be zero.");
                m[ptr] = true;
#           endif

            return (reinterpret_cast<uint64_t>(ptr)
                | mask::ptr_mask64);
        }

#       if defined CL_CHECK_INSTANCE_COUNTER
        static std::map<void*, bool >& ll() { static std::map<void*, bool > mem; return mem; }
#       endif

        Value dbl_;
        Mirror tdbl_;
        Bits bits_;

        uint32_t uibits_[2];
        unsigned short shbits_[4];

        template <int N>
        uint32_t const&
        bits4() const
        {
            static_assert(N >= 0 && N<sizeof(uibits_), "Out of range.");
            return uibits_[N];
        }

        template <int N>
        unsigned short const&
        bits2() const
        {
            static_assert(N >= 0 && N<sizeof(shbits_), "Out of range.");
            return shbits_[N];
        }
    };

    template <typename Stream, typename... Types>
    inline Stream&
    operator << (Stream& s, cl::double_value<Types...> const& v)
    {
        s << (void*)v.bits_;
        return s;
    }
}

# endif
