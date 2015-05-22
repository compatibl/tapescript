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
#include <cl/tape/util/testutil.hpp>
#include <cl/tape/util/testoutput.hpp>

using namespace cl;
namespace
{
    std::ofstream output("..\\..\\..\\..\\..\\output\\en-us\\Tape\\CppAD\\Tests\\DerivativesTest\\Log.csv");
}

BOOST_AUTO_TEST_SUITE(DerivativesTest)


BOOST_AUTO_TEST_CASE(AddDerivativesTest)
{
    TapeDoubleVector X(2, 3.0);

    Independent(X);
    TapeDoubleVector Y(1);
    Y[0] = 2 * X[0] + X[1];
    output  << "\nFunction Y =  2 * X[0] + X[1] is being tested at X[0] = " << X[0] << ", X[1] = " << X[1]<<std::endl;
    TapeFunction<double> f(X, Y);

    // Calculate derivative of Y on X[0] in Forward mode.
    std::vector<double> sy, sx(2, 0.0);
    sx[0] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0]) = " << sy[0] << std::endl;

    // Calculate derivative of Y on X[1] in Forward mode.
    sx[0] = 0;
    sx[1] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 1.0);
    output  << "Derivative of Y on X[1] in Forward mode calculated successfully. dY / dX (X[1]) = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK((sy[0] == 2.0) && (sy[1] == 1.0));
    output  << "Derivative of Y on X in Reverse mode calculated successfully. dY / dX (X[0]) = " << sy[0] << ", dY / dX (X[1]) = " << sy[1] << std::endl;
}

BOOST_AUTO_TEST_CASE(MultDerivativesTest)
{
    TapeDoubleVector X(2, 3.0);

    Independent(X);
    TapeDoubleVector Y(1);
    Y[0] = X[0]*X[1];
    output  << "\nFunction Y =  X[0] * X[1] is being tested at X[0] = " << X[0] << ", X[1] = " << X[1] << std::endl;
    TapeFunction<double> f(X, Y);

    // Calculate derivative of Y on X[0] in Forward mode.
    std::vector<double> sy, sx(2, 0.0);
    sx[0] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 3.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0]) = " << sy[0] << std::endl;

    // Calculate derivative of Y on X[1] in Forward mode.
    sx[0] = 0;
    sx[1] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 3.0);
    output  << "Derivative of Y on X[1] in Forward mode calculated successfully. dY / dX (X[1]) = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 3.0);
    output  << "Derivative of Y on X in Reverse mode calculated successfully. dY / dX (X[0]) = " << sy[0] << ", dY / dX (X[1]) = " << sy[1] << std::endl;
}

BOOST_AUTO_TEST_CASE(PowDerivativesTest)
{
    TapeDoubleVector X(1, 3.0);

    Independent(X);
    TapeDoubleVector Y(1);
    Y[0] = std::pow(X[0], 4);
    output  << "\nFunction Y =  X[0]^4 is being tested at X[0] = " << X[0] << std::endl;
    TapeFunction<double> f(X, Y);

    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 4 * pow(3.0, 3));
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0]) = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 4 * pow(3.0, 3));
    output  << "Derivative of Y on X in Reverse mode calculated successfully. dY / dX (X[0]) = " << sy[0] << std::endl;
}

BOOST_AUTO_TEST_CASE(ExpDerivativesTest)
{
    TapeDoubleVector X(1, 3.0);
    Independent(X);

    TapeDoubleVector Y(1);
    Y[0] = std::exp(-2.0 * X[0]);
    output  << "\nFunction Y =  exp(-2.0 * X[0]) is being tested at X[0] = " << X[0] << std::endl;
    TapeFunction<double> f(X, Y);

    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == -2*std::exp(-6.0));
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0]) = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -2 * std::exp(-6.0));
    output  << "Derivative of Y on X in Reverse mode calculated successfully. dY / dX (X[0]) = " << sy[0] << std::endl;
}

