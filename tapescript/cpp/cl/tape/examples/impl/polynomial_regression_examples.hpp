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

#ifndef cl_tape_examples_impl_polynomial_regression_examples_hpp
#define cl_tape_examples_impl_polynomial_regression_examples_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include "polynomial_regression.hpp"
#include "polynomial_regression_nonoptimized.hpp"
#include "polynomial_regression_data.hpp"

namespace cl
{
    // Check two numbers with tolerance.
    bool check_tol(double a, double b, double tol)
    {
        double abs_diff = std::abs(a - b);
        if (abs_diff < tol)
            return true;
        double rel_diff = abs_diff / std::abs(a);
        if (rel_diff < tol)
            return true;
        return false;
    }

    // Check adjoint forward derivatives vs. analytical.
    template<typename element_type>
    std::vector<element_type> check_forward_derivatives(const std::vector<element_type>& dx, const std::vector<element_type>& adjoint, const std::vector<std::vector<double>>& coef_deriv_analyt, const std::vector<std::vector<double>>& estim_deriv_analyt, const double tol);

    // tvalue specification of the method defined above.
    template<>
    std::vector<tvalue> check_forward_derivatives(const std::vector<tvalue>& dx, const std::vector<tvalue>& adjoint, const std::vector<std::vector<double>>& coef_deriv_analyt, const std::vector<std::vector<double>>& estim_deriv_analyt, const double tol)
    {
        int order = coef_deriv_analyt.size() - 1;
        int npar = coef_deriv_analyt[0].size();
        int npoints = estim_deriv_analyt.size();
        std::vector<tvalue> analyt = adjoint;
        for (int i = 0; i < order + 1; i++)
        {
            double adjoint_value = adjoint[i].scalar_value_;
            double analyt_value = 0;
            for (int j = 0; j < npar; j++)
                analyt_value += coef_deriv_analyt[i][j] * dx[j].scalar_value_;
            if (!check_tol(analyt_value, adjoint_value, tol))
                throw_("Adjoint derivative differs from expectation.");
            analyt[i] = analyt_value;
        }
        for (int i = 0; i < npoints; i++)
        {
            double adjoint_value = adjoint[order + 1].element_at(i);
            double analyt_value = 0;
            for (int j = 0; j < npar; j++)
                analyt_value += estim_deriv_analyt[i][j] * dx[j].scalar_value_;
            if (!check_tol(analyt_value, adjoint_value, tol))
                throw_("Adjoint derivative differs from expectation.");
            analyt[order + 1][i] = analyt_value;
        }
        return analyt;
    }

    // double specification of the method defined above.
    template<>
    std::vector<double> check_forward_derivatives(const std::vector<double>& dx, const std::vector<double>& adjoint, const std::vector<std::vector<double>>& coef_deriv_analyt, const std::vector<std::vector<double>>& estim_deriv_analyt, const double tol)
    {
        int order = coef_deriv_analyt.size() - 1;
        int npar = coef_deriv_analyt[0].size();
        int npoints = estim_deriv_analyt.size();
        std::vector<double> analyt = adjoint;
        for (int i = 0; i < order + 1; i++)
        {
            double adjoint_value = adjoint[i];
            double analyt_value = 0;
            for (int j = 0; j < npar; j++)
                analyt_value += coef_deriv_analyt[i][j] * dx[j];
            if (!check_tol(analyt_value, adjoint_value, tol))
                throw_("Adjoint derivative differs from expectation.");
            analyt[i] = analyt_value;
        }
        for (int i = order + 1; i < order + 1 + npoints; i++)
        {
            double adjoint_value = adjoint[i];
            double analyt_value = 0;
            for (int j = 0; j < npar; j++)
                analyt_value += estim_deriv_analyt[i - order - 1][j] * dx[j];
            if (!check_tol(analyt_value, adjoint_value, tol))
                throw_("Adjoint derivative differs from expectation.");
            analyt[i] = analyt_value;
        }
        return analyt;
    }

