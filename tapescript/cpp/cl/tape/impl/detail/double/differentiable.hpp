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

# if !defined cl_tape_impl_detail_double_differentiable_hpp
# define cl_tape_impl_detail_double_differentiable_hpp

namespace cl
{
    // destructible able to differentiation class
    template <typename Ty_>
    struct differentiable
    {
        enum { enabled = true };

        ~differentiable();
    };

    // not destructible unable to differentiation class
    template <typename Ty_>
    struct not_differentiable
    {
        enum { enabled = false };
    };
}

# endif