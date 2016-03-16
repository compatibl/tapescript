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

#ifndef cl_tape_impl_detail_experimental_atomic_linear_ops_hpp
#define cl_tape_impl_detail_experimental_atomic_linear_ops_hpp

#include <cl/tape/impl/atomics/dense_atomic.hpp>
#include <cl/tape/impl/inner/base_tape_inner.hpp>

namespace cl
{
    namespace tapescript
    {
        template <class Inner>
        struct movingaverage_vec_impl
        {
            typedef Inner inner_type;

            static inline inner_type movingaverage_vec(const inner_type& x, double w)
            {
                if (x.is_scalar())
                    return x;
                int n = x.array_value_.size();
                typename inner_type::array_type temp(n);
                temp[0] = x[0];
                for (int i = 1; i < n; i++)
                {
                    temp[i] = w * x[i - 1] + (1 - w) * x[i];
                }
                return temp;
            }

            static inline inner_type movingaverage_vec_reverse(const inner_type& x, double w)
            {
                if (x.is_scalar())
                    return x;
                int n = x.array_value_.size();
                typename inner_type::array_type temp(n);
                temp[0] = x[0];
                for (int i = 1; i < n; i++)
                {
                    temp[i] = (x[i] - temp[i - 1] * w) / (1 - w);
                }
                return temp;
            }

            struct atomic_movingaverage_vec : public dense_atomic<inner_type>
            {
                typedef inner_type Base;
                template <class T> using vector = CppAD::vector<T>;

                // weight w: x[i] = w * x[i - 1] + (1 - w) * x[i]
                const double w_;

                atomic_movingaverage_vec(double w)
                    : dense_atomic<inner_type>("Moving Average"),
                    w_(w)
                {}

                bool forward(
                    size_t                    p,
                    size_t                    q,
                    const vector<bool>&       vx,
                    vector<bool>&             vy,
                    const vector<Base>&       tx,
                    vector<Base>&             ty)
                {
                    vy = vx;
                    for (size_t i = p; i <= q; i++)
                        ty[i] = movingaverage_vec(tx[i], w_);
                    return true;
                }

                bool reverse(
                    size_t                    q,
                    const vector<Base>&       tx,
                    const vector<Base>&       ty,
                    vector<Base>&       px,
                    const vector<Base>&       py)
                {
#ifndef NDEBUG
                    for (size_t i = 0; i < tx.size(); i++)
                        assert(ty[i] == movingaverage_vec(tx[i], w_));
#endif
                    for (size_t i = 0; i < py.size(); i++)
                    {
                        px[i] = movingaverage_vec_reverse(py[i], w_);
                    }
                    return true;
                }
            };

            CppAD::AD<inner_type> operator()(const CppAD::AD<inner_type>& x, double w)
            {
                typedef std::array<CppAD::AD<inner_type>, 1> ADVector;

                //!! TODO: Remove weight from initialization.
                static atomic_movingaverage_vec afun(w);

                const ADVector X = { x };
                ADVector Y;
                afun(X, Y);
                return Y[0];
            }
        };

        // Returns moving average vector.
        template <class Inner>
        inline CppAD::AD<Inner> movingaverage_vec(const CppAD::AD<Inner>& x, double w)
        {
            return movingaverage_vec_impl<Inner>()(x, w);
        }

        // Returns moving average vector.
        template <class Inner>
        inline tape_wrapper<Inner> movingaverage_vec(const tape_wrapper<Inner>& x, double w = 0.5)
        {
            return movingaverage_vec(x.value(), w);
        }
    }
}

#endif
