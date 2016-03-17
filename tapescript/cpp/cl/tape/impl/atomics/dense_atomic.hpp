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

#ifndef cl_tape_impl_atomics_dense_atomic_hpp
#define cl_tape_impl_atomics_dense_atomic_hpp

#include <array>
#include <vector>
#include <numeric>
#include <set>
#include <assert.h>
#include <cppad/vector.hpp>

namespace cl
{
    namespace tapescript
    {
        // Implementation of atomic_base sparsity patterns for transformations with dense Jacobian.
        // For more details see http://www.coin-or.org/CppAD/Doc/atomic_base.htm.
        template <class Inner>
        struct dense_atomic
            : CppAD::atomic_base<Inner>
        {
            typedef Inner Base;
            template <class T> using vector = CppAD::vector<T>;

            explicit dense_atomic(const std::string&  name)
                : CppAD::atomic_base<Inner>(name)
            {}

            bool for_sparse_jac(
                size_t                                  q,
                const vector< std::set<size_t> >&       r,
                vector< std::set<size_t> >&       s)
            {
                for (size_t i = 0; i < r.size(); i++)
                {
                    s[0].insert(r[i].begin(), r[i].end());
                }
                for (size_t i = 1; i < s.size(); i++)
                {
                    s[i] = s[0];
                }

                return true;
            }

            bool for_sparse_jac(
                size_t                                  q,
                const vector<bool>&                     r,
                vector<bool>&                     s)
            {
                CL_ASSERT(s.size() % q == 0, "Invalid sparsity vector size.");
                CL_ASSERT(r.size() % q == 0, "Invalid sparsity vector size.");

                size_t n = r.size() / q;
                size_t m = s.size() / q;

                for (size_t j = 0; j < q; j++)
                {
                    s[j] = 0;
                    for (size_t i = 0; i < n; i++)
                    {
                        s[j] = s[j] || r[i * q + j];
                    }
                    for (size_t i = 1; i < m; i++)
                    {
                        s[i * q + j] = s[j];
                    }
                }
                return true;
            }

            bool rev_sparse_jac(
                size_t                                  q,
                const vector< std::set<size_t> >&       rt,
                vector< std::set<size_t> >&       st)
            {
                for (size_t i = 0; i < rt.size(); i++)
                {
                    st[0].insert(rt[i].begin(), rt[i].end());
                }
                for (size_t i = 1; i < st.size(); i++)
                {
                    st[i] = st[0];
                }

                return true;
            }

            bool rev_sparse_jac(
                size_t                                  q,
                const vector<bool>&                     rt,
                vector<bool>&                     st)
            {
                CL_ASSERT(st.size() % q == 0, "Invalid sparsity vector size.");
                CL_ASSERT(rt.size() % q == 0, "Invalid sparsity vector size.");

                size_t m = rt.size() / q;
                size_t n = st.size() / q;

                for (size_t j = 0; j < q; j++)
                {
                    st[j] = 0;
                    for (size_t i = 0; i < m; i++)
                    {
                        st[j] = st[j] || rt[i * q + j];
                    }
                    for (size_t i = 0; i < n; i++)
                    {
                        st[i * q + j] = st[j];
                    }
                }
                return true;
            }
        };
    } // namespace tapescript
} // namespace cl

#endif
