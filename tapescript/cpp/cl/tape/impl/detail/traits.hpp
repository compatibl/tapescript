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

namespace cl
{
    template <class Ty_>
    using ref_wrap = std::reference_wrapper<Ty_>;

    template <class Value, class Mirror, class Bits = uint64_t>
    union double_value;

    template <class T>
    struct base_type_traits
    {
        enum { is_union_able = false };
        typedef double type;

#       ifdef CL_TAPE_CPPAD
        typedef CppAD::AD<T> value_type;
#       elif defined CL_TAPE_ADOLC
        typedef adouble value_type;
#       else
        typedef double value_type;
#       endif

        template <class Value, class Mirror, class Bits = uint64_t>
        struct double_value_type
        {
            typedef CppAD::AD<Value> type;
            typedef CppAD::AD<Value> tdouble_type;
            typedef CppAD::AD<Value> double_type;
        };
    };

    template <>
    struct base_type_traits<double>
    {
        enum { is_union_able = true };
        typedef double type;
#       ifdef CL_TAPE_CPPAD
        typedef CppAD::AD<double> value_type;
#       elif defined CL_TAPE_ADOLC
        typedef adouble value_type;
#       else
        typedef double value_type;
#       endif
        template <class Value, class Mirror, class Bits = uint64_t>
        struct double_value_type
        {
            typedef double_value<Value, Mirror, Bits> type;
            typedef CppAD::AD<Value> tdouble_type;
            typedef double double_type;
        };
    };

    // This class is pressure to instanciate operator
    // on transation line with impl and without
    template <class T
        , int IsImpl /*= cl::is_implemented<cl::compatibl_ad_enabled>::value*/>
    struct argument_adapter
    {
        argument_adapter(T const& v) : v_(v)
        {}

        T const v_;

        template <class Type>
        static inline Type const&
        get(Type const& v)
        {
            return v;
        }

        template <class Type>
        static inline Type const&
        get(argument_adapter<Type > const& v)
        {
            return v.v_;
        }
    };

    template <class T>
    struct argument_adapter<T, 0>
    {
        argument_adapter(T const& v) : v_(v)
        {}

        T const& v_;

        template <class Type>
        static inline Type const&
        get(Type const& v)
        {
            return v;
        }

        template <class Type>
        static inline Type const&
        get(argument_adapter<Type > const& v)
        {
            return v.v_;
        }
    };

}