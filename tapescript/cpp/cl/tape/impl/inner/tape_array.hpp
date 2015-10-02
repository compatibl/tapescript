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

#ifndef CL_TAPE_ARRAY_HPP_INCLUDED
#define CL_TAPE_ARRAY_HPP_INCLUDED

#pragma once

#include <cl/tape/impl/inner/base_inner_array.hpp>

namespace cl
{
    /// <summary>Class that implement specific tape_double functionality
    /// for inner_array template parameter.</summary>
    template <class Array>
    class tape_double_base<inner_array<Array>>
    {
        typedef tape_double<inner_array<Array>> tape_type;
        typedef inner_array<Array> base_type;
        typedef CppAD::AD<base_type> value_type;
        typedef typename base_type::scalar_type scalar_type;

    public:
        bool is_scalar() const
        {
            return value_type::UnaryBool(
                [](base_type const& inner){ return inner.is_scalar(); }
                , derived().value());
        }

        bool is_array() const
        {
            return value_type::UnaryBool(
                [](base_type const& inner){ return inner.is_array(); }
                , derived().value());
        }

    protected:
        typedef double constructor1_argument_type;
        typedef std::initializer_list<scalar_type> constructor2_argument_type;

        template <class Arg>
        static inline value_type convert_to_value_type(Arg arg)
        {
            return value_type(arg);
        }

    private:
        tape_type& derived() { return *static_cast<tape_type *>(this); }
        tape_type const& derived() const { return *static_cast<tape_type const*>(this); }
    };
}

#endif
