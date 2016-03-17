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
# define cl_tape_impl_inner_tape_inner_fields_hpp
# ifndef cl_tape_impl_inner_tape_inner_fields_hpp
# define cl_tape_impl_inner_tape_inner_fields_hpp

# include <cl/tape/impl/reflection/mem_access.hpp>
# include <cl/tape/impl/tape_fwd.hpp>

namespace cl
{
    template <template <class > class Ho_, class Array>
    struct class_fields<Ho_<tape_inner<Array>>>
    {
        typedef tape_inner<Array> Class;

        typedef fields <
            decl_mem(Class::mode_)
        > type;

        typedef void after;

        template <class Serializer>
        void after_io(Class& x, Serializer& ss)
        {
            if (x.is_scalar())
            {
                ss & x.scalar_value_;
            }
            else
            {
                ss & x.array_value_;
            }
        }
    };
}

#endif
