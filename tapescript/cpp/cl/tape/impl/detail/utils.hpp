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

#ifndef cl_tape_impl_detail_utils_hpp
#define cl_tape_impl_detail_utils_hpp

# include <boost/filesystem.hpp>
# include <cl/tape/impl/detail/value_detail.hpp>

namespace cl
{
    inline void throw_(char const* what)
    {
        throw std::runtime_error(what);
    }

    template <class Ty>
    struct is_tape_wrapper
        : std::false_type
    {};

    template <class Inner>
    struct is_tape_wrapper<cl::tape_wrapper<Inner>>
        : std::true_type
    {};

    template <class Test, class Type = Test>
    struct enable_if_tape_wrapper
        : std::enable_if<is_tape_wrapper<Test>::value, Type>
    {};

    template <class It1, class It2, class Meta = void>
    struct tape_type_from_iter {};

    template <class It1, class It2>
    struct tape_type_from_iter<
        It1, It2
        , typename std::enable_if<
            is_tape_wrapper<
                typename std::iterator_traits<It1>::value_type
            >::value
            || is_tape_wrapper<
                typename std::iterator_traits<It2>::value_type
            >::value
        >::type
    >
    {
        typedef typename std::iterator_traits<It1>::value_type type1;
        typedef typename std::iterator_traits<It2>::value_type type2;
        typedef typename
            std::conditional<
                is_tape_wrapper<type1>::value
                , type1
                , type2
            >::type type;
    };

    template <class Inner>
    bool is_variable(tape_wrapper<Inner> const& v)
    {
        return ext::Variable(cl::tapescript::cvalue(v));
    }

    template <class Base>
    CppAD::AD<Base> get_ad_value(tape_wrapper<Base> const& x)
    {
        return x.is_ptr() ? x.value() : CppAD::AD<Base>(Base(x));
    }

    inline std::string get_current_module_path()
    {
        return boost::filesystem::current_path().generic_string();
    }
}

#endif  // cl_tape_impl_detail_utils_hpp
