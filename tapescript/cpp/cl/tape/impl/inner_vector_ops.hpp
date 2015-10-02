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

#include <cl/tape/impl/base_inner_vector.hpp>
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
            static atomic_reverse_vec afun;

            const std::array<CppAD::AD<InnerVector>, 1> X = { x };
            std::array<CppAD::AD<InnerVector>, 1> Y;
            afun(X, Y);
            return Y[0];
        }
    };

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
            static atomic_sum_vec afun;

            const std::array<CppAD::AD<InnerVector>, 1> X = { x };
            std::array<CppAD::AD<InnerVector>, 1> Y;
            afun(X, Y);
            return Y[0];
        }
    };

    inline CppAD::AD<InnerVector> sum_vec(const CppAD::AD<InnerVector>& x)
    {
        return SumImpl::sum_vec(x);
    }
}

#endif