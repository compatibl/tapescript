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

#ifndef cl_tape_examples_array_examples_hpp
#define cl_tape_examples_array_examples_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>

template <class Array>
inline void print_sizeofs()
{
    std::cout << std::setw(40) << typeid(Array).name() << ": \t";
    std::cout << sizeof(Array) << "\t";
    std::cout << sizeof(cl::inner_array<Array>) << "\t";
    std::cout << sizeof(CppAD::AD<cl::inner_array<Array>>) << "\t";
    std::cout << sizeof(cl::tape_double<cl::inner_array<Array>>) << std::endl;
}


inline void print_sizeofs_all()
{
    std::cout << "Sizeofs:" << std::endl;

    /*
    print_sizeofs<Eigen::ArrayXd>();
    print_sizeofs<Eigen::Array2d>();
    print_sizeofs<Eigen::Array4d>();
    print_sizeofs<std::valarray<double>>();
    */
}


template <class Ty>
std::ostream& operator<<(std::ostream& ostr, std::vector<Ty> const& v)
{
    if (v.size() == 0)
    {
        return ostr << "{}";
    }

    std::stringstream ss;
    ss.precision(ostr.precision());
    ss << "{ " << v[0];
    for (size_t i = 1; i < v.size(); i++)
    {
        ss << ", " << v[i];
    }
    ss << " }";

    return ostr << ss.str();
}

