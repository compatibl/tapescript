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

#pragma once

#ifndef CL_INNER_VECTOR_OPS_INCLUDED
#define CL_INNER_VECTOR_OPS_INCLUDED

#include <cl/tape/impl/inner/base_inner_vector.hpp>
#include <cppad/local/atomic_base.hpp>
#include <cppad/vector.hpp>
#include <array>
#include <numeric>
#include <assert.h>

namespace cl
{
    struct ReverseImpl
    {
        static inline InnerVector reverse_vec(const InnerVector& x)
        {
            if (x.is_scalar())
                return x;
            InnerVector::vector_type temp(x.vector_value_.size());
            std::reverse_copy(begin(x.vector_value_), end(x.vector_value_), begin(temp));
            return temp;
        }

        struct atomic_reverse_vec : public CppAD::atomic_base<InnerVector>
        {
            typedef InnerVector Base;
            template <class T> using vector = CppAD::vector<T>;

            atomic_reverse_vec()
                : atomic_base("atomic_reverse_vec")
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

            bool for_sparse_jac(
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       r  ,
		              vector< std::set<size_t> >&       s  )
	        {
                s = r;
                return true;
	        }

            bool for_sparse_jac(
		        size_t                                  q  ,
		        const vector<bool>&                     r  ,
		              vector<bool>&                     s  )
	        {
                s = r;
                return true;
	        }

	        bool rev_sparse_jac(
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       rt ,
		              vector< std::set<size_t> >&       st )
            {
                st = rt;
                return true;
	        }

            bool rev_sparse_jac(
		        size_t                                  q  ,
		        const vector<bool>&                     rt ,
		              vector<bool>&                     st )
            {
                st = rt;
                return true;
	        }

	        bool rev_sparse_hes(
		        const vector<bool>&                     vx ,
		        const vector<bool>&                     s  ,
		              vector<bool>&                     t  ,
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       r  ,
		        const vector< std::set<size_t> >&       u  ,
		              vector< std::set<size_t> >&       v  )
            {
                t = s;
                v = u;
                return true;
	        }

	        bool rev_sparse_hes(
		        const vector<bool>&                     vx ,
		        const vector<bool>&                     s  ,
		              vector<bool>&                     t  ,
		        size_t                                  q  ,
		        const vector<bool>&                     r  ,
		        const vector<bool>&                     u  ,
		              vector<bool>&                     v  )
            {
                t = s;
                v = u;
                return true;
	        }
        };

        static inline CppAD::AD<InnerVector> reverse_vec(const CppAD::AD<InnerVector>& x)
        {
            typedef std::array<CppAD::AD<InnerVector>, 1> ADVector;
            static atomic_reverse_vec afun;

            const ADVector X = { x };
            ADVector Y;
            afun(X, Y);
            return Y[0];
        }
    };

    // Returns reversed vector.
    inline CppAD::AD<InnerVector> reverse_vec(const CppAD::AD<InnerVector>& x)
    {
        return ReverseImpl::reverse_vec(x);
    }


    struct SumImpl
    {
        static inline InnerVector sum_vec(const InnerVector& x)
        {
            if (x.is_scalar())
                return x;
            return std::accumulate(begin(x.vector_value_), end(x.vector_value_), 0.0);
        }

        struct atomic_sum_vec : public CppAD::atomic_base<InnerVector>
        {
            typedef InnerVector Base;
            template <class T> using vector = CppAD::vector<T>;

            atomic_sum_vec()
                : atomic_base("atomic_sum_vec")
            {}

            static inline InnerVector adjust_size(double val, const InnerVector& model)
            {
                // Uncomment next line for more efficiency.
                // (vector/scalar state for Reverse return will be undefined)
                return val;
                if (model.is_scalar())
                    return val;
                return InnerVector(val, model.vector_value_.size());
            }

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
                    if (tx[i].is_scalar() && tx[0].is_vector())
                    {
                        ty[i] = tx[i] * tx[0].vector_value_.size();
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
                    px[i] = adjust_size(sum_vec(py[i]).scalar_value_, tx[0]);
                return true;
	        }

