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

#ifndef cl_tape_examples_impl_linear_regression_hpp
#define cl_tape_examples_impl_linear_regression_hpp

#include <cmath>
#include <valarray>

namespace cl
{
    // Class for linear regression calculation.
    class LinearRegression
    {
    private:
        // Size of input data vectors.
        const size_t n_;
        // Parameters for generated y input values: y_i = a + b * x_i + exp(-c * x_i).
        const double a_, b_, c_;
        // Arrays of generated input data.
        tarray x_input_, y_input_;
        // Estimation values for y.
        tarray y_estim_;
        // Derivatives w.r.t to input x, y arrays.
        tarray d_alpha_d_x_input_;
        tarray d_alpha_d_y_input_;
        tarray d_beta_d_x_input_;
        tarray d_beta_d_y_input_;
        std::valarray<tarray> d_y_estim_d_x_input_;
        std::valarray<tarray> d_y_estim_d_y_input_;
        // Derivatives w.r.t to a, b, c parameters.
        double d_alpha_d_a_;
        double d_alpha_d_b_;
        double d_alpha_d_c_;
        double d_beta_d_a_;
        double d_beta_d_b_;
        double d_beta_d_c_;
        tarray d_y_estim_d_a_;
        tarray d_y_estim_d_b_;
        tarray d_y_estim_d_c_;

    public:
        LinearRegression(int n, double a, double b, double c) : n_(n), a_(a), b_(b), c_(c)
        {
            x_input_.resize(n_);
            y_input_.resize(n_);
            for (int i = 0; i < n_; i++)
            {
                x_input_[i] = i;
                y_input_[i] = a_ + b_ * x_input_[i] + exp(-1 * c_ * x_input_[i]);
            }
        }

        tarray GetInputX()
        {
            return x_input_;
        }

        tarray GetInputY()
        {
            return y_input_;
        }

        void Calculate()
        {
            // Calculate helper variables.
            double one_over_n_ = 1.0 / n_;
            double mean_x = Mean(x_input_);
            double mean_y = Mean(y_input_);
            double covar_xx = Covariance(x_input_, x_input_);
            double covar_xy = Covariance(x_input_, y_input_);
            double beta = covar_xy / covar_xx;
            double alpha = mean_y - beta * mean_x;

            // Calculate estimated y values.
            y_estim_.resize(n_);
            for (int i = 0; i < n_; i++)
                y_estim_[i] = alpha + beta * x_input_[i];

            // Calculate derivatives for mean values.
            double d_mean_x_d_x_input_ = 1.0 / n_;
            double d_mean_y_d_y_input_ = 1.0 / n_;
            double d_mean_x_d_y_input_ = 0.0;
            double d_mean_y_d_x_input_ = 0.0;

            // Calculate derivatives for alpha and beta.
            d_alpha_d_x_input_.resize(n_);
            d_alpha_d_y_input_.resize(n_);
            d_beta_d_x_input_.resize(n_);
            d_beta_d_y_input_.resize(n_);
            for (int i = 0; i < n_; i++)
            {
                double d_covar_xx_d_x_input = 0.0;
                double d_covar_xx_d_y_input = 0.0;
                double d_covar_xy_d_x_input = 0.0;
                double d_covar_xy_d_y_input = 0.0;
                for (int j = 0; j < n_; j++)
                {
                    double kronecker = (i == j) ? 1.0 : 0.0;
                    d_covar_xx_d_x_input += 2 * (x_input_[j] - mean_x) * (kronecker - one_over_n_);
                    d_covar_xy_d_x_input += (y_input_[j] - mean_y) * (kronecker - one_over_n_);
                    d_covar_xy_d_y_input += (x_input_[j] - mean_x) * (kronecker - one_over_n_);
                }

                // Calculate derivatives for beta.
                d_beta_d_x_input_[i] = (d_covar_xy_d_x_input * covar_xx - d_covar_xx_d_x_input * covar_xy) / pow(covar_xx, 2.0);
                d_beta_d_y_input_[i] = (d_covar_xy_d_y_input * covar_xx - d_covar_xx_d_y_input * covar_xy) / pow(covar_xx, 2.0);

                // Calcualte derivatives for alpha.
                d_alpha_d_x_input_[i] = d_mean_y_d_x_input_ - d_mean_x_d_x_input_ * beta - mean_x * d_beta_d_x_input_[i];
                d_alpha_d_y_input_[i] = d_mean_y_d_y_input_ - mean_x * d_beta_d_y_input_[i];
            }

            // Calculate derivatives for y estimations.
            d_y_estim_d_x_input_.resize(n_);
            d_y_estim_d_y_input_.resize(n_);
            for (int i = 0; i < n_; i++)
            {
                d_y_estim_d_x_input_[i].resize(n_);
                d_y_estim_d_y_input_[i].resize(n_);
                for (int j = 0; j < n_; j++)
                {
                    double kronecker = (i == j) ? 1.0 : 0.0;
                    d_y_estim_d_x_input_[i][j] = d_alpha_d_x_input_[j] + kronecker * beta + d_beta_d_x_input_[j] * x_input_[i];
                    d_y_estim_d_y_input_[i][j] = d_alpha_d_y_input_[j] + d_beta_d_y_input_[j] * x_input_[i];
                }
            }

            // Calculate derivatives w.r.t input parameters a, b, c.
            // Derivatives for beta.
            d_beta_d_a_ = 0.0;
            d_beta_d_b_ = 1.0;
            double d_cov_xx_d_c = 0.0;
            double d_cov_xy_d_c = 0.0;
            for (int i = 0; i < n_; i++)
                d_cov_xy_d_c += (x_input_[i] - mean_x) * (-1 * x_input_[i] * exp(-1 * c_ * x_input_[i]));
            d_beta_d_c_ = (d_cov_xy_d_c * covar_xx - d_cov_xx_d_c * covar_xy) / pow(covar_xx, 2.0);
            // Derivatives for alpha.
            d_alpha_d_a_ = 1.0;
            d_alpha_d_b_ = 0.0;
            double d_mean_y_d_c = 0.0;
            for (int i = 0; i < n_; i++)
                d_mean_y_d_c += -1 * x_input_[i] * exp(-1 * c_ * x_input_[i]);
            d_mean_y_d_c /= n_;
            d_alpha_d_c_ = d_mean_y_d_c - d_beta_d_c_ * mean_x;
            // Derivatives for y estimation.
            d_y_estim_d_a_.resize(n_);
            d_y_estim_d_b_.resize(n_);
            d_y_estim_d_c_.resize(n_);
            for (int i = 0; i < n_; i++)
            {
                d_y_estim_d_a_[i] = d_alpha_d_a_ + d_beta_d_a_ * x_input_[i];
                d_y_estim_d_b_[i] = d_alpha_d_b_ + d_beta_d_b_ * x_input_[i];
                d_y_estim_d_c_[i] = d_alpha_d_c_ + d_beta_d_c_ * x_input_[i];
            }
        }

