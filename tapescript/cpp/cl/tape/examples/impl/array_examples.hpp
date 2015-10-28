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

#ifndef cl_array_examples_hpp
#define cl_array_examples_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include "impl/utils.hpp"

namespace cl
{
    inline void plus_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Plus operation:\n" << std::endl;

        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        cl::tvalue x1 = { 0, 1 };
        std::vector<cl::tobject> X = { x0, x1 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject sum = X[0] + X[1];
        std::vector<cl::tobject> Y = { sum };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 2, 5 }, { 1, -1 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 3, 7 } };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);

        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void minus_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Minus operation:\n\n";

        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        cl::tvalue x1 = { 0, 1 };
        std::vector<cl::tobject> X = { x0, x1 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject diff = X[0] - X[1];
        std::vector<cl::tobject> Y = { diff };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";

# if defined CL_TAPE_ARCHIVER_TEST
        typedef cl::tape_archive<double, boost::archive::binary_oarchive> tapewriter;
        tapewriter ss("c:\\ooo.bin");

        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y, ss);
#   else
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);
#   endif

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 2, 5 }, { 1, -1 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 3, 7 } };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void exp_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Exponent function:\n\n";

        // Input values initialization.
        cl::tvalue x0 = { 0, 1 };
        std::vector<cl::tobject> X = { x0 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject exp = std::exp(X[0]);
        std::vector<cl::tobject> Y = { exp };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 2, -1 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 1, -1 } };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void cos_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Cosine function:\n\n";

        // Input values initialization.
        cl::tvalue x0 = { 1, 3.14159265359 / 2 };
        std::vector<cl::tobject> X = { x0 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject cos = std::cos(X[0]);
        std::vector<cl::tobject> Y = { cos };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 1, 1 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { -2, 1 } };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void discount_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Discount calculations:\n\n";

        // Input values initialization.
        cl::tvalue spot = { 100, 110 };
        cl::tvalue rate = { 0.05, 0.04 };
        cl::tvalue time = { 1, 2 };
        std::vector<cl::tobject> X = { spot, rate, time };
        out_str << "Spot: " << spot << "\n";
        out_str << "Rate: " << rate << "\n";
        out_str << "Time: " << time << "\n";
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject discount_factor = std::exp(-X[1] * X[2]);
        cl::tobject discounted_price = X[0] * discount_factor;
        std::vector<cl::tobject> Y = { discounted_price, discount_factor };
        out_str << "Discounted price: " << discounted_price << "\n";
        out_str << "Discount factor: " << discount_factor << "\n";
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        out_str << "Calculation of sensitivity to 1% change in interest rate.\n";
        std::vector<cl::tvalue> dx = { 0, 0.01, 0 };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n";
        out_str << "Sensitivity of discounted price: " << forw[0] << "\n";
        out_str << "Sensitivity of discount factor:  " << forw[1] << "\n\n";

        // Reverse sweep calculations.
        out_str << "Calculation of sensitivity of discount factor.\n";
        std::vector<cl::tvalue> w = { 0, 1 };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n";
        out_str << "Sensitivity to the changes in spot price:    " << rev[0] << "\n";
        out_str << "Sensitivity to the changes in interest rate: " << rev[1] << "\n";
        out_str << "Sensitivity to the changes in maturity time: " << rev[2] << "\n\n\n";
    }

    inline void example1(std::ostream& out_stream = std::cout)
    {
        out_str << "We are mixing arrays and scalars in calculations of\n";
        out_str << "Y = { sin(x0) / cos(x1) + x1 / x0, y0 + x0 * x1^3 - 0.5 } function:\n\n";

        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        cl::tvalue x1 = -1.2;
        std::vector<cl::tobject> X = { x0, x1 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject y0 = std::sin(X[0]) / std::cos(X[1]) + X[1] / X[0];
        cl::tobject y1 = y0 + X[0] * std::pow(X[1], 3) - 0.5;
        std::vector<cl::tobject> Y = { y0, y1 };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 1, -1 }, 2 };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { -2, 1 }, { 2, 3 } };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void sum_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Sum of an array:\n\n";

        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        std::vector<cl::tobject> X = { x0 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject y0 = cl::tapescript::sum_vec(X[0]);
        std::vector<cl::tobject> Y = { y0 };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 1.5, -1 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { 4 };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void rev_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Reversing of an array:\n\n";

        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        std::vector<cl::tobject> X = { x0 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject y0 = cl::tapescript::reverse_vec(X[0]);
        std::vector<cl::tobject> Y = { y0 };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 1.5, -1 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { 4 };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void conc_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Array concatenation:\n" << std::endl;

        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        cl::tvalue x1 = 7;
        cl::tvalue x2 = { 0, 5 };
        std::vector<cl::tobject> X = { x0, x1, x2 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject temp = cl::tapescript::conc_vec(X[0], X[1]);
        cl::tobject y0 = cl::tapescript::conc_vec(temp, X[2]);
        std::vector<cl::tobject> Y = { y0 };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 2, 5 }, 12, { 1, -1 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 1, 2, 3, 4, 5 } };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void make_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Array construction from scalar:\n" << std::endl;

        // Input values initialization.
        cl::tvalue x0 = 7;
        std::vector<cl::tobject> X = { x0 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject y0 = cl::tapescript::make_vec(X[0], 2);
        std::vector<cl::tobject> Y = { y0 };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { 1 };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 1, 2 } };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void max_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Maximum of two arrays:\n" << std::endl;

        // Input values initialization.
        cl::tvalue x0 = { -1, 0, 1 };
        cl::tvalue x1 = { 1, 0, -1 };
        std::vector<cl::tobject> X = { x0, x1 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject y0 = std::max(X[0], X[1]);
        std::vector<cl::tobject> Y = { y0 };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 1, 1, 1 }, { 2, 2, 2 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 1, 2, 3 } };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void norm_example(std::ostream& out_stream = std::cout)
    {
        out_str << "L2 norm of an array:\n" << std::endl;

        // Input values initialization.
        cl::tvalue x0 = 3;
        std::vector<cl::tobject> X = { x0 };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject array = cl::tapescript::make_vec(X[0], 2);
        cl::tobject norm = std::sqrt(cl::tapescript::sum_vec(std::pow(array, 2)));
        std::vector<cl::tobject> Y = { norm };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { 1 };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { 1 };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void linear_regression_example(std::ostream& out_stream = std::cout)
    {
        out_str << "Linear regression:\n" << std::endl;

        // Input values initialization.
        size_t n = 3;
        cl::tvalue x_ref = { -1, 0, 1 };
        cl::tvalue y_ref = { 1, 0, 3 };
        std::vector<cl::tobject> X = { x_ref, y_ref };
        out_str << "Input vector: " << X << "\n";

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject& x = X[0];
        cl::tobject& y = X[1];
        cl::tobject x_mean = 1.0 / n * cl::tapescript::sum_vec(x);
        cl::tobject y_mean = 1.0 / n * cl::tapescript::sum_vec(y);
        cl::tobject x_centralized = x - x_mean;
        cl::tobject y_centralized = y - y_mean;
        // Variance times n: n * Var[x]
        cl::tobject var_x_n = cl::tapescript::sum_vec(x_centralized * x_centralized);
        // Covariance times n: n * Cov[x, y]
        cl::tobject cov_xy_n = cl::tapescript::sum_vec(x_centralized * y_centralized);
        cl::tobject beta = cov_xy_n / var_x_n;
        cl::tobject alpha = y_mean - beta * x_mean;
        cl::tobject y_estimate = alpha + beta * x;
        std::vector<cl::tobject> Y = { alpha, beta, y_estimate };
        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 1, 0, 0 }, { 0, 0, 0 } };
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { 0, 1, 0 };
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<cl::tvalue> rev = f.reverse(1, w, out_stream);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";
    }

    inline void array_examples()
    {
        std::ofstream of("output/output.txt");
        cl::tape_serializer<cl::tvalue> serializer(of);
        serializer.precision(3);

        plus_example(serializer);
        minus_example(serializer);
        exp_example(serializer);
        cos_example(serializer);
        discount_example(serializer);
        example1(serializer);
        sum_example(serializer);
        rev_example(serializer);
        conc_example(serializer);
        make_example(serializer);
        max_example(serializer);
        norm_example(serializer);
        linear_regression_example(serializer);
    }
}

#endif // cl_array_examples_hpp