            bool for_sparse_jac(
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       r  ,
		              vector< std::set<size_t> >&       s  )
	        {
                s = r;
                return true;
	        }

            bool for_sparse_jac(
		        size_t                                  q  ,
		        const vector<bool>&                     r  ,
		              vector<bool>&                     s  )
	        {
                s = r;
                return true;
	        }

	        bool rev_sparse_jac(
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       rt ,
		              vector< std::set<size_t> >&       st )
            {
                st = rt;
                return true;
	        }

            bool rev_sparse_jac(
		        size_t                                  q  ,
		        const vector<bool>&                     rt ,
		              vector<bool>&                     st )
            {
                st = rt;
                return true;
	        }

	        bool rev_sparse_hes(
		        const vector<bool>&                     vx ,
		        const vector<bool>&                     s  ,
		              vector<bool>&                     t  ,
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       r  ,
		        const vector< std::set<size_t> >&       u  ,
		              vector< std::set<size_t> >&       v  )
            {
                t = s;
                v = u;
                return true;
	        }

	        bool rev_sparse_hes(
		        const vector<bool>&                     vx ,
		        const vector<bool>&                     s  ,
		              vector<bool>&                     t  ,
		        size_t                                  q  ,
		        const vector<bool>&                     r  ,
		        const vector<bool>&                     u  ,
		              vector<bool>&                     v  )
            {
                t = s;
                v = u;
                return true;
	        }
        };

        static inline CppAD::AD<InnerVector> sum_vec(const CppAD::AD<InnerVector>& x)
        {
            typedef std::array<CppAD::AD<InnerVector>, 1> ADVector;
            static atomic_sum_vec afun;

            const ADVector X = { x };
            ADVector Y;
            afun(X, Y);
            return Y[0];
        }
    };

    // Returns sum of vector elements.
    inline CppAD::AD<InnerVector> sum_vec(const CppAD::AD<InnerVector>& x)
    {
        return SumImpl::sum_vec(x);
    }


    struct ConcImpl
    {
        static inline size_t size(const InnerVector& x)
        {
            if (x.is_scalar())
            {
                return 1;
            }
            return x.vector_value_.size();
        }

        static inline bool equal_sized(const InnerVector& x, const InnerVector& y)
        {
            return x.is_scalar() == y.is_scalar() && size(x) == size(y);
        }

        static inline const double* begin(const InnerVector& x)
        {
            if (x.is_scalar())
            {
                return &(x.scalar_value_);
            }
            return std::begin(x.vector_value_);
        }

        static inline const double* end(const InnerVector& x)
        {
            if (x.is_scalar())
            {
                return &(x.scalar_value_) + 1;
            }
            return std::end(x.vector_value_);
        }

        static inline InnerVector conc_vec(const InnerVector& x, const InnerVector& y, size_t size0, size_t size1)
        {
            InnerVector result(0.0, size0 + size1);
            auto x_dest = result.vector_value_[std::slice(0, size0, 1)];
            auto y_dest = result.vector_value_[std::slice(size0, size1, 1)];
            if (x.is_scalar())
            {
                x_dest = x.scalar_value_;
            }
            else
            {
                x_dest = x.vector_value_;
            }
            if (y.is_scalar())
            {
                y_dest = y.scalar_value_;
            }
            else
            {
                y_dest = y.vector_value_;
            }
            return result;
        }

        struct atomic_conc_vec : public CppAD::atomic_base<InnerVector>
        {
            typedef InnerVector Base;
            template <class T> using vector = CppAD::vector<T>;

