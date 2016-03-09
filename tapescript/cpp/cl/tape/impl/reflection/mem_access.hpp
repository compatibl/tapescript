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

# if !defined cl_tape_impl_reflection_mem_access_hpp
# define cl_tape_impl_reflection_mem_access_hpp

namespace cl
{
    template <typename Mem_, typename Ty_>
    inline void debug_mem(Mem_, Ty_&);

    namespace tapescript
    {
        template <class Ty_>
        struct mem_accessor
        {
            enum { value = 0 };
            typedef struct is_not_implemented_mem_for_this_type { } mem_type;

            template <Ty_ V>
            struct spec
            {
                typedef mem_type type;
                static mem_type mem() { return V; }
                mem_type operator()() { return V; }
            };
        };

        template <class R_, class Ty_>
        struct mem_accessor<R_ Ty_::*>
        {
            enum { value = 1 };
            typedef R_ Ty_::*mem_type;

            template <mem_type V>
            struct spec
            {
                static mem_type mem() { return V; }
                typedef mem_type type;
                mem_type operator()()
                {
                    return V;
                }
            };
        };
    }

#   define decl_mem(m) typename cl::tapescript::mem_accessor<decltype(&m)>::template spec<&m>

    template <typename Mem_, typename Ty_>
    inline void debug_mem(Mem_, Ty_&)
    {
        /* Overload this function to debug member IO */
    }

    // This class is fields list we use for iterable fields
    template <typename... Ty_>
    struct fields
    {};

    // Class fields
    template <class Ty_>
    struct class_fields;
}


# endif  // cl_tape_impl_reflection_mem_access_hpp