BOOST_AUTO_TEST_CASE(CosSinDerivativesTest)
{
    TapeDoubleVector X(2, 3.0);
    Independent(X);

    TapeDoubleVector Y(1);
    Y[0] = std::cos(X[0]) + std::sin(X[1]);
    output  << "\nFunction Y = cos(X[0]) + sin(X[1]) is being tested at X[0] = " << X[0] << ", X[1] = " << X[1] << std::endl;

    TapeFunction<double> f(X, Y);
    // Calculate derivative of Y on X[0] in Forward mode.
    std::vector<double> sy, sx(2, 0.0);
    sx[0] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == - std::sin(X[0]));
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0]) = " << sy[0] << std::endl;

    // Calculate derivative of Y on X[1] in Forward mode.
    sx[0] = 0;
    sx[1] = 1;
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == std::cos(X[0]));
    output  << "Derivative of Y on X[1] in Forward mode calculated successfully. dY / dX (X[1]) = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK((sy[0] == -std::sin(X[0])) && (sy[1] == std::cos(X[0])));
    output  << "Derivative of Y on X in Reverse mode calculated successfully. dY / dX (X[0]) = " << sy[0] << ", dY / dX (X[1]) = " << sy[1] << std::endl;
}

BOOST_AUTO_TEST_CASE(MaxDerivativesTest)
{
    TapeDoubleVector X(1, 3.0);
    Independent(X);

    TapeDoubleVector Y(1);
    Y[0] = std::max(X[0]*X[0], 2 - X[0]);
    output  << "\nFunction Y = max(X[0]*X[0], 2 - X[0]) is being tested at X[0] = " << X[0] << std::endl;

    TapeFunction<double> f(X, Y);
    output  << "Sequence of operations in a tape is fixed according to the value of X[0]\n"<<
        "Y = max(X[0]*X[0], 2 - X[0]) =  X[0]*X[0]\n"<<
        "because X[0]*X[0] > 2 - X[0] at X[0] = " << X[0] << std::endl;

    // Derivatives in x = 3 are correct.
    // Should be: y(x = 3) = max(X * X, 2 - X) = X * X
    // dy/dx(x = 3) = 2*X = 6

    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2*X[0]);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = "<< X[0] <<") = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 2 * X[0]);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Derivatives in x = 1 are correspond to y = X * X.
    // Initialize function in x = 1.

    std::vector<double> x(1, 1.0);
    sy = f.Forward(0, x);
    output  << "Check derivative of Y at X[0] =" << x[0] << std::endl;
    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2 * x[0]);
    output  << "Derivative is correspond to Y = X[0]*X[0]" << std::endl;
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 2 * x[0]);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Derivatives in x = 0 are incorrect.
    // Should be: y(x = 0) = max(X * X, 2 - X) = 2 - X
    // dy/dx(x = 0) = -1
    // Initialize function in x = 0.
    x[0] = 0;
    output  << "Check derivative of Y at X[0] = " << x[0] << std::endl;
    sy = f.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] != -1.0);
    output  << "Derivative is correspond to Y = X[0]*X[0], but for X[0] = " << x[0] << " Y should be 2 - X[0]" << std::endl;
    output  << "Derivative of Y on X[0] in Forward mode calculated unsuccessfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] != -1.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated unsuccessfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    output  << "For successflul calculation form of function should be changed." << std::endl;

    // Change form of function.
    X[0] = 0;
    output  << "Function Y = max(X[0]*X[0], 2 - X[0]) is being tested at X[0] = " << X[0] << std::endl;

    Independent(X);
    Y[0] = std::max(X[0] * X[0], 2 - X[0]);

    TapeFunction<double> f2(X, Y);
    output  << "Sequence of operations in a tape is fixed according to the value of X[0]\n" <<
        "Y = max(X[0]*X[0], 2 - X[0]) =  2 - X[0]\n" <<
        "because X[0]*X[0] < 2 - X[0] at X[0] = " << X[0] << std::endl;

    // Derivatives in x = 0 are correct now.
    // Should be: y(x = 0) = max(X * X, 2 - X) = 2 - X
    // dy/dx(x = 0) = -1
    // Calculate derivative in Forward mode.
    output  << "Check derivative of Y at X[0] =" << X[0] << std::endl;
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Derivatives in x = 1 are correspond to y = 2 - X.
    x[0] = 1;
    output  << "Check derivative of Y at X[0] =" << x[0] << std::endl;
    // Initialize function in x = 1
    sy = f2.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative is correspond to Y = 2 - X[0]." << std::endl;
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;
}