            atomic_conc_vec()
                : atomic_base("atomic_conc_vec")
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
			        vy[0] = vx[0] || vx[1];
		        }
                for (size_t i = p; i <= q; i++)
                {
                    auto& left = tx[i];
                    auto& right = tx[q + 1 + i];
                    if (left.is_scalar() && right.is_scalar() && left == right)
                    {
                        ty[i] = left;
                    }
                    else
                    {
                        ty[i] = conc_vec(tx[i], tx[q + 1 + i], size(tx[0]), size(tx[q + 1]));
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
#ifndef NDEBUG
                assert(py.size() == q + 1);
                assert(px.size() == 2 * (q + 1));
#endif
                for (size_t i = 0; i <= q; i++)
                {
                    if (py[i].is_scalar())
                    {
                        px[i] = py[i];
                        px[i + q + 1] = py[i];
                    }
                    else
                    {
                        size_t size0 = size(tx[0]);
                        size_t size1 = size(tx[q + 1]);
                        assert(size(py[i]) == size0 + size1);
                        if (tx[0].is_scalar())
                        {
                            px[i] = py[i].vector_value_[0];
                        }
                        else
                        {
                            px[i] = InnerVector(0.0, size0);
                            std::copy(begin(py[i]), begin(py[i]) + size0, std::begin(px[i].vector_value_));
                        }
                        if (tx[q + 1].is_scalar())
                        {
                            px[i + q + 1] = py[i].vector_value_[size0];
                        }
                        else
                        {
                            px[i + q + 1] = InnerVector(0.0, size1);
                            std::copy(begin(py[i]) + size0, end(py[i]), std::begin(px[i + q + 1].vector_value_));
                        }
                    }
                }
                return true;
	        }

            bool for_sparse_jac(
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       r  ,
		              vector< std::set<size_t> >&       s  )
	        {
                std::set_union(r[0].begin(), r[0].end(), r[1].begin(), r[1].end()
                    , std::inserter(s[0], s[0].begin()));
                return true;
	        }

            bool for_sparse_jac(
		        size_t                                  q  ,
		        const vector<bool>&                     r  ,
		              vector<bool>&                     s  )
            {
                for (size_t j = 0; j < q; j++)
                {
                    s[j] = r[0 * q + j] || r[1 * q + j];
                }
                return true;
	        }

	        bool rev_sparse_jac(
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       rt ,
		              vector< std::set<size_t> >&       st )
            {
                st[0] = rt[0];
                st[1] = rt[0];
                return true;
	        }

            bool rev_sparse_jac(
		        size_t                                  q  ,
		        const vector<bool>&                     rt ,
		              vector<bool>&                     st )
            {
                for (size_t j = 0; j < q; j++)
                {
                    st[0 * q + j] = rt[j];
                    st[1 * q + j] = rt[j];
                }
                return true;
	        }

	        bool rev_sparse_hes(
		        const vector<bool>&                     vx ,
		        const vector<bool>&                     s  ,
		              vector<bool>&                     t  ,
		        size_t                                  q  ,
		        const vector< std::set<size_t> >&       r  ,
		        const vector< std::set<size_t> >&       u  ,
		              vector< std::set<size_t> >&       v  )
            {
                t[0] = s[0];
                t[1] = s[0];

                v[0] = u[0];
                v[1] = u[0];
                return true;
	        }

	        bool rev_sparse_hes(
		        const vector<bool>&                     vx ,
		        const vector<bool>&                     s  ,
		              vector<bool>&                     t  ,
		        size_t                                  q  ,
		        const vector<bool>&                     r  ,
		        const vector<bool>&                     u  ,
		              vector<bool>&                     v  )
            {
                t[0] = s[0];
                t[1] = s[0];

                for (size_t j = 0; j < q; j++)
                {
                    v[0 * q + j] = u[j];
                    v[1 * q + j] = u[j];
                }
                return true;
	        }
        };

        static inline CppAD::AD<InnerVector> conc_vec(const CppAD::AD<InnerVector>& x, const CppAD::AD<InnerVector>& y)
        {
            typedef std::vector<CppAD::AD<InnerVector>> ADVector;
            static atomic_conc_vec afun;

            const ADVector X = { x, y };
            ADVector Y(1);
            afun(X, Y);
            return Y[0];
        }
    };

    // Concatenation of two vectors.
    inline CppAD::AD<InnerVector> conc_vec(const CppAD::AD<InnerVector>& x, const CppAD::AD<InnerVector>& y)
    {
        return ConcImpl::conc_vec(x, y);
    }
}

#endif