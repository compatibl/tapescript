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

#ifndef cl_tape_examples_impl_utils_hpp
#define cl_tape_examples_impl_utils_hpp

#include <cl/tape/tape.hpp>

namespace cl
{
    template <class Ty>
    std::ostream& operator<<(std::ostream& ostr, std::vector<Ty> const& v)
    {
        if (v.size() == 0)
        {
            return ostr << "{}";
        }

        std::stringstream ss;
        ss.precision(ostr.precision());
        ss << "{ " << v[0];
        for (size_t i = 1; i < v.size(); i++)
        {
            ss << ", " << v[i];
        }
        ss << " }";

        return ostr << ss.str();
    }
}

#endif