BOOST_AUTO_TEST_CASE(MinDerivativesTest)
{
    TapeDoubleVector X(1, 3.0);
    Independent(X);

    TapeDoubleVector Y(1);
    Y[0] = std::min(X[0] * X[0], 2 - X[0]);
    output  << "\nFunction Y = min(X[0]*X[0], 2 - X[0]) is being tested at X[0] = " << X[0] << std::endl;

    TapeFunction<double> f(X, Y);
    output  << "Sequence of operations in a tape is fixed according to the value of X[0]\n" <<
        "Y = min(X[0]*X[0], 2 - X[0]) =  2 - X[0]\n" <<
        "because X[0]*X[0] > 2 - X[0] at X[0] = " << X[0] << std::endl;

    // Derivatives in x = 3 are correct.
    // Should be: y(x = 3) = min(X * X, 2 - X) = 2 - X
    // dy/dx(x = 3) = -1
    // Calculate derivative in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Derivatives in x = 1 are correspond to y = 2 - X.
    std::vector<double> x(1, 1.0);
    output  << "Check derivative of Y at X[0] =" << x[0] << std::endl;
    // Initialize function in x = 1.
    sy = f.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative is correspond to Y = 2 - X[0]" << std::endl;
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Calculate derivative in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Derivatives in x = 0 are incorrect.
    // Should be: y(x = 0) = min(X * X, 2 - X) = X * X
    // dy/dx(x = 0) = 2*x = 0
    // Initialize function in x = 0
    x[0] = 0;
    output  << "Check derivative of Y at X[0] =" << x[0] << std::endl;
    sy = f.Forward(0, x);
    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] != 0.0);
    output  << "Derivative is correspond to Y = 2 - X[0], but for X[0] = " << x[0] << " Y should be X[0] * X[0]"<< std::endl;
    output  << "Derivative of Y on X[0] in Forward mode calculated unsuccessfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Calculate derivative in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] != 0.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated unsuccessfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Change form of function.
    output  << "For successflul calculation form of function should be changed." << std::endl;
    X[0] = 0;
    Independent(X);

    Y[0] = std::min(X[0] * X[0], 2 - X[0]);
    output  << "Function Y = max(X[0]*X[0], 2 - X[0]) is being tested at X[0] = " << X[0] << std::endl;

    TapeFunction<double> f2(X, Y);
    output  << "Sequence of operations in a tape is fixed according to the value of X[0]\n" <<
        "Y = min(X[0]*X[0], 2 - X[0]) =  X[0]*X[0]\n" <<
        "because X[0]*X[0] > 2 - X[0] at X[0] = " << X[0] << std::endl;

    // Derivatives in x = 0 are correct now.
    // Should be: y(x = 0) = min(X * X, 2 - X) = X * X
    // dy/dx(x = 0) = 2*x = 0
    // Calculate derivative in Forward mode.
    output  << "Check derivative of Y at X[0] =" << X[0] << std::endl;
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == 0.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Calculate derivative in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 0.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Derivatives in x = 1 are correspond to y = X * X.
    x[0] = 1;
    output  << "Check derivative of Y at X[0] =" << x[0] << std::endl;
    // Initialize function in x = 1.
    sy = f2.Forward(0, x);
    // Calculate derivative in Forward mode.
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2.0);
    output  << "Derivative is correspond to Y = X[0]*X[0]" << std::endl;
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

    // Calculate derivative in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 2.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;


}

BOOST_AUTO_TEST_CASE(MinBoundDerivativesTest)
{
    TapeDoubleVector X(1, 1.0);
    TapeDoubleVector Y(1);

    Independent(X);

    Y[0] = std::min(X[0] * X[0], 2 - X[0]);

    output  << "\nFunction Y = min(X[0]*X[0], 2 - X[0]) is being tested at X[0] = " << X[0] << std::endl;

    TapeFunction<double> f(X, Y);
    output  << "Sequence of operations in a tape is fixed according to the value of X[0].\n" <<
        "As at X[0] = 1 right and left operand are equal, " <<
        "Y = min(X[0]*X[0], 2 - X[0]) =  2 - X[0]\n" <<
        "due to the realization of min function."<< std::endl;

    // Derivatives in x = 1 are correct.
    // Right operand is chosen.
    // Calculate derivatives in Forward mode.
    std::vector<double> sy, sx(1, 1.0);
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Calculate derivatives in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == -1.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Change position of operands.
    output  << "Positions of operands are changed. "<< std::endl;
    Independent(X);

    Y[0] = std::min(2 - X[0], X[0] * X[0]);
    output  << "Function Y = min(2 - X[0], X[0]*X[0]) is being tested at X[0] = " << X[0] << std::endl;
    TapeFunction<double> f2(X, Y);

    output  << "Sequence of operations in a tape is fixed according to the value of X[0].\n" <<
        "As at X[0] = 1 right and left operand are equal, " <<
        "Y = min(2 - X[0], X[0]*X[0]) = X[0]*X[0]\n" <<
        "due to the realization of min function." << std::endl;
    // Derivatives in x = 1 are correct now.
    // Right operand is chosen
    // Calculate derivatives in Forward mode
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == 2.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;
    // Calculate derivatives in Reverse mode
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 2.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;
}

