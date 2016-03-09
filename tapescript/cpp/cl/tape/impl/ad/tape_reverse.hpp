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

This file includes code from CppAD, a C++ algorithmic differentiation library
distributed under multiple licenses. This distribution is under the terms of
the Eclipse Public License Version 1.0, a copy of which is available at:

https://www.eclipse.org/legal/epl-v10.html

CppAD code included in this file is subject to copyright:

Copyright (C) 2003-15 Bradley M. Bell
*/

#ifndef cl_tape_impl_ad_tape_reverse_hpp
#define cl_tape_impl_ad_tape_reverse_hpp

# include <cl/tape/impl/inner/tape_inner.hpp>

namespace CppAD
{
    template <int I, typename Arg>
    inline typename std::conditional<I == 0, Arg const&
        , cl::empty_result>::type getarg(Arg const& v)
    {
        return v;
    }

    template <int I, typename VectorBase, typename Stream>
    inline typename std::conditional<I == 0, VectorBase, Stream*>::type
    getarg(std::pair<VectorBase, Stream*> const& v)
    {
        return std::get<I>(v);
    }

    // This class is a helper for make result pair and not
    // we nned optimize copying in nex steps
    template <typename VectorType>
    struct make_result
    {
        make_result(VectorType&  v) : v_(v)
        {}

        VectorType& get() { return v_; }
        VectorType& v_;
    };

    template <typename VectorType, typename Stream>
    struct make_result<std::pair<VectorType, Stream*>>
    {
        make_result(VectorType& v) : v_(v)
        {}

        std::pair<VectorType, Stream*> get() { return std::make_pair(v_, (Stream*)0); }
        VectorType& v_;
    };


    template<typename Base>
    template <typename VectorBaseType>
    VectorBaseType ADFun<Base >::Reverse(size_t q, const VectorBaseType &arg)
    {   // constants
        const Base zero(0);

        // Calculation cases when we have
        // stream and certain vector
        auto w = getarg<0>(arg);
        auto stream = getarg<1>(arg);

        // Declaration of
        typedef decltype(w) VectorBase;

        // temporary indices
        size_t i, j, k;

        // number of independent variables
        size_t n = ind_taddr_.size();

        // number of dependent variables
        size_t m = dep_taddr_.size();

        pod_vector<Base> Partial;
        Partial.extend(num_var_tape_  * q);

        // update maximum memory requirement
        // memoryMax = std::max( memoryMax,
        // 	Memory() + num_var_tape_  * q * sizeof(Base)
        // );

        // check VectorBase is Simple Vector class with Base type elements
        CheckSimpleVector<Base, VectorBase>();

        CPPAD_ASSERT_KNOWN(
            size_t(w.size()) == m || size_t(w.size()) == (m * q),
            "Argument w to Reverse does not have length equal to\n"
            "the dimension of the range for the corresponding ADFun."
            );
        CPPAD_ASSERT_KNOWN(
            q > 0,
            "The first argument to Reverse must be greater than zero."
            );
        CPPAD_ASSERT_KNOWN(
            num_order_taylor_ >= q,
            "Less that q taylor_ coefficients are currently stored"
            " in this ADFun object."
            );
        // special case where multiple forward directions have been computed,
        // but we are only using the one direction zero order results
        if ((q == 1) & (num_direction_taylor_ > 1))
        {
            num_order_taylor_ = 1;        // number of orders to copy
            size_t c = cap_order_taylor_; // keep the same capacity setting
            size_t r = 1;                 // only keep one direction
            capacity_order(c, r);
        }
        CPPAD_ASSERT_KNOWN(
            num_direction_taylor_ == 1,
            "Reverse mode for Forward(q, r, xq) with more than one direction"
            "\n(r > 1) is not yet supported for q > 1."
            );
	    // initialize entire Partial matrix to zero
        for (i = 0; i < num_var_tape_; i++)
        {
		    for (j = 0; j < q; j++)
            {
			    Partial[i * q + j] = zero;
                cl::tapescript::set_intrusive(Partial[i * q + j], taylor_[i * cap_order_taylor_]);
            }
        }

        // set the dependent variable direction
        // (use += because two dependent variables can point to same location)
        for (i = 0; i < m; i++)
        {
            CPPAD_ASSERT_UNKNOWN(dep_taddr_[i] < num_var_tape_);
            if (size_t(w.size()) == m)
                Partial[dep_taddr_[i] * q + q - 1] += w[i];
            else
            {
                for (k = 0; k < q; k++)
                    // ? should use += here, first make test to demonstrate bug
                    Partial[dep_taddr_[i] * q + k] = w[i * q + k];
            }
        }

        // evaluate the derivatives
        CPPAD_ASSERT_UNKNOWN(cskip_op_.size() == play_.num_op_rec());
        CPPAD_ASSERT_UNKNOWN(load_op_.size() == play_.num_load_op_rec());
        ReverseSweep(
            q - 1,
            n,
            num_var_tape_,
            &play_,
            cap_order_taylor_,
            taylor_.data(),
            q,
            Partial.data(),
            cskip_op_.data(),
            load_op_,
            stream
            );

        // return the derivative values
        VectorBase value(n * q);
        for (j = 0; j < n; j++)
        {
            CPPAD_ASSERT_UNKNOWN(ind_taddr_[j] < num_var_tape_);

            // independent variable taddr equals its operator taddr
            CPPAD_ASSERT_UNKNOWN(play_.GetOp(ind_taddr_[j]) == InvOp);

            // by the Reverse Identity Theorem
            // partial of y^{(k)} w.r.t. u^{(0)} is equal to
            // partial of y^{(q-1)} w.r.t. u^{(q - 1 - k)}
            if (size_t(w.size()) == m)
            {
                for (k = 0; k < q; k++)
                    value[j * q + k] =
                    Partial[ind_taddr_[j] * q + q - 1 - k];
            }
            else
            {
                for (k = 0; k < q; k++)
                    value[j * q + k] =
                    Partial[ind_taddr_[j] * q + k];
            }
        }
        CPPAD_ASSERT_KNOWN(!(hasnan(value) && check_for_nan_),
            "dw = f.Reverse(q, w): has a nan,\n"
            "but none of its Taylor coefficents are nan."
            );

        for (size_t i = 0; i < value.size(); i++)
        {
            cl::tapescript::set_not_intrusive(value[i]);
        }
        return make_result<VectorBaseType>(value).get();
    }
}

#endif // cl_tape_impl_ad_tape_reverse_hpp