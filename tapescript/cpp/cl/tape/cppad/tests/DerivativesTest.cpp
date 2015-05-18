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

#include <cppad/cppad.hpp>
#include <boost/test/unit_test.hpp>
#include <cl/tape/tape.hpp>

using namespace cl;

BOOST_AUTO_TEST_SUITE(DerivativesTest)

BOOST_AUTO_TEST_CASE(Add)
{
    CppAdjVector X(2, 3.0);

    Independent(X);
    CppAdjVector Y(1);
    Y[0] = 2 * X[0] + X[1];
    CppAdjFun<double> f(X, Y);

    // Calculate derivative of Y on X[0] in Forward mode.
    std::vector<double> sy, sx(2, 0.0);
    sx[0] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2.0);

    // Calculate derivative of Y on X[1] in Forward mode.
    sx[0] = 0;
    sx[1] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 1.0);

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK((sy[0] == 2.0) && (sy[1] == 1.0));
}

BOOST_AUTO_TEST_CASE(Mult)
{
    CppAdjVector X(2, 3.0);

    Independent(X);
    CppAdjVector Y(1);
    Y[0] = X[0]*X[1];
    CppAdjFun<double> f(X, Y);

    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(2, 0.0);
    sx[0] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 3.0);

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 3.0);
}

BOOST_AUTO_TEST_CASE(Pow)
{
    CppAdjVector X(1, 3.0);

    Independent(X);
    CppAdjVector Y(1);
    Y[0] = std::pow(X[0], 4);
    CppAdjFun<double> f(X, Y);

    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 4 * pow(3.0, 3));

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 4 * pow(3.0, 3));
}

BOOST_AUTO_TEST_CASE(Exp)
{
    CppAdjVector X(1, 3.0);
    Independent(X);

    CppAdjVector Y(1);
    Y[0] = std::exp(-2.0 * X[0]);

    CppAdjFun<double> f(X, Y);

    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == -2*std::exp(-6.0));

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -2 * std::exp(-6.0));
}

BOOST_AUTO_TEST_CASE(CosSin)
{
    CppAdjVector X(2, 3.0);
    Independent(X);

    CppAdjVector Y(1);
    Y[0] = std::cos(X[0]) + std::sin(X[1]);

    CppAdjFun<double> f(X, Y);
    // Calculate derivative of Y on X[0] in Forward mode.
    std::vector<double> sy, sx(2, 0.0);
    sx[0] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == - std::sin(X[0]));

    // Calculate derivative of Y on X[1] in Forward mode.
    sx[0] = 0;
    sx[1] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == std::cos(X[0]));

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK((sy[0] == -std::sin(X[0])) && (sy[1] == std::cos(X[0])));
}

BOOST_AUTO_TEST_CASE(Max)
{
    CppAdjVector X(1, 3.0);
    Independent(X);

    CppAdjVector Y(1);
    Y[0] = std::max(X[0]*X[0], 2 - X[0]);

    CppAdjFun<double> f(X, Y);

    // Derivatives in x = 3 are correct.
    // Should be: y(x = 3) = max(X * X, 2 - X) = X * X
    // dy/dx(x = 3) = 2*X = 6

    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2*X[0]);

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 2 * X[0]);

    // Derivatives in x = 0 are correspond to y = X * X.
    // Initialize function in x = 0.
    std::vector<double> x(1, 0.0);
    sy = f.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2 * x[0]);

    // Calculate derivatives in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 2 * x[0]);


    // Derivatives in x = 0 are incorrect.
    // Should be: y(x = 0) = max(X * X, 2 - X) = 2 - X
    // dy/dx(x = 0) = -1
    // Initialize function in x = 0.
    x[0] = 0;
    sy = f.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] != -1.0);

    // Calculate derivatives in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] != -1.0);

    // Change form of function.
    X[0] = 0;
    Independent(X);

    Y[0] = std::max(X[0] * X[0], 2 - X[0]);

    CppAdjFun<double> f2(X, Y);

    // Derivatives in x = 0 are correct now.
    // Should be: y(x = 0) = max(X * X, 2 - X) = 2 - X
    // dy/dx(x = 0) = -1
    // Calculate derivative in Forward mode.
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);

    // Calculate derivatives in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);

    // Derivatives in x = 1 are correspond to y = 2 - X.
    x[0] = 1;
    // Initialize function in x = 1
    sy = f2.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);

    // Calculate derivatives in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);
}

