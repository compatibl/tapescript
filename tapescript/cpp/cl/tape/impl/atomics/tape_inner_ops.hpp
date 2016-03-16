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

#ifndef cl_tape_impl_inner_tape_inner_ops_hpp
#define cl_tape_impl_inner_tape_inner_ops_hpp

#include <cl/tape/impl/atomics/dense_atomic.hpp>
#include <cl/tape/impl/inner/base_tape_inner.hpp>

namespace cl
{
    namespace tapescript
    {
        template <class Inner>
        struct sum_vec_impl
        {
            typedef Inner inner_type;

            static inline inner_type sum_vec(const inner_type& x)
            {
                if (x.is_scalar())
                    return x;
                return std::accumulate(begin(x.array_value_), end(x.array_value_), 0.0);
            }

            struct atomic_sum_vec : dense_atomic<inner_type>
            {
                atomic_sum_vec()
                    : dense_atomic<inner_type>("Sum")
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
                    {
                        if (tx[i].is_scalar() && tx[0].is_array())
                        {
                            ty[i] = tx[i] * tx[0].array_value_.size();
                        }
                        else
                        {
                            ty[i] = sum_vec(tx[i]);
                        }
                    }
                    return true;
                }

                bool reverse(
                    size_t                    q  ,
                    const vector<Base>&       tx ,
                    const vector<Base>&       ty ,
                          vector<Base>&       px ,
                    const vector<Base>&       py )
                {
                    for (size_t i = 0; i < py.size(); i++)
                    {
                        CL_ASSERT(py[i].is_scalar(), "Sum of vector have to be a scalar."
                            " Check that intrusive scalars work in reverse sweep.");
                        px[i] = py[i].scalar_value_;
                    }
                    return true;
                }
            };

            CppAD::AD<inner_type> operator()(const CppAD::AD<inner_type>& x)
            {
                typedef std::array<CppAD::AD<inner_type>, 1> ADVector;
                static atomic_sum_vec afun;

                const ADVector X = { x };
                ADVector Y;
                afun(X, Y);
                return Y[0];
            }
        };

        // Returns sum of vector elements.
        template <class Inner>
        inline CppAD::AD<Inner> sum_vec(const CppAD::AD<Inner>& x)
        {
            return sum_vec_impl<Inner>()(x);
        }

        // Returns sum of vector elements.
        template <class Inner>
        inline tape_wrapper<Inner> sum_vec(const tape_wrapper<Inner> &x)
        {
            return sum_vec(x.value());
        }


        template <class Inner>
        struct conc_vec_impl
        {
            typedef Inner inner_type;
            typedef typename Inner::scalar_type scalar_type;

            static inline size_t size(const inner_type& x)
            {
                if (x.is_scalar())
                {
                    return 1;
                }
                return x.array_value_.size();
            }

            static inline bool equal_sized(const inner_type& x, const inner_type& y)
            {
                return x.is_scalar() == y.is_scalar() && size(x) == size(y);
            }

            static inline const scalar_type* begin(const inner_type& x)
            {
                if (x.is_scalar())
                {
                    return &(x.scalar_value_);
                }
                return std::begin(x.array_value_);
            }

            static inline scalar_type* begin(inner_type& x)
            {
                if (x.is_scalar())
                {
                    return &(x.scalar_value_);
                }
                return std::begin(x.array_value_);
            }

            static inline const scalar_type* end(const inner_type& x)
            {
                if (x.is_scalar())
                {
                    return &(x.scalar_value_) + 1;
                }
                return std::end(x.array_value_);
            }

            struct atomic_conc_vec : dense_atomic<inner_type>
            {
                atomic_conc_vec()
                    : dense_atomic<inner_type>("Concatenation")
                {}