inline void plus_example(std::ostream& out_str = std::cout)
{
    out_str << "Plus operation:\n" << std::endl;

    // Input values initialization.
    cl::InnerArray x0 = { 1, 2 };
    cl::InnerArray x1 = { 0, 1 };
    std::vector<cl::TapeArray> X = { x0 , x1 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray sum = X[0] + X[1];
    std::vector<cl::TapeArray> Y = { sum };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 2, 5 }, { 1, -1 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { { 3, 7 } };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void minus_example(std::ostream& out_str = std::cout)
{
    out_str << "Minus operation:\n\n";

    // Input values initialization.
    cl::InnerArray x0 = { 1, 2 };
    cl::InnerArray x1 = { 0, 1 };
    std::vector<cl::TapeArray> X = { x0, x1 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray diff = X[0] - X[1];
    std::vector<cl::TapeArray> Y = { diff };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";

    # if defined CL_TAPE_ARCHIVER_TEST
    typedef cl::tape_archive<double, boost::archive::binary_oarchive> tapewriter;
    tapewriter ss("c:\\ooo.bin");

    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y, ss);
#   else
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);
#   endif

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 2, 5 }, { 1, -1 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { { 3, 7 } };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void exp_example(std::ostream& out_str = std::cout)
{
    out_str << "Exponent function:\n\n";

    // Input values initialization.
    cl::InnerArray x0 = { 0, 1 };
    std::vector<cl::TapeArray> X = { x0 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray exp = std::exp(X[0]);
    std::vector<cl::TapeArray> Y = { exp };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 2, -1 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { { 1, -1 } };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void cos_example(std::ostream& out_str = std::cout)
{
    out_str << "Cosine function:\n\n";

    // Input values initialization.
    cl::InnerArray x0 = { 1, 3.14159265359 / 2 };
    std::vector<cl::TapeArray> X = { x0 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray cos = std::cos(X[0]);
    std::vector<cl::TapeArray> Y = { cos };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 1, 1 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { { -2, 1 } };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void discount_example(std::ostream& out_str = std::cout)
{
    out_str << "Discount calculations:\n\n";

    // Input values initialization.
    cl::InnerArray spot = { 100, 110 };
    cl::InnerArray rate = { 0.05, 0.04 };
    cl::InnerArray time = { 1, 2 };
    std::vector<cl::TapeArray> X = { spot, rate, time };
    out_str << "Spot: " << spot << "\n";
    out_str << "Rate: " << rate << "\n";
    out_str << "Time: " << time << "\n";
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray discount_factor = std::exp(-X[1] * X[2]);
    cl::TapeArray discounted_price = X[0] * discount_factor;
    std::vector<cl::TapeArray> Y = { discounted_price, discount_factor };
    out_str << "Discounted price: " << discounted_price<< "\n";
    out_str << "Discount factor: " << discount_factor << "\n";
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    out_str << "Calculation of sensitivity to 1% change in interest rate.\n";
    std::vector<cl::InnerArray> dx = { 0, 0.01, 0 };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n";
    out_str << "Sensitivity of discounted price: " << forw[0] << "\n";
    out_str << "Sensitivity of discount factor:  " << forw[1] << "\n\n";

    // Reverse sweep calculations.
    out_str << "Calculation of sensitivity of discount factor.\n";
    std::vector<cl::InnerArray> w = { 0, 1 };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n";
    out_str << "Sensitivity to the changes in spot price:    " << rev[0] << "\n";
    out_str << "Sensitivity to the changes in interest rate: " << rev[1] << "\n";
    out_str << "Sensitivity to the changes in maturity time: " << rev[2] << "\n\n\n";
}

inline void example1(std::ostream& out_str = std::cout)
{
    out_str << "We are mixing arrays and scalars in calculations of\n";
    out_str << "Y = { sin(x0) / cos(x1) + x1 / x0, y0 + x0 * x1^3 - 0.5 } function:\n\n";

    // Input values initialization.
    cl::InnerArray x0 = { 1, 2 };
    cl::InnerArray x1 = -1.2;
    std::vector<cl::TapeArray> X = { x0, x1 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray y0 = std::sin(X[0]) / std::cos(X[1]) + X[1] / X[0];
    cl::TapeArray y1 = y0 + X[0] * std::pow(X[1], 3) - 0.5;
    std::vector<cl::TapeArray> Y = { y0, y1 };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 1, -1 }, 2 };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { { -2, 1 }, { 2, 3 } };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void sum_example(std::ostream& out_str = std::cout)
{
    out_str << "Sum of an array:\n\n";

    // Input values initialization.
    cl::InnerArray x0 = { 1, 2 };
    std::vector<cl::TapeArray> X = { x0 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray y0 = cl::tapescript::sum_vec(X[0]);
    std::vector<cl::TapeArray> Y = { y0 };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 1.5, -1 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { 4 };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void rev_example(std::ostream& out_str = std::cout)
{
    out_str << "Reversing of an array:\n\n";

    // Input values initialization.
    cl::InnerArray x0 = { 1, 2 };
    std::vector<cl::TapeArray> X = { x0 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray y0 = cl::tapescript::reverse_vec(X[0]);
    std::vector<cl::TapeArray> Y = { y0 };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 1.5, -1 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { 4 };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void conc_example(std::ostream& out_str = std::cout)
{
    out_str << "Array concatenation:\n" << std::endl;

    // Input values initialization.
    cl::InnerArray x0 = { 1, 2 };
    cl::InnerArray x1 = 7;
    cl::InnerArray x2 = { 0, 5 };
    std::vector<cl::TapeArray> X = { x0, x1, x2 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray temp = cl::tapescript::conc_vec(X[0], X[1]);
    cl::TapeArray y0 = cl::tapescript::conc_vec(temp, X[2]);
    std::vector<cl::TapeArray> Y = { y0 };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 2, 5 }, 12, { 1, -1 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { { 1, 2, 3, 4, 5 } };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void make_example(std::ostream& out_str = std::cout)
{
    out_str << "Array construction from scalar:\n" << std::endl;

    // Input values initialization.
    cl::InnerArray x0 = 7;
    std::vector<cl::TapeArray> X = { x0 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray y0 = cl::tapescript::make_vec(X[0], 2);
    std::vector<cl::TapeArray> Y = { y0 };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { 1 };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { { 1, 2 } };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void max_example(std::ostream& out_str = std::cout)
{
    out_str << "Maximum of two arrays:\n" << std::endl;

    // Input values initialization.
    cl::InnerArray x0 = { -1, 0, 1 };
    cl::InnerArray x1 = { 1, 0, -1 };
    std::vector<cl::TapeArray> X = { x0, x1 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray y0 = std::max(X[0], X[1]);
    std::vector<cl::TapeArray> Y = { y0 };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 1, 1, 1 }, { 2, 2, 2 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { { 1, 2, 3 } };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void norm_example(std::ostream& out_str = std::cout)
{
    out_str << "L2 norm of an array:\n" << std::endl;

    // Input values initialization.
    cl::InnerArray x0 = 3;
    std::vector<cl::TapeArray> X = { x0 };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray array = cl::tapescript::make_vec(X[0], 2);
    cl::TapeArray norm = std::sqrt(cl::tapescript::sum_vec(std::pow(array, 2)));
    std::vector<cl::TapeArray> Y = { norm };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { 1 };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { 1 };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void linear_regression_example(std::ostream& out_str = std::cout)
{
    out_str << "Linear regression:\n" << std::endl;

    // Input values initialization.
    size_t n = 3;
    cl::InnerArray x_ref = { -1, 0, 1 };
    cl::InnerArray y_ref = { 1, 0, 3 };
    std::vector<cl::TapeArray> X = { x_ref, y_ref };
    out_str << "Input vector: " << X << "\n";

    // Declare the X vector as independent and start a tape recording.
    cl::Independent(X);

    // Output calculations.
    cl::TapeArray& x = X[0];
    cl::TapeArray& y = X[1];
    cl::TapeArray x_mean = 1.0 / n * cl::tapescript::sum_vec(x);
    cl::TapeArray y_mean = 1.0 / n * cl::tapescript::sum_vec(y);
    cl::TapeArray x_centralized = x - x_mean;
    // Variance times n: n * Var[x]
    cl::TapeArray var_x_n = cl::tapescript::sum_vec(x_centralized * x_centralized);
    // Covariance times n: n * Cov[x, y]
    cl::TapeArray cov_xy_n = cl::tapescript::sum_vec(x_centralized * (y - y_mean));
    cl::TapeArray beta = cov_xy_n / var_x_n;
    cl::TapeArray alpha = y_mean - beta * x_mean;
    cl::TapeArray y_estimate = alpha + beta * x;
    std::vector<cl::TapeArray> Y = { alpha, beta, y_estimate };
    out_str << "Output vector: " << Y << "\n\n";

    out_str << "Ininial Forward(0) sweep...\n\n";
    // Declare a tape function and stop the tape recording.
    cl::TapeFunction<cl::InnerArray> f(X, Y);

    // Forward sweep calculations.
    std::vector<cl::InnerArray> dx = { { 1, 0, 0 }, { 0, 0, 0 } };
    out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
    std::vector<cl::InnerArray> forw = f.Forward(1, dx, out_str);
    out_str << "Forward sweep result: " << forw << "\n\n";

    // Reverse sweep calculations.
    std::vector<cl::InnerArray> w = { 0, 1, 0 };
    out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
    std::vector<cl::InnerArray> rev = f.Reverse(1, w);
    out_str << "Reverse sweep result: " << rev << "\n\n\n";
}

inline void examples()
{
    std::ofstream of("output.txt");
    CppAD::tape_serializer<cl::InnerArray> serializer(of);
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

#endif // cl_tape_examples_array_examples_hpp