        // Number of points.
        size_t GetN()
        {
            return n_;
        }

        // d(alpha) / d(x_i)
        double GetDerivate_dAlpha_dX(int i)
        {
            return d_alpha_d_x_input_[i];
        }

        // d(alpha) / d(y_i)
        double GetDerivate_dAlpha_dY(int i)
        {
            return d_alpha_d_y_input_[i];
        }

        // d(alpha) / d(a)
        double GetDerivate_dAlpha_dA()
        {
            return d_alpha_d_a_;
        }

        // d(alpha) / d(b)
        double GetDerivate_dAlpha_dB()
        {
            return d_alpha_d_b_;
        }

        // d(alpha) / d(c)
        double GetDerivate_dAlpha_dC()
        {
            return d_alpha_d_c_;
        }

        // d(beta) / d(x_i)
        double GetDerivate_dBeta_dX(int i)
        {
            return d_beta_d_x_input_[i];
        }

        // d(beta) / d(y_i)
        double GetDerivate_dBeta_dY(int i)
        {
            return d_beta_d_y_input_[i];
        }

        // d(beta) / d(a)
        double GetDerivate_dBeta_dA()
        {
            return d_beta_d_a_;
        }

        // d(beta) / d(b)
        double GetDerivate_dBeta_dB()
        {
            return d_beta_d_b_;
        }

        // d(beta) / d(c)
        double GetDerivate_dBeta_dC()
        {
            return d_beta_d_c_;
        }

        // d(y_estim_i) / d(x_j)
        double GetDerivate_dYEstim_dX(int i, int j)
        {
            return d_y_estim_d_x_input_[i][j];
        }

        // d(y_estim_i) / d(y_j)
        double GetDerivate_dYEstim_dY(int i, int j)
        {
            return d_y_estim_d_y_input_[i][j];
        }

        // d(y_estim_i) / d(a)
        double GetDerivate_dYEstim_dA(int i)
        {
            return d_y_estim_d_a_[i];
        }

        // d(y_estim_i) / d(b)
        double GetDerivate_dYEstim_dB(int i)
        {
            return d_y_estim_d_b_[i];
        }

        // d(y_estim_i) / d(c)
        double GetDerivate_dYEstim_dC(int i)
        {
            return d_y_estim_d_c_[i];
        }

    private:
        // Calculate mean of values in array x.
        static double Mean(const tarray& x)
        {
            double mean = 0.0;
            for (int i = 0; i < x.size(); i++)
                mean += x[i];
            mean /= x.size();
            return mean;
        }

        // Calculate covariance cov(x, y).
        static double Covariance(const tarray& x, const tarray& y)
        {
            double mean_x = Mean(x);
            double mean_y = Mean(y);
            double cov = 0.0;
            for (int i = 0; i < x.size(); i++)
                cov += (x[i] - mean_x) * (y[i] - mean_y);
            return cov;
        }
    };
}

#endif // cl_tape_examples_impl_linear_regression_hpp
