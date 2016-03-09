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

#ifndef cl_tape_examples_impl_regression_examples_hpp
#define cl_tape_examples_impl_regression_examples_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include "linear_regression.hpp"
#include "regression_examples_impl.hpp"

namespace cl
{
    struct initial_data
    {
        // Control production of tape output to serializer.
        bool flag_serializer = false;

        // Input linear regression parameters.
        // n: number of points {x_i, y_i}.
        // x_i are x_0 = 0, x_1 = 1, ..., x_n = n.
        // y_i are y_i = a + b * x_i + exp(-1 * c * x_i).
#if defined NDEBUG
        size_t n = 5000;
#else
        size_t n = 2000;
#endif
        double a = 1.0;
        double b = 2.0;
        double c = 3.0;

        // Tolerance for analytical vs. adjoint derivative check.
        double eps = 1e-10;
    };

    // Example of linear regression differentiation with respect to parameters of input distribution using optimized tape.
    inline void linear_regression_with_params_optimized_example(const initial_data& data, std::ostream& out_stream = std::cout)
    {
        // Control production of tape output to serializer.
        bool flag_serializer = data.flag_serializer;

        // Input linear regression parameters.
        // n: number of points {x_i, y_i}.
        // x_i are x_0 = 0, x_1 = 1, ..., x_n = n.
        // y_i are y_i = a + b * x_i + exp(-1 * c * x_i).
        size_t n = data.n;
        double a = data.a;
        double b = data.b;
        double c = data.c;

        // Tolerance for analytical vs. adjoint derivative check.
        double eps = data.eps;

        out_stream << "Linear regression with parameters (optimized tape):\n" << std::endl;

        // Input values initialization.
        out_stream << "Input vector size: n = " << n << std::endl;
        LinearRegression lin_regr(n, a, b, c);
        lin_regr.Calculate();
        tvalue a_ref = a;
        tvalue b_ref = b;
        tvalue c_ref = c;
        std::vector<tobject> X = { a_ref, b_ref, c_ref };
        if (flag_serializer)
            out_stream << "Input vector: " << X << "\n";

        std::clock_t start_time = std::clock();
        // Declare the X vector as independent and start a tape recording.
        tape_start(X);

        // Output calculations.
        tobject& par_a = X[0];
        tobject& par_b = X[1];
        tobject& par_c = X[2];
        // Obtain x_i values.
        tobject x = (tvalue)lin_regr.GetInputX();
        // Calculate corresponding y_i values.
        tobject y = par_a + x * par_b + exp(-1 * par_c * x);
        // Start linear regression calculation: calculate mean values.
        tobject x_mean = 1.0 / n * tapescript::sum_vec(x);
        tobject y_mean = 1.0 / n * tapescript::sum_vec(y);
        // Variance times n: n * Var[x]
        tobject var_x_n = tapescript::sum_vec((x - x_mean) * (x - x_mean));
        // Covariance times n: n * Cov[x, y]
        tobject cov_xy_n = tapescript::sum_vec((x - x_mean) * (y - y_mean));
        // Linear regression coefficients.
        tobject beta = cov_xy_n / var_x_n;
        tobject alpha = y_mean - beta * x_mean;
        // Estimation for y_i.
        tobject y_estimate = alpha + beta * x;
        // Output vector.
        std::vector<tobject> Y = { alpha, beta, y_estimate };
        //out_stream << "Output vector: " << Y << "\n\n";

        if (flag_serializer)
            out_stream << "Initial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        tfunc<tvalue> f(X, Y);
        std::clock_t stop_time = std::clock();
        out_stream << "Tape memory (bytes): " << f.Memory() << std::endl;
        out_stream << "Tape creation took (ms): " << (stop_time - start_time) / (double)(CLOCKS_PER_SEC) * 1000 << '\n';

        // Forward sweep calculations.
        tarray d_ref_array;
        d_ref_array.resize(3);

        // Derivative calculation time.
        std::clock_t calc_time = 0;

        // Derivatives with respect to a.
        std::vector<tvalue> dx = { 1, 0, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        std::vector<tvalue> forw;
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;
        check_derivatives_da(lin_regr, forw, eps);

        // Derivatives with respect to b.
        dx = { 0, 1, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;
        check_derivatives_db(lin_regr, forw, eps);

        // Derivatives with respect to c.
        dx = { 0, 0, 1 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;
        check_derivatives_dc(lin_regr, forw, eps);

        out_stream << "Forward calculation took (ms): " << calc_time / (double)(CLOCKS_PER_SEC) * 1000 << '\n';
        out_stream << "All derivatives calculated successfully." << std::endl;
        out_stream << std::endl;
    }

    // Example of linear regression differentiation with respect to parameters of input distribution using non-optimized tape.
    inline void linear_regression_with_params_nonoptimized_example(const initial_data& data, std::ostream& out_stream = std::cout)
    {
        // Control production of tape output to serializer.
        bool flag_serializer = data.flag_serializer;

        // Input linear regression parameters.
        // n: number of points {x_i, y_i}.
        // x_i are x_0 = 0, x_1 = 1, ..., x_n = n.
        // y_i are y_i = a + b * x_i + exp(-1 * c * x_i).
        size_t n = data.n;
        double a = data.a;
        double b = data.b;
        double c = data.c;

        // Tolerance for analytical vs. adjoint derivative check.
        double eps = data.eps;

        out_stream << "Linear regression with parameters (non-optimized tape):\n" << std::endl;

        // Input values initialization.
        out_stream << "Input vector size: n = " << n << std::endl;
        LinearRegression lin_regr(n, a, b, c);
        lin_regr.Calculate();
        std::vector<tdouble> X = { a, b, c };
        if (flag_serializer)
            out_stream << "Input vector: " << X << "\n";

        std::clock_t start_time = std::clock();
        // Declare the X vector as independent and start a tape recording.
        tape_start(X);

        // Output calculations.
        std::vector<tdouble> x(n);
        // Obtain x_i values.
        tarray x_ref = lin_regr.GetInputX();
        for (int i = 0; i < n; i++)
            x[i] = x_ref[i];
        // Calculate corresponding y_i values.
        std::vector<tdouble> y(n);
        tdouble& par_a = X[0];
        tdouble& par_b = X[1];
        tdouble& par_c = X[2];
        for (int i = 0; i < n; i++)
            y[i] = par_a + x[i] * par_b + std::exp(-1 * par_c * x[i]);
        // Start linear regression calculation: calculate mean values.
        tdouble x_mean = 0.0;
        for (int i = 0; i < n; i++)
            x_mean += x[i];
        x_mean /= n;
        tdouble y_mean = 0.0;
        for (int i = 0; i < n; i++)
            y_mean += y[i];
        y_mean /= n;
        // Variance times n: n * Var[x]
        tdouble var_x_n = 0.0;
        for (int i = 0; i < n; i++)
            var_x_n += (x[i] - x_mean) * (x[i] - x_mean);
        // Covariance times n: n * Cov[x, y]
        tdouble cov_xy_n = 0.0;
        for (int i = 0; i < n; i++)
            cov_xy_n += (x[i] - x_mean) * (y[i] - y_mean);
        // Linear regression coefficients.
        tdouble beta = cov_xy_n / var_x_n;
        tdouble alpha = y_mean - beta * x_mean;
        // Estimation for y_i.
        std::vector<tdouble> y_estimate(n);
        for (int i = 0; i < n; i++)
            y_estimate[i] = alpha + beta * x[i];
        // Output vector.
        std::vector<tdouble> Y(n + 2);
        Y[0] = alpha;
        Y[1] = beta;
        for (int i = 0; i < n; i++)
            Y[2 + i] = y_estimate[i];
        if (flag_serializer)
            out_stream << "Output vector: " << Y << "\n\n";

        if (flag_serializer)
            out_stream << "Initial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        tfunc<double> f(X, Y);
        std::clock_t stop_time = std::clock();
        out_stream << "Tape memory (bytes): " << f.Memory() << std::endl;
        out_stream << "Tape creation took (ms): " << (stop_time - start_time) / (double)(CLOCKS_PER_SEC) * 1000 << '\n';

        // Derivative calculation time.
        std::clock_t calc_time = 0;

        // Forward sweep calculations.
        std::vector<double> dx(3);

        // Derivatives with respect to a.
        dx = { 1, 0, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        std::vector<double> forw;
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;
        check_derivatives_da(lin_regr, forw, eps);

        // Derivatives with respect to b.
        dx = { 0, 1, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;
        check_derivatives_db(lin_regr, forw, eps);

        // Derivatives with respect to c.
        dx = { 0, 0, 1 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;
        check_derivatives_dc(lin_regr, forw, eps);

        out_stream << "Forward calculation took (ms): " << calc_time / (double)(CLOCKS_PER_SEC) * 1000 << '\n';
        out_stream << "All derivatives calculated successfully." << std::endl;
        out_stream << std::endl;
    }

    // Example of linear regression differentiation with respect to input points using optimized tape.
    inline void linear_regression_with_points_optimized_example(const initial_data& data, std::ostream& out_stream = std::cout)
    {
        // Control production of tape output to serializer.
        bool flag_serializer = data.flag_serializer;

        // Input linear regression parameters.
        // n: number of points {x_i, y_i}.
        // x_i are x_0 = 0, x_1 = 1, ..., x_n = n.
        // y_i are y_i = a + b * x_i + exp(-1 * c * x_i).
        size_t n = data.n;
        double a = data.a;
        double b = data.b;
        double c = data.c;

        // Tolerance for analytical vs. adjoint derivative check.
        double eps = data.eps;

        out_stream << "Linear regression with points (optimized tape):\n" << std::endl;

        // Input values initialization.
        out_stream << "Input vector size: n = " << n << std::endl;
        LinearRegression lin_regr(n, a, b, c);
        lin_regr.Calculate();
        tarray x_ref_array = lin_regr.GetInputX();
        tarray y_ref_array = lin_regr.GetInputY();
        tvalue x_ref(x_ref_array), y_ref(y_ref_array);
        std::vector<tobject> X = { x_ref, y_ref };
        if (flag_serializer)
            out_stream << "Input vector: " << X << "\n";

        std::clock_t start_time = std::clock();
        // Declare the X vector as independent and start a tape recording.
        tape_start(X);

        // Output calculations.
        tobject& x = X[0];
        tobject& y = X[1];
        // Start linear regression calculation: calculate mean values.
        tobject x_mean = 1.0 / n * tapescript::sum_vec(x);
        tobject y_mean = 1.0 / n * tapescript::sum_vec(y);
        // Variance times n: n * Var[x]
        tobject var_x_n = tapescript::sum_vec((x - x_mean) * (x - x_mean));
        // Covariance times n: n * Cov[x, y]
        tobject cov_xy_n = tapescript::sum_vec((x - x_mean) * (y - y_mean));
        // Linear regression coefficients.
        tobject beta = cov_xy_n / var_x_n;
        tobject alpha = y_mean - beta * x_mean;
        // Estimation for y_i.
        tobject y_estimate = alpha + beta * x;
        // Output vector.
        std::vector<tobject> Y = { alpha, beta, y_estimate };
        if (flag_serializer)
            out_stream << "Output vector: " << Y << "\n\n";

        if (flag_serializer)
            out_stream << "Initial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        tfunc<tvalue> f(X, Y);
        std::clock_t stop_time = std::clock();
        out_stream << "Tape memory (bytes): " << f.Memory() << std::endl;
        out_stream << "Tape creation took (ms): " << (stop_time - start_time) / (double)(CLOCKS_PER_SEC)* 1000 << '\n';

        // Forward sweep calculations.
        tarray dx_ref_array, dy_ref_array;
        dx_ref_array.resize(n);
        dy_ref_array.resize(n);

        // Derivative calculation time.
        std::clock_t calc_time = 0;

        // Derivatives with respect to x_i.
        for (int j = 0; j < n; j++)
            dy_ref_array[j] = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                dx_ref_array[j] = 0;
            dx_ref_array[i] = 1;

            tvalue dx_ref(dx_ref_array), dy_ref(dy_ref_array);
            std::vector<tvalue> dx = { dx_ref, dy_ref };
            if (flag_serializer)
                out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
            start_time = std::clock();
            std::vector<tvalue> forw;
            if (flag_serializer)
                forw = f.forward(1, dx, out_stream);
            else
                forw = f.forward(1, dx);
            stop_time = std::clock();
            calc_time += stop_time - start_time;
            check_derivatives_dx(lin_regr, forw, i, eps);
        }

        // Derivatives with respect to y_i.
        for (int j = 0; j < n; j++)
            dx_ref_array[j] = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                dy_ref_array[j] = 0;
            dy_ref_array[i] = 1;

            tvalue dx_ref(dx_ref_array), dy_ref(dy_ref_array);
            std::vector<tvalue> dx = { dx_ref, dy_ref };
            if (flag_serializer)
                out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
            start_time = std::clock();
            std::vector<tvalue> forw;
            if (flag_serializer)
                forw = f.forward(1, dx, out_stream);
            else
                forw = f.forward(1, dx);
            stop_time = std::clock();
            calc_time += stop_time - start_time;
            check_derivatives_dy(lin_regr, forw, i, eps);
        }

        out_stream << "Forward calculation took (ms): " << calc_time / (double)(CLOCKS_PER_SEC)* 1000 << '\n';
        out_stream << "All derivatives calculated successfully." << std::endl;
        out_stream << std::endl;
    }

    // Example of linear regression differentiation with respect to input points using non-optimized tape.
    inline void linear_regression_with_points_nonoptimized_example(const initial_data& data, std::ostream& out_stream = std::cout)
    {
        // Control production of tape output to serializer.
        bool flag_serializer = data.flag_serializer;

        // Input linear regression parameters.
        // n: number of points {x_i, y_i}.
        // x_i are x_0 = 0, x_1 = 1, ..., x_n = n.
        // y_i are y_i = a + b * x_i + exp(-1 * c * x_i).
        size_t n = data.n;
        double a = data.a;
        double b = data.b;
        double c = data.c;

        // Tolerance for analytical vs. adjoint derivative check.
        double eps = data.eps;

        out_stream << "Linear regression with points (non-optimized tape):\n" << std::endl;

        // Input values initialization.
        out_stream << "Input vector size: n = " << n << std::endl;
        LinearRegression lin_regr(n, a, b, c);
        lin_regr.Calculate();
        tarray x_ref_array = lin_regr.GetInputX();
        tarray y_ref_array = lin_regr.GetInputY();
        std::vector<tdouble> X(2 * n);
        for (int i = 0; i < n; i++)
            X[i] = x_ref_array[i];
        for (int i = 0; i < n; i++)
            X[n + i] = y_ref_array[i];
        if (flag_serializer)
            out_stream << "Input vector: " << X << "\n";

        std::clock_t start_time = std::clock();
        // Declare the X vector as independent and start a tape recording.
        tape_start(X);

        // Output calculations.
        // Start linear regression calculation: calculate mean values.
        tdouble x_mean = 0.0;
        for (int i = 0; i < n; i++)
            x_mean += X[i];
        x_mean /= n;
        tdouble y_mean = 0.0;
        for (int i = 0; i < n; i++)
            y_mean += X[n + i];
        y_mean /= n;
        // Variance times n: n * Var[x]
        tdouble var_x_n = 0.0;
        for (int i = 0; i < n; i++)
            var_x_n += (X[i] - x_mean) * (X[i] - x_mean);
        // Covariance times n: n * Cov[x, y]
        tdouble cov_xy_n = 0.0;
        for (int i = 0; i < n; i++)
            cov_xy_n += (X[i] - x_mean) * (X[n + i] - y_mean);
        // Linear regression coefficients.
        tdouble beta = cov_xy_n / var_x_n;
        tdouble alpha = y_mean - beta * x_mean;
        // Estimation for y_i.
        std::vector<tdouble> y_estimate(n);
        for (int i = 0; i < n; i++)
            y_estimate[i] = alpha + beta * X[i];
        // Output vector.
        std::vector<tdouble> Y(n + 2);
        Y[0] = alpha;
        Y[1] = beta;
        for (int i = 0; i < n; i++)
            Y[2 + i] = y_estimate[i];
        if (flag_serializer)
            out_stream << "Output vector: " << Y << "\n\n";

        if (data.flag_serializer)
            out_stream << "Initial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        tfunc<double> f(X, Y);
        std::clock_t stop_time = std::clock();
        out_stream << "Tape memory (bytes): " << f.Memory() << std::endl;
        out_stream << "Tape creation took (ms): " << (stop_time - start_time) / (double)(CLOCKS_PER_SEC)* 1000 << '\n';

        // Derivative calculation time.
        std::clock_t calc_time = 0;

        // Forward sweep calculations.
        std::vector<double> dx(2 * n);

        // Derivatives with respect to x_i.
        for (int j = 0; j < n; j++)
            dx[n + j] = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                dx[j] = 0;
            dx[i] = 1;
            start_time = std::clock();
            std::vector<double> forw;
            if (flag_serializer)
                forw = f.forward(1, dx, out_stream);
            else
                forw = f.forward(1, dx);
            stop_time = std::clock();
            calc_time += stop_time - start_time;
            check_derivatives_dx(lin_regr, forw, i, eps);
        }

        // Derivatives with respect to y_i.
        for (int j = 0; j < n; j++)
            dx[j] = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                dx[n + j] = 0;
            dx[n + i] = 1;
            start_time = std::clock();
            std::vector<double> forw;
            if (flag_serializer)
                forw = f.forward(1, dx, out_stream);
            else
                forw = f.forward(1, dx);
            stop_time = std::clock();
            calc_time += stop_time - start_time;
            check_derivatives_dy(lin_regr, forw, i, eps);
        }

        out_stream << "Forward calculation took (ms): " << calc_time / (double)(CLOCKS_PER_SEC)* 1000 << '\n';
        out_stream << "All derivatives calculated successfully." << std::endl;
        out_stream << std::endl;
    }

    inline void regression_examples()
    {
        std::ofstream of("output/performance/regression_output.txt");
        tape_serializer<tvalue> serializer(of);
        serializer.precision(3);

        // Input data parameters.
        initial_data data;

        linear_regression_with_params_optimized_example(data, serializer);
        linear_regression_with_params_nonoptimized_example(data, serializer);
        linear_regression_with_points_optimized_example(data, serializer);
        linear_regression_with_points_nonoptimized_example(data, serializer);
    }
}

#endif // cl_tape_examples_impl_regression_examples_hpp