                bool forward(
                    size_t                    p ,
                    size_t                    q ,
                    const vector<bool>&      vx ,
                          vector<bool>&      vy ,
                    const vector<Base>&      tx ,
                          vector<Base>&      ty )
                {
                    if( vx.size() > 0 )
                    {
                        vy[0] = std::accumulate(vx.data(), vx.data() + vx.size(), false, std::logical_or<bool>());
                    }

                    for (size_t i = p; i <= q; i++)
                    {
                        size_t total_size = 0;
                        for (size_t j = 0; j < tx.size(); j += q + 1)
                        {
                            total_size += size(tx[j]);
                        }
                        ty[i].resize(total_size);

                        auto dest = begin(ty[i]);
                        for (size_t j = i; j < tx.size(); j += q + 1)
                        {
                            // Size of zero order taylor coefficient.
                            size_t implied_size = size(tx[j - j % (q + 1)]);
                            if (tx[j].is_scalar())
                            {
                                // If zero order taylor coefficient is an array,
                                // than higer order taylor coefficient should be
                                // treated as an array even if it is a scalar.
                                std::fill(dest, dest + implied_size, tx[j].scalar_value_);
                                dest += implied_size;
                            }
                            else
                            {
                                CL_ASSERT(size(tx[j]) == implied_size
                                    , "Zero order taylor coefficient and higer order taylor coefficient size mismatch");
                                dest = std::copy(begin(tx[j]), end(tx[j]), dest);
                            }
                        }
                    }
                    return true;
                }

                bool reverse(
                    size_t                    q  ,
                    const vector<Base>&       tx ,
                    const vector<Base>&       ty ,
                          vector<Base>&       px ,
                    const vector<Base>&       py )
                {
                    CL_ASSERT(py.size() == q + 1, "Invalid vector size. Number of output variables (m) have to be 1.");
                    CL_ASSERT(ty.size() == q + 1, "Invalid vector size. Number of output variables (m) have to be 1.");

                    for (size_t i = 0; i <= q; i++)
                    {
                        if (py[i].is_scalar())
                        {
                            // case1: py[i] represent an array with all coefficients equal,
                            //        so it is cut to representation of arrays with all coefficients equal
                            // case2: py[i] is a real scalar, px[i] should be a scalar too.
                            for (size_t j = i; j < tx.size(); j += q + 1)
                            {
                                px[j] = py[i];
                            }
                        }
                        else
                        {
                            CL_ASSERT(size(py[i]) == size(ty[0])
                                , "Reverse weights array and value array size mismatch.");

                            auto source = begin(py[i]);

                            for (size_t j = i; j < tx.size(); j += q + 1)
                            {
                                // Zero order taylor coefficient (value)
                                auto& origin = tx[j - j % (q + 1)];
                                // Copy from source to px[j] of size and mode of origin.
                                if (origin.is_scalar())
                                {
                                    px[j] = *source++;
                                }
                                else
                                {
                                    size_t implied_size = origin.size();
                                    px[j].resize(implied_size);
                                    std::copy(source, source + implied_size, begin(px[j]));
                                    source += implied_size;
                                }
                            }
                            CL_ASSERT(source == end(py[i])
                                , "Copied element nubmer and weights array size mismatch.");
                        }
                    }
                    return true;
                }
            };

            template <class ADVector>
            CppAD::AD<inner_type> operator()(const ADVector& v)
            {
                static atomic_conc_vec afun;

                ADVector const& X = v;
                ADVector Y(1);
                afun(X, Y);
                return Y[0];
            }
        };

        // Concatenation of arrays and scalars.
        template <class ADVector>
        inline typename ADVector::value_type conc_vec(const ADVector& v)
        {
            typedef typename remove_ad<typename ADVector::value_type>::type inner_type;
            return conc_vec_impl<inner_type>()(v);
        }

        // Concatenation of arrays and scalars.
        template <class Inner>
        inline tape_wrapper<Inner> conc_vec(std::vector<tape_wrapper<Inner>> const& v)
        {
            return conc_vec(tapescript::adapt(v));
        }

        template <class OutIt, class Head, class ... Tail>
        OutIt copy(OutIt dest, Head const& first, Tail const& ... rest)
        {
            *dest++ = first;
            return copy(dest, rest ...);
        }

        template <class OutIt>
        OutIt copy(OutIt dest)
        {
            return dest;
        }

        // Concatenation of tape_wrappers (arrays or scalars).
        template <class Inner, class ... Tail>
        inline tape_wrapper<Inner> conc_vec(tape_wrapper<Inner> const& head, Tail const& ... tail)
        {
            std::vector<tape_wrapper<Inner>> proxy(1 + sizeof...(Tail));
            copy(proxy.begin(), head, tail ...);
            return conc_vec(proxy);
        }


        template <class Inner>
        struct make_vec_impl
        {
            typedef Inner inner_type;

            static inline inner_type sum_vec(const inner_type& x)
            {
                if (x.is_scalar())
                    return x;
                return std::accumulate(begin(x.array_value_), end(x.array_value_), 0.0);
            }

            static inline size_t size(const inner_type& x)
            {
                if (x.is_scalar())
                {
                    return 1;
                }
                return x.array_value_.size();
            }