    // Check adjoint reversed derivatives vs. analytical.
    template<typename element_type>
    std::vector<element_type> check_reversed_derivatives(const std::vector<element_type>& dx, const std::vector<element_type>& adjoint, const std::vector<std::vector<double>>& coef_deriv_analyt, const std::vector<std::vector<double>>& estim_deriv_analyt, const double tol);

    // tvalue specification of the method defined above.
    template<>
    std::vector<tvalue> check_reversed_derivatives(const std::vector<tvalue>& dx, const std::vector<tvalue>& adjoint, const std::vector<std::vector<double>>& coef_deriv_analyt, const std::vector<std::vector<double>>& estim_deriv_analyt, const double tol)
    {
        int order = coef_deriv_analyt.size() - 1;
        int npar = coef_deriv_analyt[0].size();
        int npoints = estim_deriv_analyt.size();
        std::vector<tvalue> analyt = adjoint;
        for (int i = 0; i < npar; i++)
        {
            double adjoint_value = adjoint[i].scalar_value_;
            double analyt_value = 0;
            for (int j = 0; j <= order; j++)
                analyt_value += coef_deriv_analyt[j][i] * dx[j].scalar_value_;
            for (int j = 0; j < npoints; j++)
                analyt_value += estim_deriv_analyt[j][i] * dx[order + 1].element_at(j);
            if (!check_tol(analyt_value, adjoint_value, tol))
                throw_("Adjoint derivative for differs from expectation.");
            analyt[i] = analyt_value;
        }
        return analyt;
    }

    // double specification of the method defined above.
    template<>
    std::vector<double> check_reversed_derivatives(const std::vector<double>& dx, const std::vector<double>& adjoint, const std::vector<std::vector<double>>& coef_deriv_analyt, const std::vector<std::vector<double>>& estim_deriv_analyt, const double tol)
    {
        int order = coef_deriv_analyt.size() - 1;
        int npar = coef_deriv_analyt[0].size();
        int npoints = estim_deriv_analyt.size();
        std::vector<double> analyt = adjoint;
        for (int i = 0; i < npar; i++)
        {
            double adjoint_value = adjoint[i];
            double analyt_value = 0;
            for (int j = 0; j <= order; j++)
                analyt_value += coef_deriv_analyt[j][i] * dx[j];
            for (int j = order + 1; j < order + 1 + npoints; j++)
                analyt_value += estim_deriv_analyt[j - order - 1][i] * dx[j];
            if (!check_tol(analyt_value, adjoint_value, tol))
                throw_("Adjoint derivative for differs from expectation.");
            analyt[i] = analyt_value;
        }
        return analyt;
    }