BOOST_AUTO_TEST_CASE(LogBarrierDerivativesTest)
{
    double b = 1;
    double H = pow(10, 6);

    TapeDoubleVector X(1, 0.0);
    Independent(X);

    TapeDoubleVector Y(1);

    Y[0] = std::min(-std::log(b - X[0]), H);
    output  << "\nFunction Y = min(-log(b - X[0]), H), b = "<< b <<", H = "<< H << " is being tested at X[0] = " << X[0] << std::endl;
    TapeFunction<double> f(X, Y);

    output  << "Sequence of operations in a tape is fixed according to the value of X[0].\n" <<
        "Y = min(-log(b - X[0]), H) =  -log(b - X[0]),\n" <<
        "because -log(b - X[0]) < H at X[0] = " << X[0] << std::endl;
    // Derivatives in x = 0 are correct.
    // Should be: y(x = 0) = - log(b - X[0])
    // dy/dx(x = 0) = 1 / (b - x) = 1 / b
    std::vector<double> sy, sx(1, 1.0);
    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    BOOST_CHECK(sy[0] == 1 / b);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Calculate derivative in Reverse mode.
    std::vector<double> sw(1, 1.0);
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 1 / b);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << X[0] << ") = " << sy[0] << std::endl;

    // Derivatives in x = b - 1 / (std::exp(H)) are correspond to -std::log(b - X[0]).
    // Initialize function in x = b - 1 / (std::exp(H)).
    std::vector<double> x(1, 0.0);
    x[0] = b - 1 / (std::exp(10));
    output  << "Check derivative of Y at X[0] = b - 1 / exp(10) = " << x[0] << std::endl;
    sy = f.Forward(0, x);

    // Calculate derivative in Forward mode.
    sy = f.Forward(1, sx);
    double exp = 1 / (b - x[0]);
    BOOST_CHECK(sy[0] == 1 / (b - x[0]));
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] <<" (should be big due to division by little number)"<< std::endl;

    // Calculate derivative in Reverse mode.
    sy = f.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 1 / (b - x[0]));
    output << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << " (should be big due to division by little number)" << std::endl;

    //Change form of function
    X[0] = 2;
    Independent(X);

    Y[0] = std::min(-std::log(b - X[0]), H);

    output  << "Function Y = min(-log(b - X[0]), H), b = " << b << ", H = " << H << " is being tested at X[0] = " << X[0] << std::endl;

    TapeFunction<double> f2(X, Y);
    output  << "Sequence of operations in a tape is fixed according to the value of X[0].\n" <<
        "Y = min(-log(b - X[0]), H) =  H,\n" <<
        "because -log(b - X[0]) < H at X[0] = " << X[0] << std::endl;

    // Derivatives in x = b - 1 / (std::exp(H)) are correspond to H.
    x[0] = b - 1 / (std::exp(H));
    output  << "Check derivative of Y at X[0] = b - 1 / exp(H) = " << x[0] << std::endl;
    //Initialize function in x = b - 1 / (std::exp(H))
    sy = f.Forward(0, x);
    // Calculate derivative in Forward mode.
    sy = f2.Forward(1, sx);
    BOOST_CHECK(sy[0] == 0.0);
    output  << "Derivative of Y on X[0] in Forward mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;
    // Calculate derivative in Reverse mode.
    sy = f2.Reverse(1, sw);
    BOOST_CHECK(sy[0] == 0.0);
    output  << "Derivative of Y on X[0] in Reverse mode calculated successfully. dY / dX (X[0] = " << x[0] << ") = " << sy[0] << std::endl;

}

BOOST_AUTO_TEST_SUITE_END()