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

# ifndef cl_tape_impl_detail_value_detail_hpp
# define cl_tape_impl_detail_value_detail_hpp

# include <cl/tape/impl/tape_fwd.hpp>

namespace cl
{
    // TODO move it to the
    namespace tapescript
    {
        // Friendly for tape calculation
        // class functionality
        class tape_calculation_scope
        {
            template <typename Vector
            , typename Value>
            friend struct adapt_type_convention;

            template <typename TapeType>
            static inline typename
            TapeType::tdouble_type const&
            cvalue(TapeType const& tv)
            {
                static typename
                    TapeType::tdouble_type empty;

                // Return empty tdouble in case of the invalid pointer
                if (!tv.is_ptr())
                    return empty;

                return tv.tdouble_();
            }
        };

        /// Take value from tape double
        template <typename TapeType>
        inline typename
        TapeType::tdouble_type const&
        cvalue(TapeType const& tv)
        {
            CL_ASSERT(tv.is_ptr(), "Should be initialized pointer.");

            return tv.tdouble_();
        }

        /// Take const value from tape double
        template <typename TapeType>
        inline typename
        TapeType::tdouble_type&
        value(TapeType& tv)
        {
            if (tv.is_ptr())
                return tv.tdouble_();

            tv.alloc_(tv.double_());
            return tv.tdouble_();
        }
    }
}

# if defined CL_TAPE_CPPAD
    namespace CppAD
    {
        template <typename T>
        inline T Value(T v) { return v; }
    }
# endif

# endif //cl_tape_impl_detail_value_detail_hpp