            struct atomic_make_vec : dense_atomic<inner_type>
            {
                atomic_make_vec()
                    : dense_atomic<inner_type>("Construction(val, n)")
                {}

                bool forward(
                    size_t                    p,
                    size_t                    q,
                    const vector<bool>&      vx,
                    vector<bool>&            vy,
                    const vector<Base>&      tx,
                    vector<Base>&            ty)
                {
                    if (vx.size() > 0)
                    {
                        vy[0] = vx[0];
                        CL_ASSERT(vx[1] == false, "x[1] have to be an integer parameter (constructed array size)");
                    }
                    for (size_t i = p; i <= q; i++)
                    {
                        // filled value
                        auto& left = tx[i];
                        // array size
                        auto& right = tx[q + 1];
                        CL_ASSERT(left.is_scalar(), "Constructed array have to be filled with scalar value.");
                        ty[i] = inner_type(left.scalar_value_, CppAD::Integer(right));
                    }
                    return true;
                }

                bool reverse(
                    size_t                    q,
                    const vector<Base>&       tx,
                    const vector<Base>&       ty,
                    vector<Base>&             px,
                    const vector<Base>&       py)
                {
                    CL_ASSERT(py.size() == q + 1, "Invalid vector size. Number of output variables (m) have to be 1.");
                    CL_ASSERT(ty.size() == q + 1, "Invalid vector size. Number of output variables (m) have to be 1.");

                    // size of the constructed array.
                    size_t res_size = CppAD::Integer(tx[q + 1]);
                    for (size_t i = 0; i <= q; i++)
                    {
                        CL_ASSERT(tx[i].is_scalar(), "Constructed array have to be filled with scalar value.");
                        if (py[i].is_scalar())
                        {
                            // py[i] represents an array of size res_size.
                            px[i] = py[i] * res_size;
                        }
                        else
                        {
                            CL_ASSERT(size(py[i]) == res_size, "Reverse weights array and constructed array size mismatch.");

                            px[i] = sum_vec(py[i]);
                        }
                    }
                    return true;
                }
            };

            CppAD::AD<inner_type> operator()(const CppAD::AD<inner_type>& x, size_t count)
            {
                typedef std::vector<CppAD::AD<inner_type>> ADVector;
                static atomic_make_vec afun;

                const ADVector X = { x, CppAD::AD<inner_type>(count) };
                ADVector Y(1);
                afun(X, Y);
                return Y[0];
            }
        };

        // Construct an array of given size and fills it with one value.
        template <class Inner>
        inline CppAD::AD<Inner> make_vec(const CppAD::AD<Inner>& x, size_t count)
        {
            return make_vec_impl<Inner>()(x, count);
        }

        // Construct an array of given size and fills it with one value.
        template <class Inner>
        inline tape_wrapper<Inner> make_vec(const tape_wrapper<Inner>& x, size_t count)
        {
            return make_vec(x.value(), count);
        }


        template <class Inner>
        struct unpack_vec_impl
        {
            typedef Inner inner_type;
            typedef typename Inner::scalar_type scalar_type;

            struct atomic_unpack_vec : dense_atomic<inner_type>
            {
                atomic_unpack_vec()
                    : dense_atomic<inner_type>("Unpack")
                {}

                bool forward(
                    size_t                    p,
                    size_t                    q,
                    const vector<bool>&      vx,
                    vector<bool>&      vy,
                    const vector<Base>&      tx,
                    vector<Base>&      ty)
                {
                    // Size of unpacked array / number of output variables (n);
                    size_t m = tx[0].size();

                    CL_ASSERT(tx.size() == q + 1, "Invalid vector size. Number of input variables (n) have to be 1.");
                    CL_ASSERT(m * (q + 1) == ty.size(), "Invalid vector size. Number of output variables have to be m.");

                    if (vx.size() > 0)
                    {
                        std::fill(vy.data(), vy.data() + vy.size(), vx[0]);
                    }

                    for (size_t i = p; i <= q; i++)
                    {
                        for (size_t j = i, k = 0; k < m; j += q + 1, k++)
                        {
                            ty[j] = tx[i].element_at(k);
                        }
                    }
                    return true;
                }