    // Example of polynomial regression differentiation with respect to parameters of input distribution using optimized tape.
    inline void polynomial_regression_with_params_optimized_example(const polynomial_regression_data& data, std::ostream& out_stream = std::cout)
    {
        // Control production of tape output to serializer.
        bool flag_serializer = data.GetFlagSerializer();

        // Control analytical check of derivatives.
        bool flag_check = data.GetFlagCheck();
        
        // Order of polynomial regression.
        int order = data.GetOrder();

        // Number of points.
        int n = data.GetPointsNumber();

        out_stream << "Polynomial regression (order " << order << ") differentiation with respect to input parameters (optimized tape):\n" << std::endl;

        // Input values initialization.
        out_stream << "Input vector size: n = " << n << std::endl;
        std::vector<double> pars_polynom = data.GetPolynomParameters();
        double par_exponent = data.GetExponentParameter();
        int npar = pars_polynom.size() + 1;
        std::vector<tobject> X(npar);
        for (int i = 0; i < npar - 1; i++)
            X[i] = pars_polynom[i];
        X[npar - 1] = par_exponent;
        if (flag_serializer)
            out_stream << "Independent vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        std::clock_t tape_time = std::clock();
        tape_start(X);

        // Create vector of x_i values.
        tobject x = data.CreateEquidistantXVector<tobject>();
        // Create vector of y_i values.
        tobject y = polynomial_regression_data::FunctionPolynomPlusExp(x, X);
        if (flag_serializer)
        {
            out_stream << "Input data x: " << x << "\n";
            out_stream << "Input data y: " << y << "\n";
        }

        // Polynomial regression calculation.
        polynomial_regression regression(x, y, order);
        std::vector<tobject> coef = regression.calculate_coefficients();
        tobject estim = regression.calculate_estimation();

        // Create output vector.
        std::vector<tobject> Y(order + 2);
        for (int i = 0; i <= order; i++)
            Y[i] = coef[i];
        Y[order + 1] = estim;
        if (flag_serializer)
            out_stream << "Output vector: " << Y << "\n\n";

        // Declare a tape function and stop the tape recording.
        if (flag_serializer)
            out_stream << "Initial Forward(0) sweep...\n";
        tfunc<tvalue> f(X, Y);
        tape_time = std::clock() - tape_time;
        out_stream << "Tape memory (bytes): " << f.Memory() << std::endl;
        out_stream << "Tape creation took (ms): " << tape_time / (double)(CLOCKS_PER_SEC) * 1000 << '\n';
        
        //  Calculate analytical derivatives if needed.
        std::vector<std::vector<double>> coef_deriv_analyt;
        std::vector<std::vector<double>> estim_deriv_analyt;
        if (flag_check)
        {
            // Derivatives of output quantities w.r.t data values.
            std::vector<std::vector<double>> d_coef_d_y = regression.calculate_coefficients_derivatives();
            std::vector<std::vector<double>> d_estim_d_y = regression.calculate_estimation_derivatives();
            int npoints = d_coef_d_y[0].size();

            // Derivatives of data values w.r.t input parameters.
            std::vector<std::vector<double>> d_y_d_par(npar);
            for (int i = 0; i < npar; i++)
            {
                d_y_d_par[i].resize(npoints);
                tvalue d_y_d_par_tvalue = (tvalue)polynomial_regression_data::FunctionPolynomPlusExp(x, X, i);
                for (int j = 0; j < npoints; j++)
                    d_y_d_par[i][j] = d_y_d_par_tvalue.element_at(j);
            }

            // Derivatives of cofficients w.r.t input parameters.
            coef_deriv_analyt.resize(order + 1);
            for (int i = 0; i <= order; i++)
                coef_deriv_analyt[i].resize(npar);

            // Derivatives of estimations w.r.t input parameters.
            estim_deriv_analyt.resize(npoints);
            for (int i = 0; i < npoints; i++)
                estim_deriv_analyt[i].resize(npar);

            // Convolution.
            for (int i = 0; i < npar; i++)
            {
                for (int j = 0; j < npoints; j++)
                {
                    for (int k = 0; k <= order; k++)
                        coef_deriv_analyt[k][i] += d_coef_d_y[k][j] * d_y_d_par[i][j];
                    for (int k = 0; k < npoints; k++)
                        estim_deriv_analyt[k][i] += d_estim_d_y[k][j] * d_y_d_par[i][j];
                }
            }
        }

        // Forward derivatives calculation.
        if (data.GetFlagForward())
        {
            std::vector<tvalue> forward;
            std::vector<tvalue> dx(X.size());
            // Forward derivative calculation time.
            std::clock_t forward_time = 0;
            // Loop over input parameters.
            for (int i = 0; i < X.size(); i++)
            {
                for (int j = 0; j < X.size(); j++)
                    dx[j] = 0;
                dx[i] = 1;
                if (flag_serializer)
                    out_stream << std::endl << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
                forward_time -= std::clock();
                flag_serializer ? forward = f.forward(1, dx, out_stream) : forward = f.forward(1, dx);
                forward_time += std::clock();
                if (flag_serializer)
                    out_stream << "Forward derivatives = " << forward << "..." << std::endl;

                //  Check derivatives analytically if needed.
                if (flag_check)
                {
                    std::vector<tvalue> analyt = check_forward_derivatives(dx, forward, coef_deriv_analyt, estim_deriv_analyt, data.GetTolerance());
                    if (flag_serializer)
                        out_stream << "Analytic derivatives = " << analyt << "..." << std::endl;
                }
            }
            out_stream << std::endl << "Forward calculation took (ms): " << forward_time / (double)(CLOCKS_PER_SEC)* 1000 << '\n';
        }

        // Reverse derivatives calculation.
        if (data.GetFlagReverse())
        {
            std::vector<tvalue> reversed;
            std::vector<tvalue> dx(order + 2);
            dx[order + 1].resize(n);
            // Forward derivative calculation time.
            std::clock_t reversed_time = 0;
            // Loop over coefficients.
            for (int i = 0; i <= order; i++)
            {
                for (int j = 0; j <= order; j++)
                    dx[j] = 0;
                dx[i] = 1;
                if (flag_serializer)
                    out_stream << std::endl << "Reversed(1, dx) sweep for dx = " << dx << "..." << std::endl;
                reversed_time -= std::clock();
                flag_serializer ? reversed = f.reverse(1, dx, out_stream) : reversed = f.reverse(1, dx);
                reversed_time += std::clock();
                if (flag_serializer)
                    out_stream << "Reversed derivatives = " << reversed << "..." << std::endl;

                //  Check derivatives analytically if needed.
                if (flag_check)
                {
                    std::vector<tvalue> analyt = check_reversed_derivatives(dx, reversed, coef_deriv_analyt, estim_deriv_analyt, data.GetTolerance());
                    if (flag_serializer)
                        out_stream << "Analytic derivatives = " << analyt << "..." << std::endl;
                }
            }
            dx[order] = 0;
            // Loop over estimations.
            if (data.GetFlagReverseAll())
            {
                for (int i = 0; i < n; i++)
                {
                    for (int j = 0; j < n; j++)
                        dx[order + 1][j] = 0;
                    dx[order + 1][i] = 1;
                    if (flag_serializer)
                        out_stream << std::endl << "Reversed(1, dx) sweep for dx = " << dx << "..." << std::endl;
                    reversed_time -= std::clock();
                    flag_serializer ? reversed = f.reverse(1, dx, out_stream) : reversed = f.reverse(1, dx);
                    reversed_time += std::clock();
                    if (flag_serializer)
                        out_stream << "Reversed derivatives = " << reversed << "..." << std::endl;

                    //  Check derivatives analytically if needed.
                    if (flag_check)
                    {
                        std::vector<tvalue> analyt = check_reversed_derivatives(dx, reversed, coef_deriv_analyt, estim_deriv_analyt, data.GetTolerance());
                        if (flag_serializer)
                            out_stream << "Analytic derivatives = " << analyt << "..." << std::endl;
                    }
                }
            }
            out_stream << std::endl << "Reversed calculation took (ms): " << reversed_time / (double)(CLOCKS_PER_SEC)* 1000 << '\n';
        }

        out_stream << std::endl << "All derivatives calculated successfully." << std::endl;
        out_stream << std::endl;
    }

