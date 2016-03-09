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

#ifndef cl_tape_examples_impl_polynomial_regression_nonoptimized_hpp
#define cl_tape_examples_impl_polynomial_regression_nonoptimized_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include "polynomial_regression.hpp"

namespace cl
{
    // Class for polynomial regression calculation (non-optimized).
    class polynomial_regression_nonoptimized
    {
    private:
        // Order of polynomial regression.
        const int order_;
        // Size of matrixes m = order + 1.
        const int m_;

        // Input data vectors.
        const std::vector<tdouble> data_x_;
        const std::vector<tdouble> data_y_;

        // Intermediate regression struuctures:
        // Vectors of powers of x from 0 to 2 * (order + 1).
        std::vector<std::vector<tdouble>> x_power_;
        // Matrix X * X^T
        std::vector<std::vector<tdouble>> mat_X_XT_;
        // Inverse matrix (X * X^T)^-1
        std::vector<std::vector<tdouble>> mat_X_XT_inv_;
        // Vector X^T * y.
        std::vector<tdouble> vec_XT_y_;

        // Regression coefficients and their derivatives.
        std::vector<tdouble> coef_;

        // Status of calculation.
        bool is_coef_calculated;
        bool is_estim_calculated;

    public:
        // Construct using provided data and order of polynomial regression.
        polynomial_regression_nonoptimized(const std::vector<tdouble>& x, const std::vector<tdouble>& y, int order) : data_x_(x), data_y_(y), order_(order), m_(order + 1)
        {
            is_coef_calculated = false;
            is_estim_calculated = false;
        }

        // Calculate polynomial regression coefficients.
        std::vector<tdouble> calculate_coefficients()
        {
            // Number of data points.
            int npoints = data_x_.size();
            // Create vectors of powers of x from 0 to 2m.
            x_power_.resize(2 * m_);
            std::vector<tdouble> x_power_sum_vec(2 * m_);
            for (int i = 0; i < 2 * m_; i++)
            {
                x_power_[i].resize(npoints);
                x_power_sum_vec[i] = 0;
                for (int j = 0; j < npoints; j++)
                {
                    x_power_[i][j] = std::pow(data_x_[j], i);
                    x_power_sum_vec[i] += x_power_[i][j];
                }
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
            mat_X_XT_inv_ = polynomial_regression::invert_sym_matrix_boost(mat_X_XT_);

            // Calculate vector X^T * y.
            vec_XT_y_.resize(m_);
            for (int i = 0; i < m_; i++)
                for (int j = 0; j < npoints; j++)
                    vec_XT_y_[i] += x_power_[i][j] * data_y_[j];

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
        std::vector<tdouble> calculate_estimation()
        {
            if (!is_coef_calculated)
                throw_("Regression coefficients are not calculated.");
            // Number of data points.
            int npoints = data_x_.size();
            std::vector<tdouble> estim_y(npoints);
            for (int i = 0; i < m_; i++)
                for (int j = 0; j < npoints; j++)
                    estim_y[j] += coef_[i] * x_power_[i][j];
            is_estim_calculated = true;
            return estim_y;
        }

        // Calculate polynomial regression coefficients derivatives w.r.t data points (y).
        std::vector<std::vector<double>> calculate_coefficients_derivatives()
        {
            if (!is_coef_calculated)
                throw_("Regression coefficients are not calculated.");
            std::vector<std::vector<double>> d_coef_d_y(m_);
            // Number of data points.
            int npoints = data_x_.size();
            for (int i = 0; i < m_; i++)
            {
                d_coef_d_y[i].resize(npoints);
                for (int j = 0; j < npoints; j++)
                    for (int k = 0; k < m_; k++)
                        d_coef_d_y[i][j] += (double)(mat_X_XT_inv_[i][k] * x_power_[k][j]);
            }
            return d_coef_d_y;
        }

        // Calculate polynomial regression coefficients derivatives w.r.t data points (y).
        std::vector<std::vector<double>> calculate_estimation_derivatives()
        {
            std::vector<std::vector<double>> d_coef_d_y = calculate_coefficients_derivatives();
            // Number of data points.
            int npoints = data_x_.size();
            std::vector<std::vector<double>> estim_y_deriv(npoints);
            for (int i = 0; i < npoints; i++)
            {
                estim_y_deriv[i].resize(npoints);
                for (int j = 0; j < npoints; j++)
                    for (int k = 0; k < m_; k++)
                        estim_y_deriv[i][j] += (double)(d_coef_d_y[k][i] * x_power_[k][j]);
            }
            return estim_y_deriv;
        }
    };
}

#endif // cl_tape_examples_impl_polynomial_regression_hpp
