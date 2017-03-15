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

#ifndef cl_tape_impl_inner_detail_experimental_atomic_reverse_hpp
#define cl_tape_impl_inner_detail_experimental_atomic_reverse_hpp

#include <cl/tape/impl/atomics/dense_atomic.hpp>
#include <cl/tape/impl/inner/base_tape_inner.hpp>

namespace cl
{
    namespace tapescript
    {
        template <class Inner>
        struct reverse_vec_impl
        {
            typedef Inner inner_type;

            static inline inner_type reverse_vec(const inner_type& x)
            {
                if (x.is_scalar())
                    return x;
                typename inner_type::array_type temp(x.array_value_.size());
                std::reverse_copy(begin(x.array_value_), end(x.array_value_), begin(temp));
                return temp;
            }

            struct atomic_reverse_vec : dense_atomic<inner_type>
            {
                typedef typename dense_atomic<inner_type>::Base Base;
                template <class T> using vector = CppAD::vector<T>;

                atomic_reverse_vec()
                    : dense_atomic<inner_type>("Reversing")
                {}

                bool forward(
                    size_t                    p ,
                    size_t                    q ,
                    const vector<bool>&      vx ,
                          vector<bool>&      vy ,
                    const vector<Base>&      tx ,
                          vector<Base>&      ty )
                {
                    vy = vx;
                    for (size_t i = p; i <= q; i++)
                        ty[i] = reverse_vec(tx[i]);
                    return true;
                }

                bool reverse(
                    size_t                    q  ,
                    const vector<Base>&       tx ,
                    const vector<Base>&       ty ,
                          vector<Base>&       px ,
                    const vector<Base>&       py )
                {
    #ifndef NDEBUG
                    for (size_t i = 0; i < tx.size(); i++)
                        assert(tx[i] == reverse_vec(ty[i]));
    #endif
                    for (size_t i = 0; i < py.size(); i++)
                        px[i] = reverse_vec(py[i]);
                    return true;
                }
            };

            CppAD::AD<inner_type> operator()(const CppAD::AD<inner_type>& x)
            {
                typedef std::array<CppAD::AD<inner_type>, 1> ADVector;
                static atomic_reverse_vec afun;

                const ADVector X = { x };
                ADVector Y;
                afun(X, Y);
                return Y[0];
            }
        };

        // Returns reversed vector.
        template <class Inner>
        inline CppAD::AD<Inner> reverse_vec(const CppAD::AD<Inner>& x)
        {
            return reverse_vec_impl<Inner>()(x);
        }

        // Returns reversed vector.
        template <class Inner>
        inline tape_wrapper<Inner> reverse_vec(const tape_wrapper<Inner>& x)
        {
            return reverse_vec(x.value());
        }
    } // namespace tapescript
} // namespace cl

#endif
