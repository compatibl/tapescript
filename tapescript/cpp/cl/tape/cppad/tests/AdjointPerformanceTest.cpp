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
#include <time.h>
#include <boost/timer.hpp>
#include <cl/tape/tape.hpp>

using namespace cl;

template<typename Type>
Type calculate(std::vector<Type>& X)
{
    size_t n = X.size();
    Type sum = 0;
    for (size_t i = 1; i < n; i++)
        sum += X[i] * X[i];
    Type result = X[0] * std::exp(-std::sqrt(sum));
    return result;
}

void adjointPerformanceTest()
{

    std::cout << "\nTest adjoint vs finite differences method:" << std::endl;
    // Set number of independent variables.
    size_t n = 10000;

    CppAdjVector X(n, 2);
    CppAdjVector Y(1);
    boost::timer timer;

    // Start timing of tape recording.
    timer.restart();

    // Start taping.
    Independent(X);

    // Calculate functions.
    Y[0] = calculate<CppDouble>(X);

    // End of tape recording. Declare vector Y as dependent variable.
    CppAdjFun<double> f(X, Y);

    // Calculate time for tape recording.
    double tapeRecordingTime = timer.elapsed();
    std::cout << "\tTime for tape recording : " << tapeRecordingTime << " s" << std::endl;
    std::vector<double> sy, sx(n, 0.0), sw(1, 1), sf_Forward(n), sf_Reverse(n);

    // Forward mode.
    timer.restart();
    for (size_t i = 0; i < n; i++)
    {
        sx[i] = 1;
        sy = f.Forward(1, sx);
        sf_Forward[i] = sy[0];
        sx[i] = 0;
    }
    // Calculate time for forward mode.
    double forwardTime = timer.elapsed();
    std::cout << "\tTime for Forward mode : " << forwardTime << " s" << std::endl;

    // Reverse mode
    timer.restart();
    for (size_t i = 0; i < 10; i++)
        sf_Reverse = f.Reverse(1, sw);
    // Calculate time for reverse mode.
    double reverseTime = timer.elapsed()/10;
    std::cout << "\tTime for Reverse mode : " << reverseTime << " s" << std::endl;

    //Finite differences
    timer.restart();
    std::vector<double> x(n, 2.0), sf_Finite(n);
    double h = 0.001;
    double right, left;
    for (size_t i = 0; i < n; i++)
    {
        //Calculate function with shifting x on value +h
        x[i] += h;
        right = calculate<double>(x);
        //Calculate function with shifting x on value -h
        x[i] -= 2 * h;
        left = calculate<double>(x);
        //Calculate central finite differences.
        sf_Finite[i] = (right - left) / (2 * h);
        x[i] += h;
    }
    // Calculate time for finite differences.
    double finiteTime = timer.elapsed();
    std::cout << "\tTime for Finite differences : " << finiteTime << " s" << std::endl;
    //Check results.
    for (size_t i = 0; i < n; i++)
    {
        BOOST_CHECK_CLOSE(sf_Forward[i], sf_Finite[i], 1e-5);
        BOOST_CHECK_CLOSE(sf_Forward[i], sf_Reverse[i], 1e-5);
        BOOST_CHECK_CLOSE(sf_Reverse[i], sf_Finite[i], 1e-5);
    }
}

void JacobianTest()
{
    std::cout << "\nTest adjoint Jacobian vs finite differences method:" << std::endl;

    // Initialize random number generator.
    srand((unsigned int)time(NULL));

    // Set number of independent variables.
    int sizeof_indep = 5000;

    // Set number of dependent variables.
    int sizeof_dep = 2;

    // Create vector of random doubles (0,1].
    std::vector<double> xd(sizeof_indep);
    for (int i = 0; i < sizeof_indep; i++)
    {
        xd[i] = (std::rand() % 100 + 1) / 100.;
    }

    // Create vector of independent variables.
    CppAdjVector X(sizeof_indep);
    for (int i = 0; i < sizeof_indep; i++)
    {
        X[i] = xd[i];
    }

    // Start timing of tape recording.
    boost::timer timer;

    // Start taping. Declare vector X as independent variable.
    Independent(X);

    // Create vector of dependent variables.
    CppAdjVector Y(sizeof_dep, 0);

    // Calculate functions.
    for (int i = 0; i < sizeof_indep; i++)
    {
        Y[0] += std::exp(-X[i]);
        Y[1] += std::exp(-(X[i] * X[i]));
    }

    // End of tape recording. Declare vector Y as dependent variable.
    CppAdjFun<double> f(X, Y);

    std::cout << "\tTime for tape recording : " << timer.elapsed() << " s" << std::endl;

    // Start timing for calculating derivatives using adjoint Jacobian.
    timer.restart();

    // Compute derivatives using Jacobian.
    std::vector<double> Jacobian = f.Jacobian(xd);

    std::cout << "\tTime for adjoint Jacobian : " << timer.elapsed() << " s" << std::endl;

    // Start timing for calculating derivatives by finite differences method.
    timer.restart();

    // Set step for finite difference method.
    double h = 1e-8;

    std::vector<double> yd(sizeof_dep, 0);

    // Calculate functions.
    for (int i = 0; i < sizeof_indep; i++)
    {
        yd[0] += std::exp(-xd[i]);
        yd[1] += std::exp(-(xd[i] * xd[i]));
    }

    // Create vector for Jacobian calculated using finite differences method.
    std::vector<double> JacobianFinit(sizeof_dep * sizeof_indep);

    for (int i = 0; i < sizeof_indep; i++)
    {
        // Create vector for values of recalculated functions.
        std::vector<double> yf(sizeof_dep, 0);

        xd[i] += h;

        // Recalculate functions.
        for (int j = 0; j < sizeof_indep; j++)
        {
            yf[0] += std::exp(-xd[j]);
            yf[1] += std::exp(-(xd[j] * xd[j]));
        }

        xd[i] -= h;

        for (int j = 0; j < sizeof_dep; j++)
        {
            //Evaluate derivative using finite differences method.
            JacobianFinit[sizeof_indep*j + i] = (yf[j] - yd[j]) / h;
        }
    }

    std::cout << "\tTime for finite differences method : " << timer.elapsed() << " s" << std::endl;

    // Check derivatives calculated by adjoint and finite differences method.
    for (int i = 0; i < Jacobian.size(); i++)
    {
        double error = std::abs(Jacobian[i] - JacobianFinit[i]);
        double tollerance = 1e-2 * std::max(std::abs(Jacobian[i]), std::abs(JacobianFinit[i]));
        if (error > tollerance)
        {
            BOOST_ERROR("\nAdjoint derivative and finite differences derivative at position " << i << " mismatch."
                        << "\n  adjoint: " << Jacobian[i]
                        << "\n  finite differences method: " << JacobianFinit[i]
                        << "\n  tollerance: " << tollerance);
        }
    }
}

BOOST_AUTO_TEST_SUITE(AdjointPerformanceTest)

BOOST_AUTO_TEST_CASE(adjPerformTest)
{
    adjointPerformanceTest();
}

BOOST_AUTO_TEST_CASE(jacobianTest)
{
    JacobianTest();
}

BOOST_AUTO_TEST_SUITE_END()