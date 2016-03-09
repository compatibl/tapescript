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

#ifndef cl_tape_examples_impl_quadratic_regression_examples_hpp
#define cl_tape_examples_impl_quadratic_regression_examples_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>

namespace cl
{
    struct quadratic_initial_data
    {
        // Control production of tape output to serializer.
        bool flag_serializer = false;

        // Input quadratic regression parameters.
        // n: number of points {x_i, y_i}.
        // x_i are x_0 = 0, x_1 = 1, ..., x_n = n.
        // y_i are y_i = a + b * x_i + c * x_i * x_i + exp(-1 * d * x_i).
#if defined NDEBUG
        size_t n = 12000;
#else
        size_t n = 2000;
#endif
        double a = 1.0;
        double b = 2.0;
        double c = 3.0;
        double d = 4.0;

        // Tolerance for analytical vs. adjoint derivative check.
        double eps = 1e-10;
    };

    // Example of quadratic regression differentiation with respect to parameters of input distribution using optimized tape.
    inline void quadratic_regression_with_params_optimized_example(const quadratic_initial_data& data, std::ostream& out_stream = std::cout)
    {
        // Control production of tape output to serializer.
        bool flag_serializer = data.flag_serializer;

        // Input linear regression parameters.
        // n: number of points {x_i, y_i}.
        // x_i are x_0 = 0, x_1 = 1, ..., x_n = n.
        // y_i are y_i = a + b * x_i + c * x_i * x_i + exp(-1 * d * x_i).
        size_t n = data.n;
        double a = data.a;
        double b = data.b;
        double c = data.c;
        double d = data.d;

        // Tolerance for analytical vs. adjoint derivative check.
        double eps = data.eps;

        out_stream << "Quadratic regression with parameters (optimized tape):\n" << std::endl;

        // Input values initialization.
        out_stream << "Input vector size: n = " << n << std::endl;
        tvalue a_ref = a;
        tvalue b_ref = b;
        tvalue c_ref = c;
        tvalue d_ref = d;
        std::vector<tobject> X = { a_ref, b_ref, c_ref, d_ref };
        if (flag_serializer)
            out_stream << "Input vector: " << X << "\n";

        std::clock_t start_time = std::clock();
        // Declare the X vector as independent and start a tape recording.
        tape_start(X);

        // Output calculations.
        tobject& par_a = X[0];
        tobject& par_b = X[1];
        tobject& par_c = X[2];
        tobject& par_d = X[3];
        // Obtain x_i values.
        tarray x_ref(n);
        for (int i = 0; i < n; i++)
            x_ref[i] = i;
        tobject x = (tvalue)x_ref;
        tobject x2 = x * x;
        // Calculate corresponding y_i values.
        tobject y = par_a + x * par_b + x2 * par_c + std::exp(-1 * par_d * x);
        // Start quadratic regression calculation: calculate mean values.
        tobject sum_x = tapescript::sum_vec(x);
        tobject sum_y = tapescript::sum_vec(y);
        tobject sum_x2 = tapescript::sum_vec(x2);
        tobject y2 = y * y;
        tobject sum_y2 = tapescript::sum_vec(y2);
        tobject xy = x * y;
        tobject sum_xy = tapescript::sum_vec(xy);
        tobject x3 = x2 * x;
        tobject sum_x3 = tapescript::sum_vec(x3);
        tobject x2y = x2 * y;
        tobject sum_x2y = tapescript::sum_vec(x2y);
        tobject x4 = x3 * x;
        tobject sum_x4 = tapescript::sum_vec(x4);
        // Calculate covariances.
        tobject S_xx = sum_x2 - sum_x * sum_x / n;
        tobject S_xy = sum_xy - sum_x * sum_y / n;
        tobject S_xx2 = sum_x3 - sum_x * sum_x2 / n;
        tobject S_x2y = sum_x2y - sum_x2 * sum_y / n;
        tobject S_x2x2 = sum_x4 - sum_x2 * sum_x2 / n;
        // Quadratic regression coefficients.
        tobject denominator = S_xx * S_x2x2 - pow(S_xx2, 2.0);
        tobject gamma = (S_x2y * S_xx - S_xy * S_xx2) / denominator;
        tobject beta = (S_xy * S_x2x2 - S_x2y * S_xx2) / denominator;
        tobject alpha = (sum_y - beta * sum_x - gamma * sum_x2) / n;
        // Estimation for y_i.
        tobject y_estimate = alpha + beta * x + gamma * x2;
        // Output vector.
        std::vector<tobject> Y = { alpha, beta, gamma, y_estimate };
        //out_stream << "Output vector: " << Y << "\n\n";

        if (flag_serializer)
            out_stream << "Initial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        tfunc<tvalue> f(X, Y);
        std::clock_t stop_time = std::clock();
        out_stream << "Tape memory (bytes): " << f.Memory() << std::endl;
        out_stream << "Tape creation took (ms): " << (stop_time - start_time) / (double)(CLOCKS_PER_SEC)* 1000 << '\n';

        // Forward sweep calculations.
        tarray d_ref_array;
        d_ref_array.resize(3);

        // Derivative calculation time.
        std::clock_t calc_time = 0;

        // Derivatives with respect to a.
        std::vector<tvalue> dx = { 1, 0, 0, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<tvalue> forw;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives with respect to b.
        dx = { 0, 1, 0, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives with respect to c.
        dx = { 0, 0, 1, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives with respect to d.
        dx = { 0, 0, 0, 1 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        out_stream << "Forward calculation took (ms): " << calc_time / (double)(CLOCKS_PER_SEC) * 1000 << '\n';

        // Derivatives in reverse mode.
        tarray alpha_rev, beta_rev, gamma_rev;
        tarray y_estimate_rev(n);
        std::vector<tvalue> dx_rev = { alpha_rev, beta_rev, gamma_rev, y_estimate_rev };

        // Derivatives for alpha.
        dx_rev[0] = 1;
        dx_rev[1] = dx_rev[2] = dx_rev[3] = 0;
        std::vector<tvalue> rev;
        start_time = std::clock();
        if (flag_serializer)
            rev = f.Reverse(1, std::make_pair(dx_rev, &out_stream)).first;
        else
            rev = f.Reverse(1, dx_rev);
        stop_time = std::clock();
        calc_time = stop_time - start_time;

        // Derivatives for beta.
        dx_rev[1] = 1;
        dx_rev[0] = dx_rev[2] = dx_rev[3] = 0;
        start_time = std::clock();
        if (flag_serializer)
            rev = f.Reverse(1, std::make_pair(dx_rev, &out_stream)).first;
        else
            rev = f.Reverse(1, dx_rev);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives for gamma.
        dx_rev[2] = 1;
        dx_rev[0] = dx_rev[1] = dx_rev[3] = 0;
        start_time = std::clock();
        if (flag_serializer)
            rev = f.Reverse(1, std::make_pair(dx_rev, &out_stream)).first;
        else
            rev = f.Reverse(1, dx_rev);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives for y_estimate.
        dx_rev[0] = dx_rev[1] = dx_rev[2] = 0;
        std::valarray<double> dx_rev_estim;
        dx_rev_estim.resize(n);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                dx_rev_estim[j] = 0;
            dx_rev_estim[i] = 1;
            dx_rev[3] = tvalue(dx_rev_estim);
            start_time = std::clock();
            if (flag_serializer)
                rev = f.Reverse(1, std::make_pair(dx_rev, &out_stream)).first;
            else
                rev = f.Reverse(1, dx_rev);
            stop_time = std::clock();
            calc_time += stop_time - start_time;
        }

        out_stream << "Reverse calculation took (ms): " << calc_time / (double)(CLOCKS_PER_SEC) * 1000 << '\n';
        out_stream << std::endl;
    }

    // Example of quadratic regression differentiation with respect to parameters of input distribution using optimized tape.
    inline void quadratic_regression_with_params_nonoptimized_example(const quadratic_initial_data& data, std::ostream& out_stream = std::cout)
    {
        // Control production of tape output to serializer.
        bool flag_serializer = data.flag_serializer;

        // Input linear regression parameters.
        // n: number of points {x_i, y_i}.
        // x_i are x_0 = 0, x_1 = 1, ..., x_n = n.
        // y_i are y_i = a + b * x_i + c * x_i * x_i + exp(-1 * d * x_i).
        size_t n = data.n;
        double a = data.a;
        double b = data.b;
        double c = data.c;
        double d = data.d;

        // Tolerance for analytical vs. adjoint derivative check.
        double eps = data.eps;

        out_stream << "Quadratic regression with parameters (non-optimized tape):\n" << std::endl;

        // Input values initialization.
        out_stream << "Input vector size: n = " << n << std::endl;
        std::vector<tdouble> X = { a, b, c, d };
        if (flag_serializer)
            out_stream << "Input vector: " << X << "\n";

        std::clock_t start_time = std::clock();
        // Declare the X vector as independent and start a tape recording.
        tape_start(X);

        // Output calculations.
        tdouble& par_a = X[0];
        tdouble& par_b = X[1];
        tdouble& par_c = X[2];
        tdouble& par_d = X[3];
        // Obtain x_i values.
        std::vector<tdouble> x(n);
        for (int i = 0; i < n; i++)
            x[i] = i;
        std::vector<tdouble> x2(n);
        for (int i = 0; i < n; i++)
            x2[i] = x[i] * x[i];
        // Calculate corresponding y_i values.
        std::vector<tdouble> y(n);
        for (int i = 0; i < n; i++)
            y[i] = par_a + x[i] * par_b + x2[i] * par_c + std::exp(-1 * par_d * x[i]);
        // Start quadratic regression calculation: calculate mean values.
        tdouble sum_x = 0.0;
        for (int i = 0; i < n; i++)
            sum_x += x[i];
        tdouble sum_y = 0.0;
        for (int i = 0; i < n; i++)
            sum_y += y[i];
        tdouble sum_x2 = 0.0;
        for (int i = 0; i < n; i++)
            sum_x2 += x2[i];
        std::vector<tdouble> y2(n);
        for (int i = 0; i < n; i++)
            y2[i] = y[i] * y[i];
        tdouble sum_y2 = 0.0;
        for (int i = 0; i < n; i++)
            sum_y2 += y2[i];
        std::vector<tdouble> xy(n);
        for (int i = 0; i < n; i++)
            xy[i] = x[i] * y[i];
        tdouble sum_xy = 0.0;
        for (int i = 0; i < n; i++)
            sum_xy += xy[i];
        std::vector<tdouble> x3(n);
        for (int i = 0; i < n; i++)
            x3[i] = x2[i] * x[i];
        tdouble sum_x3 = 0.0;
        for (int i = 0; i < n; i++)
            sum_x3 += x3[i];
        std::vector<tdouble> x2y(n);
        for (int i = 0; i < n; i++)
            x2y[i] = x2[i] * y[i];
        tdouble sum_x2y = 0.0;
        for (int i = 0; i < n; i++)
            sum_x2y += x2y[i];
        std::vector<tdouble> x4(n);
        for (int i = 0; i < n; i++)
            x4[i] = x3[i] * x[i];
        tdouble sum_x4 = 0.0;
        for (int i = 0; i < n; i++)
            sum_x4 += x4[i];
        // Calculate covariances.
        tdouble S_xx = sum_x2 - sum_x * sum_x / n;
        tdouble S_xy = sum_xy - sum_x * sum_y / n;
        tdouble S_xx2 = sum_x3 - sum_x * sum_x2 / n;
        tdouble S_x2y = sum_x2y - sum_x2 * sum_y / n;
        tdouble S_x2x2 = sum_x4 - sum_x2 * sum_x2 / n;
        // Quadratic regression coefficients.
        tdouble denominator = S_xx * S_x2x2 - std::pow(S_xx2, 2.0);
        tdouble gamma = (S_x2y * S_xx - S_xy * S_xx2) / denominator;
        tdouble beta = (S_xy * S_x2x2 - S_x2y * S_xx2) / denominator;
        tdouble alpha = (sum_y - beta * sum_x - gamma * sum_x2) / n;
        // Estimation for y_i.
        std::vector<tdouble> y_estimate(n);
        for (int i = 0; i < n; i++)
           y_estimate[i] = alpha + beta * x[i] + gamma * x2[i];
        // Output vector.
        std::vector<tdouble> Y(n + 3);
        Y[0] = alpha;
        Y[1] = beta;
        Y[2] = gamma;
        for (int i = 0; i < n; i++)
            Y[3 + i] = y_estimate[i];
        if (flag_serializer)
            out_stream << "Output vector: " << Y << "\n\n";

        if (flag_serializer)
            out_stream << "Initial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        tfunc<double> f(X, Y);
        std::clock_t stop_time = std::clock();
        out_stream << "Tape memory (bytes): " << f.Memory() << std::endl;
        out_stream << "Tape creation took (ms): " << (stop_time - start_time) / (double)(CLOCKS_PER_SEC)* 1000 << '\n';

        // Derivative calculation time.
        std::clock_t calc_time = 0;

        // Derivatives with respect to a.
        std::vector<double> dx = { 1, 0, 0, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<double> forw;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives with respect to b.
        dx = { 0, 1, 0, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives with respect to c.
        dx = { 0, 0, 1, 0 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives with respect to d.
        dx = { 0, 0, 0, 1 };
        if (flag_serializer)
            out_stream << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        start_time = std::clock();
        if (flag_serializer)
            forw = f.forward(1, dx, out_stream);
        else
            forw = f.forward(1, dx);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        out_stream << "Forward calculation took (ms): " << calc_time / (double)(CLOCKS_PER_SEC)* 1000 << '\n';

        // Derivatives in revrerse mode.
        std::vector<double> dx_rev(n + 3);

        // Derivatives for alpha.
        dx_rev[0] = 1;
        dx_rev[1] = dx_rev[2] = 0;
        std::vector<double> rev;
        start_time = std::clock();
        if (flag_serializer)
            rev = f.Reverse(1, std::make_pair(dx_rev, &out_stream)).first;
        else
            rev = f.Reverse(1, dx_rev);
        stop_time = std::clock();
        calc_time = stop_time - start_time;

        // Derivatives for beta.
        dx_rev[1] = 1;
        dx_rev[0] = dx_rev[2] = 0;
        start_time = std::clock();
        if (flag_serializer)
            rev = f.Reverse(1, std::make_pair(dx_rev, &out_stream)).first;
        else
            rev = f.Reverse(1, dx_rev);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives for gamma.
        dx_rev[2] = 1;
        dx_rev[0] = dx_rev[1] = 0;
        start_time = std::clock();
        if (flag_serializer)
            rev = f.Reverse(1, std::make_pair(dx_rev, &out_stream)).first;
        else
            rev = f.Reverse(1, dx_rev);
        stop_time = std::clock();
        calc_time += stop_time - start_time;

        // Derivatives for y_estimate.
        dx_rev[0] = dx_rev[1] = dx_rev[2] = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                dx_rev[3 + j] = 0;
            dx_rev[3 + i] = 1;
            start_time = std::clock();
            if (flag_serializer)
                rev = f.Reverse(1, std::make_pair(dx_rev, &out_stream)).first;
            else
                rev = f.Reverse(1, dx_rev);
            stop_time = std::clock();
            calc_time += stop_time - start_time;
        }

        out_stream << "Reverse calculation took (ms): " << calc_time / (double)(CLOCKS_PER_SEC)* 1000 << '\n';
        out_stream << std::endl;
    }


    inline void quadratic_regression_examples()
    {
        std::ofstream of("output/performance/quadratic_regression_output.txt");
        tape_serializer<tvalue> serializer(of);
        serializer.precision(3);

        // Input data parameters.
        quadratic_initial_data data;

        quadratic_regression_with_params_optimized_example(data, serializer);
        quadratic_regression_with_params_nonoptimized_example(data, serializer);
    }
}

#endif // cl_tape_examples_impl_quadratic_regression_examples_hpp
