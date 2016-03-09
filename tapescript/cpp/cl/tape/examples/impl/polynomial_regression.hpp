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

#ifndef cl_tape_examples_impl_polynomial_regression_hpp
#define cl_tape_examples_impl_polynomial_regression_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace cl
{
    // Class for polynomial regression calculation.
    class polynomial_regression
    {
    private:
        // Order of polynomial regression.
        const int order_;
        // Size of matrixes m = order + 1.
        const int m_;

        // Input data vectors.
        const tobject data_x_;
        const tobject data_y_;

        // Intermediate regression struuctures:
        // Vectors of powers of x from 0 to 2 * (order + 1).
        std::vector<tobject> x_power_;
        // Matrix X * X^T
        std::vector<std::vector<tobject>> mat_X_XT_;
        // Inverse matrix (X * X^T)^-1
        std::vector<std::vector<tobject>> mat_X_XT_inv_;
        // Vector X^T * y.
        std::vector<tobject> vec_XT_y_;

        // Regression coefficients and their derivatives.
        std::vector<tobject> coef_;

        // Status of calculation.
        bool is_coef_calculated;
        bool is_estim_calculated;

    public:
        // Construct using provided data and order of polynomial regression.
        polynomial_regression(tobject& x, tobject& y, int order) : data_x_(x), data_y_(y), order_(order), m_(order + 1)
        {
            is_coef_calculated = false;
            is_estim_calculated = false;
        }

        // Calculate polynomial regression coefficients.
        std::vector<tobject> calculate_coefficients()
        {
            // Create vectors of powers of x from 0 to 2m.
            x_power_.resize(2 * m_);
            x_power_[0] = 1 + data_x_ - data_x_;
            std::vector<tobject> x_power_sum_vec(2 * m_);
            x_power_sum_vec[0] = tapescript::sum_vec(x_power_[0]);
            for (int i = 1; i < 2 * m_; i++)
            {
                x_power_[i] = x_power_[i - 1] * data_x_;
                x_power_sum_vec[i] = tapescript::sum_vec(x_power_[i]);
            }

            // Calculate matrix X * X^T.
            mat_X_XT_.resize(m_);
            for (int i = 0; i < m_; i++)
                mat_X_XT_[i].resize(m_);

            // Calculate lower triangular matrix elements.
            for (int i = 0; i < m_; i++)
                for (int j = 0; j <= i; j++)
                    mat_X_XT_[i][j] = x_power_sum_vec[i + j];

            // Calculate the rest of matrix elements using symmetry.
            for (int i = 0; i < m_; i++)
                for (int j = i + 1; j < m_; j++)
                    mat_X_XT_[i][j] = mat_X_XT_[j][i];

            // Calculate inverse matrix (X * X^T)^-1.
            mat_X_XT_inv_ = invert_sym_matrix_boost(mat_X_XT_);

            // Calculate vector X^T * y.
            vec_XT_y_.resize(m_);
            for (int i = 0; i < m_; i++)
                vec_XT_y_[i] = tapescript::sum_vec(x_power_[i] * data_y_);

            // Calculate regression coefficients.
            coef_.resize(m_);
            for (int i = 0; i < m_; i++)
            {
                coef_[i] = 0;
                for (int j = 0; j < m_; j++)
                    coef_[i] += mat_X_XT_inv_[i][j] * vec_XT_y_[j];
            }

            is_coef_calculated = true;
            return coef_;
        }

        // Calculate polynomial regression coefficients.
        tobject calculate_estimation()
        {
            if (!is_coef_calculated)
                throw std::runtime_error("Regression coefficients are not calculated.");
            tobject estim_y = tobject(0);
            for (int i = 0; i < m_; i++)
                estim_y += coef_[i] * x_power_[i];
            is_estim_calculated = true;
            return estim_y;
        }

        // Calculate polynomial regression coefficients derivatives w.r.t data points (y).
        std::vector<std::vector<double>> calculate_coefficients_derivatives()
        {
            if (!is_coef_calculated)
                throw std::runtime_error("Regression coefficients are not calculated.");
            std::vector<std::vector<double>> d_coef_d_y(m_);
            std::vector<tvalue> x_power_tvalue(m_);
            for (int i = 0; i < m_; i++)
                x_power_tvalue[i] = (tvalue)x_power_[i];
            int npoints = x_power_tvalue[0].size();
            for (int i = 0; i < m_; i++)
            {
                d_coef_d_y[i].resize(npoints);
                for (int j = 0; j < npoints; j++)
                    for (int k = 0; k < m_; k++)
                        d_coef_d_y[i][j] += ((tvalue)mat_X_XT_inv_[i][k]).scalar_value_ * x_power_tvalue[k].element_at(j);
            }
            return d_coef_d_y;
        }

        // Calculate polynomial regression coefficients derivatives w.r.t data points (y).
        std::vector<std::vector<double>> calculate_estimation_derivatives()
        {
            std::vector<std::vector<double>> d_coef_d_y = calculate_coefficients_derivatives();
            std::vector<tvalue> x_power_tvalue(m_);
            for (int i = 0; i < m_; i++)
                x_power_tvalue[i] = (tvalue)x_power_[i];
            int npoints = x_power_tvalue[0].size();
            std::vector<std::vector<double>> estim_y_deriv(npoints);
            for (int i = 0; i < npoints; i++)
            {
                estim_y_deriv[i].resize(npoints);
                for (int j = 0; j < npoints; j++)
                    for (int k = 0; k < m_; k++)
                        estim_y_deriv[i][j] += d_coef_d_y[k][i] * x_power_tvalue[k].element_at(j);
            }
            return estim_y_deriv;
        }

        // Invert symmetric matrix using Cholesky decomposition from the Boost library.
        // This template method works with both element_type = tobject and element_type = tdouble.
        template<typename element_type>
        static std::vector<std::vector<element_type>> invert_sym_matrix_boost(const std::vector<std::vector<element_type>>& mat)
        {
            // Matrix size.
            int m = mat.size();
            boost::numeric::ublas::matrix<element_type> input(m, m);
            for (int i = 0; i < m; i++)
                for (int j = 0; j < m; j++)
                    input.operator ()(i, j) = mat[i][j];
            // Create a permutation matrix for the LU-factorization
            boost::numeric::ublas::permutation_matrix<std::size_t> pm(m);
            // Perform LU-factorization
            if(boost::numeric::ublas::lu_factorize(input, pm))
                throw std::runtime_error("Singular matrix");
            // Create identity matrix of for inverse
            boost::numeric::ublas::matrix<element_type> inverse(m, m);
            inverse.assign(boost::numeric::ublas::identity_matrix<element_type>(m));
            boost::numeric::ublas::lu_substitute(input, pm, inverse);
            std::vector<std::vector<element_type>> mat_inv = mat;
            for (int i = 0; i < m; i++)
                for (int j = 0; j < m; j++)
                    mat_inv[i][j] = inverse(i, j);
            return mat_inv;
        }
    };
}

#endif // cl_tape_examples_impl_polynomial_regression_hpp