BOOST_AUTO_TEST_CASE(Min)
{
    CppAdjVector X(1, 3.0);
    Independent(X);

    CppAdjVector Y(1);
    Y[0] = std::min(X[0] * X[0], 2 - X[0]);

    CppAdjFun<double> f(X, Y);

    // Derivatives in x = 3 are correct.
    // Should be: y(x = 3) = min(X * X, 2 - X) = 2 - X
    // dy/dx(x = 3) = -1
    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);

    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);

    // Derivatives in x = 1 are correspond to y = 2 - X.
    std::vector<double> x(1, 1.0);
    // Initialize function in x = 1.
    sy = f.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);

    // Calculate derivative in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);


    // Derivatives in x = 0 are incorrect.
    // Should be: y(x = 0) = min(X * X, 2 - X) = X * X
    // dy/dx(x = 0) = 2*x = 0
    // Initialize function in x = 0
    x[0] = 0;
    sy = f.Forward(0, x);
    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] != 0.0);

    // Calculate derivative in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] != 0.0);

    // Change form of function.
    X[0] = 0;
    Independent(X);

    Y[0] = std::min(X[0] * X[0], 2 - X[0]);

    CppAdjFun<double> f2(X, Y);

    // Derivatives in x = 0 are correct now.
    // Should be: y(x = 0) = min(X * X, 2 - X) = X * X
    // dy/dx(x = 0) = 2*x = 0
    // Calculate derivative in Forward mode.
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == 0.0);
    // Calculate derivative in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 0.0);

    // Derivatives in x = 1 are correspond to y = X * X.
    x[0] = 1;
    // Initialize function in x = 1.
    sy = f2.Forward(0, x);
    // Calculate derivative in Forward mode.
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2.0);

    // Calculate derivative in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 2.0);


}
BOOST_AUTO_TEST_CASE(MinBound)
{
    CppAdjVector X(1, 1.0);
    CppAdjVector Y(1);

    Independent(X);

    Y[0] = std::min(X[0] * X[0], 2 - X[0]);

    CppAdjFun<double> f(X, Y);

    // Derivatives in x = 1 are correct now.
    // Right operand is chosen.
    // Calculate derivatives in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);

    // Change position of operands.
    Independent(X);

    Y[0] = std::min(2 - X[0], X[0] * X[0]);

    CppAdjFun<double> f2(X, Y);

    // Derivatives in x = 1 are correct now.
    // Right operand is chosen
    // Calculate derivatives in Forward mode
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2.0);

    // Calculate derivatives in Reverse mode
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 2.0);

}


BOOST_AUTO_TEST_CASE(LogBarrier)
{
    double b = 1;
    double H = pow(10, 6);

    CppAdjVector X(1, 0.0);
    Independent(X);

    CppAdjVector Y(1);

     Y[0] = std::min(-std::log(b - X[0]), H);

    CppAdjFun<double> f(X, Y);

    // Derivatives in x = 0 are correct.
    // Should be: y(x = 0) = - log(b - X[0])
    // dy/dx(x = 0) = 1 / (b - x) = 1 / b
    std::vector<double> sy, sx(1, 1.0);
    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 1 / b);

    // Calculate derivative in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 1 / b);

    // Derivatives in x = b - 1 / (std::exp(H)) are correspond to -std::log(b - X[0]).
    // Initialize function in x = b - 1 / (std::exp(H)).
    std::vector<double> x(1, 0.0);
    x[0] = b - 1 / (std::exp(H));
    sy = f.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 1 / (b - x[0]));

    // Calculate derivative in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 1 / (b - x[0]));

    //Change form of function
    X[0] = 2;
    Independent(X);

    Y[0] = std::min(-std::log(b - X[0]), H);

    CppAdjFun<double> f2(X, Y);

    // Derivatives in x = b - 1 / (std::exp(H)) are correspond to H.
    x[0] = b - 1 / (std::exp(H));
    //Initialize function in x = b - 1 / (std::exp(H))
    sy = f.Forward(0, x);
    // Calculate derivative in Forward mode.
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == 0.0);
    // Calculate derivative in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 0.0);

}

BOOST_AUTO_TEST_SUITE_END()