                bool reverse(
                    size_t                    q,
                    const vector<Base>&       tx,
                    const vector<Base>&       ty,
                    vector<Base>&       px,
                    const vector<Base>&       py)
                {
                    size_t m = tx[0].size();

                    for (size_t i = 0; i <= q; i++)
                    {
                        px[i].resize(m);
                        for (size_t j = i, k = 0; k < m; j += q + 1, k++)
                        {
                            CL_ASSERT(py[j].is_scalar(), "Unpacked variables have to be scalars. So does its weights.");
                            px[i].array_value_[k] = py[j].scalar_value_;
                        }
                    }
                    return true;
                }
            };

            template <class ADVector>
            void operator()(const CppAD::AD<Inner>& x, ADVector& result)
            {
                static atomic_unpack_vec afun;

                ADVector X(1);
                X[0] = x;
                ADVector& Y = result;

                afun(X, Y);
            }
        };

        // Conversion from an array to vector of scalars.
        template <class Inner, class ADVector>
        inline void unpack_vec(const CppAD::AD<Inner>& x, ADVector& result)
        {
            unpack_vec_impl<Inner>()(x, result);
        }

        // Concatenation of arrays and scalars.
        template <class Inner>
        inline std::vector<tape_wrapper<Inner>> unpack_vec(const tape_wrapper<Inner>& x, size_t n)
        {
            std::vector<tape_wrapper<Inner>> result(n);
            unpack_vec(x.value(), tapescript::adapt(result));
            return result;
        }


        template <class Inner>
        struct pack_vec_impl
        {
            typedef Inner inner_type;
            typedef typename Inner::scalar_type scalar_type;

            struct atomic_pack_vec : dense_atomic<inner_type>
            {
                atomic_pack_vec()
                    : dense_atomic<inner_type>("Pack")
                {}

                bool forward(
                    size_t                    p,
                    size_t                    q,
                    const vector<bool>&      vx,
                    vector<bool>&      vy,
                    const vector<Base>&      tx,
                    vector<Base>&      ty)
                {
                    CL_ASSERT(ty.size() == q + 1, "Invalid vector size. Number of output variables (m) have to be 1.");
                    CL_ASSERT(tx.size() % (q + 1) == 0, "Invalid vector size.");

                    size_t n = tx.size() / (q + 1);

                    if (vx.size() > 0)
                    {
                        vy[0] = std::accumulate(vx.data(), vx.data() + vx.size(), false, std::logical_or<bool>());
                    }

                    for (size_t i = p; i <= q; i++)
                    {
                        ty[i].resize(n);
                        for (size_t j = i, k = 0; j < tx.size(); j += q + 1, k++)
                        {
                            CL_ASSERT(tx[j].is_scalar(), "Packed variables have to be scalars.");
                            ty[i].array_value_[k] = tx[j].scalar_value_;
                        }
                    }
                    return true;
                }

                bool reverse(
                    size_t                    q,
                    const vector<Base>&       tx,
                    const vector<Base>&       ty,
                    vector<Base>&       px,
                    const vector<Base>&       py)
                {
                    CL_ASSERT(ty.size() == q + 1, "Invalid vector size. Number of output variables (m) have to be 1.");
                    CL_ASSERT(py.size() == q + 1, "Invalid vector size. Number of output variables (m) have to be 1.");
                    CL_ASSERT(tx.size() % (q + 1) == 0, "Invalid vector size.");
                    CL_ASSERT(px.size() % (q + 1) == 0, "Invalid vector size.");

                    size_t n = tx.size() / (q + 1);

                    for (size_t i = 0; i <= q; i++)
                    {
                        for (size_t j = i, k = 0; k < n; j += q + 1, k++)
                        {
                            px[j] = py[i].element_at(k);
                        }
                    }
                    return true;
                }
            };

            template <class ADVector>
            CppAD::AD<inner_type> operator()(const ADVector& v)
            {
                static atomic_pack_vec afun;

                ADVector const& X = v;
                ADVector Y(1);
                afun(X, Y);
                return Y[0];
            }
        };

        // Concatenation of arrays and scalars.
        template <class ADVector>
        inline typename ADVector::value_type pack_vec(const ADVector& v)
        {
            typedef typename remove_ad<typename ADVector::value_type>::type inner_type;
            return pack_vec_impl<inner_type>()(v);
        }

        // Concatenation of arrays and scalars.
        template <class Inner>
        inline tape_wrapper<Inner> pack_vec(std::vector<tape_wrapper<Inner>> const& v)
        {
            return pack_vec(tapescript::adapt(v));
        }
    } // namespace tapescript
} // namespace cl

#endif