    // Example of polynomial regression differentiation with respect to parameters of input distribution using non-optimized tape.
    inline void polynomial_regression_with_params_nonoptimized_example(const polynomial_regression_data& data, std::ostream& out_stream = std::cout)
    {
        // Control production of tape output to serializer.
        bool flag_serializer = data.GetFlagSerializer();

        // Control analytical check of derivatives.
        bool flag_check = data.GetFlagCheck();

        // Order of polynomial regression.
        int order = data.GetOrder();

        // Number of points.
        int n = data.GetPointsNumber();

        out_stream << "Polynomial regression (order " << order << ") differentiation with respect to input parameters (non-optimized tape):\n" << std::endl;

        // Input values initialization.
        out_stream << "Input vector size: n = " << n << std::endl;
        std::vector<double> pars_polynom = data.GetPolynomParameters();
        double par_exponent = data.GetExponentParameter();
        int npar = pars_polynom.size() + 1;
        std::vector<tdouble> X(npar);
        for (int i = 0; i < npar - 1; i++)
            X[i] = pars_polynom[i];
        X[npar - 1] = par_exponent;
        if (flag_serializer)
            out_stream << "Independent vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        std::clock_t tape_time = std::clock();
        tape_start(X);

        // Create vector of x_i values.
        std::vector<tdouble> x = data.CreateEquidistantXVector<std::vector<tdouble>>();
        // Create vector of y_i values.
        std::vector<std::vector<tdouble>> X_vector(npar);
        for (int i = 0; i < npar; i++)
            X_vector[i] = { X[i] };
        std::vector<tdouble> y = polynomial_regression_data::FunctionPolynomPlusExp(x, X_vector);
        if (flag_serializer)
        {
            out_stream << "Input data x: " << x << "\n";
            out_stream << "Input data y: " << y << "\n";
        }

        // Polynomial regression calculation.
        polynomial_regression_nonoptimized regression(x, y, order);
        std::vector<tdouble> coef = regression.calculate_coefficients();
        std::vector<tdouble> estim = regression.calculate_estimation();

        // Create output vector.
        std::vector<tdouble> Y(order + 1 + n);
        for (int i = 0; i <= order; i++)
            Y[i] = coef[i];
        for (int i = order + 1; i < order + 1 + n; i++)
            Y[i] = estim[i - order - 1];
        if (flag_serializer)
            out_stream << "Output vector: " << Y << "\n\n";

        // Declare a tape function and stop the tape recording.
        if (flag_serializer)
            out_stream << "Initial Forward(0) sweep...\n";
        tfunc<double> f(X, Y);
        tape_time = std::clock() - tape_time;
        out_stream << "Tape memory (bytes): " << f.Memory() << std::endl;
        out_stream << "Tape creation took (ms): " << tape_time / (double)(CLOCKS_PER_SEC)* 1000 << '\n';
        //  Calculate analytical derivatives if needed.
        std::vector<std::vector<double>> coef_deriv_analyt;
        std::vector<std::vector<double>> estim_deriv_analyt;
        if (flag_check)
        {
            // Derivatives of output quantities w.r.t data values.
            std::vector<std::vector<double>> d_coef_d_y = regression.calculate_coefficients_derivatives();
            std::vector<std::vector<double>> d_estim_d_y = regression.calculate_estimation_derivatives();
            int npoints = d_coef_d_y[0].size();

            // Derivatives of data values w.r.t input parameters.
            std::vector<std::vector<double>> d_y_d_par(npar);
            for (int i = 0; i < npar; i++)
            {
                d_y_d_par[i].resize(npoints);
                std::vector<tdouble> d_y_d_par_tvalue = polynomial_regression_data::FunctionPolynomPlusExp(x, X_vector, i);
                for (int j = 0; j < npoints; j++)
                    d_y_d_par[i][j] = (double)d_y_d_par_tvalue[j];
            }

            // Derivatives of cofficients w.r.t input parameters.
            coef_deriv_analyt.resize(order + 1);
            for (int i = 0; i <= order; i++)
                coef_deriv_analyt[i].resize(npar);

            // Derivatives of estimations w.r.t input parameters.
            estim_deriv_analyt.resize(npoints);
            for (int i = 0; i < npoints; i++)
                estim_deriv_analyt[i].resize(npar);

            // Convolution.
            for (int i = 0; i < npar; i++)
            {
                for (int j = 0; j < npoints; j++)
                {
                    for (int k = 0; k <= order; k++)
                        coef_deriv_analyt[k][i] += d_coef_d_y[k][j] * d_y_d_par[i][j];
                    for (int k = 0; k < npoints; k++)
                        estim_deriv_analyt[k][i] += d_estim_d_y[k][j] * d_y_d_par[i][j];
                }
            }
        }

        // Forward derivatives calculation.
        if (data.GetFlagForward())
        {
            std::vector<double> forward;
            std::vector<double> dx(X.size());
            // Forward derivative calculation time.
            std::clock_t forward_time = 0;
            // Loop over input parameters.
            for (int i = 0; i < X.size(); i++)
            {
                for (int j = 0; j < X.size(); j++)
                    dx[j] = 0;
                dx[i] = 1;
                if (flag_serializer)
                    out_stream << std::endl << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
                forward_time -= std::clock();
                flag_serializer ? forward = f.forward(1, dx, out_stream) : forward = f.forward(1, dx);
                forward_time += std::clock();
                if (flag_serializer)
                    out_stream << "Forward derivatives = " << forward << "..." << std::endl;

                //  Check derivatives analytically if needed.
                if (flag_check)
                {
                    std::vector<double> analyt = check_forward_derivatives(dx, forward, coef_deriv_analyt, estim_deriv_analyt, data.GetTolerance());
                    if (flag_serializer)
                        out_stream << "Analytic derivatives = " << analyt << "..." << std::endl;
                }
            }
            out_stream << std::endl << "Forward calculation took (ms): " << forward_time / (double)(CLOCKS_PER_SEC) * 1000 << '\n';
        }

        // Reverse derivatives calculation.
        if (data.GetFlagReverse())
        {
            std::vector<double> reversed;
            std::vector<double> dx(Y.size());
            // Forward derivative calculation time.
            std::clock_t reversed_time = 0;
            // Loop over coefficients.
            for (int i = 0; i <= order; i++)
            {
                for (int j = 0; j <= order; j++)
                    dx[j] = 0;
                dx[i] = 1;
                if (flag_serializer)
                    out_stream << std::endl << "Reversed(1, dx) sweep for dx = " << dx << "..." << std::endl;
                reversed_time -= std::clock();
                flag_serializer ? reversed = f.reverse(1, dx, out_stream) : reversed = f.reverse(1, dx);
                reversed_time += std::clock();
                if (flag_serializer)
                    out_stream << "Reversed derivatives = " << reversed << "..." << std::endl;

                //  Check derivatives analytically if needed.
                if (flag_check)
                {
                    std::vector<double> analyt = check_reversed_derivatives(dx, reversed, coef_deriv_analyt, estim_deriv_analyt, data.GetTolerance());
                    if (flag_serializer)
                        out_stream << "Analytic derivatives = " << analyt << "..." << std::endl;
                }
            }
            dx[order] = 0;
            // Loop over estimations.
            if (data.GetFlagReverseAll())
            {
                for (int i = 0; i < n; i++)
                {
                    for (int j = 0; j < n; j++)
                        dx[order + 1 + j] = 0;
                    dx[order + 1 + i] = 1;
                    if (flag_serializer)
                        out_stream << std::endl << "Reversed(1, dx) sweep for dx = " << dx << "..." << std::endl;
                    reversed_time -= std::clock();
                    flag_serializer ? reversed = f.reverse(1, dx, out_stream) : reversed = f.reverse(1, dx);
                    reversed_time += std::clock();
                    if (flag_serializer)
                        out_stream << "Reversed derivatives = " << reversed << "..." << std::endl;

                    //  Check derivatives analytically if needed.
                    if (flag_check)
                    {
                        std::vector<double> analyt = check_reversed_derivatives(dx, reversed, coef_deriv_analyt, estim_deriv_analyt, data.GetTolerance());
                        if (flag_serializer)
                            out_stream << "Analytic derivatives = " << analyt << "..." << std::endl;
                    }
                }
            }
            out_stream << std::endl << "Reversed calculation took (ms): " << reversed_time / (double)(CLOCKS_PER_SEC) * 1000 << '\n';
        }

        out_stream << std::endl << "All derivatives calculated successfully." << std::endl;
        out_stream << std::endl;
    }
    
    inline void polynomial_regression_examples()
    {
        std::ofstream of("output/performance/polynomial_regression_output.txt");
        tape_serializer<tvalue> serializer(of);
        serializer.precision(3);

        // Order 1.
        polynomial_regression_data data_1(1);
        polynomial_regression_with_params_optimized_example(data_1, serializer);
        polynomial_regression_with_params_nonoptimized_example(data_1, serializer);

        // Order 2.
        polynomial_regression_data data_2(2);
        polynomial_regression_with_params_optimized_example(data_2, serializer);
        polynomial_regression_with_params_nonoptimized_example(data_2, serializer);

        // Order 3.
        polynomial_regression_data data_3(3);
        polynomial_regression_with_params_optimized_example(data_3, serializer);
        polynomial_regression_with_params_nonoptimized_example(data_3, serializer);

        // Order 4.
        polynomial_regression_data data_4(4);
        polynomial_regression_with_params_optimized_example(data_4, serializer);
        polynomial_regression_with_params_nonoptimized_example(data_4, serializer);
    }
}

#endif // cl_tape_examples_impl_polynomial_regression_examples_hpp
