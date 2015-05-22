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
#include <cl/tape/util/testutil.hpp>
#include <cl/tape/util/testoutput.hpp>

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

namespace
{
    std::ofstream out("..\\..\\..\\..\\..\\output\\en-us\\Tape\\CppAD\\Tests\\AdjointPerformanceTest\\Log.csv");
}


void adjointPerformanceTest()
{
    std::cout << "\nTest adjoint vs finite differences method:" << std::endl;
    out << "Test adjoint vs finite differences method: " << std::endl;

    // Set number of independent variables.
    size_t startSize = 9999;
    size_t maxSize = 10000;

    for (size_t n = startSize; n < maxSize; n++)
    {
        out << "Number of independent variables: " << n << std::endl;
        out << "Number of dependent variables: 1" << std::endl;

        TapeDoubleVector X(n, 2);
        TapeDoubleVector Y(1);
        boost::timer timer;

        // Start timing of tape recording.
        timer.restart();

        // Start taping.
        Independent(X);

        out << "Start of tape recording: " << currentTime() << std::endl;

        // Calculate functions.
        Y[0] = calculate<TapeDouble>(X);

        // End of tape recording. Declare vector Y as dependent variable.
        TapeFunction<double> f(X, Y);

        // Calculate time for tape recording.
        double tapeRecordingTime = timer.elapsed();
        std::cout << "\tTime for tape recording : " << tapeRecordingTime << " s" << std::endl;
        out << "End of tape recording. " << std::endl;
        out << "Time for tape recording: " << tapeRecordingTime << std::endl;

        std::vector<double> sy, sx(n, 0.0), sw(1, 1), sf_Forward(n), sf_Reverse(n);


        // Forward mode.
        timer.restart();

        out << "Start of differentiation in Forward mode: " << currentTime() << std::endl;

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

        out << "End of differentiation in Forward mode." << std::endl;
        out << "Time for Forward mode: " << forwardTime << std::endl;

        // Reverse mode
        timer.restart();

        out << "Start of differentiation in Reverse mode: " << currentTime() << std::endl;

        for (size_t i = 0; i < 10; i++)
            sf_Reverse = f.Reverse(1, sw);
        // Calculate time for reverse mode.
        double reverseTime = timer.elapsed() / 10;
        std::cout << "\tTime for Reverse mode : " << reverseTime << " s" << std::endl;

        out << "End of differentiation in Reverse mode." << std::endl;
        out << "Time for Reverse mode: " << reverseTime << std::endl;

        out << "Start of  differentiation using finite differences method: " << currentTime() << std::endl;

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

        out << "End of differentiation using finite  differences method." << std::endl;
        out << "Time for finite differences method: " << finiteTime << std::endl;

        //Check results.
        bool result = true;
        for (int i = 0; i < n; i++)
        {
            double maxabs = std::max(std::abs(sf_Finite[i]), std::max(std::abs(sf_Forward[i]), std::abs(sf_Reverse[i])));
            double tollerance = 1e-5 * maxabs;
            if (std::abs(sf_Forward[i] - sf_Finite[i]) > tollerance)
            {
                result = false;
                BOOST_ERROR("\nForward mode and Finite difference derivative[" << i << "] mismatch."
                            << "\n    forward mode:      " << sf_Forward[i]
                            << "\n    finite difference: " << sf_Finite[i]
                            << "\n    tolerance:  " << tollerance);
                out << "\nForward mode and Finite difference derivative[" << i << "] mismatch:" << "forward mode" << sf_Forward[i] << "finite difference" << sf_Finite[i] << std::endl;

            }
            if (std::abs(sf_Reverse[i] - sf_Finite[i]) > tollerance)
            {
                result = false;
                BOOST_ERROR("\nReverse mode and Finite difference derivative[" << i << "] mismatch."
                            << "\n    reverse mode:      " << sf_Reverse[i]
                            << "\n    finite difference: " << sf_Finite[i]
                            << "\n    tolerance:  " << tollerance);
                out << "\n Reverse mode and Finite difference derivative[" << i << "] mismatch:" << "reverse mode" << sf_Reverse[i] << "finite difference" << sf_Finite[i] << std::endl;
            }
            if (std::abs(sf_Reverse[i] - sf_Forward[i]) > tollerance)
            {
                result = false;
                BOOST_ERROR("\nForward mode and Reverse mode derivative[" << i << "] mismatch."
                            << "\n    forward mode:      " << sf_Forward[i]
                            << "\n    reverse mode:      " << sf_Reverse[i]
                            << "\n    tolerance:  " << tollerance);
                out << "\nForward mode and Reverse mode derivative[" << i << "] mismatch:" << "forward mode" << sf_Forward[i] << "reverse mode" << sf_Reverse[i] << std::endl;
            }
        }

        if (result)
        {
            out << "Forward, Reverse and Finite diference derivatives:" << std::endl;
            for (int i = 0; i < n; i++)
            {
                out << "dy / dx[" << i << "] : " << sf_Forward[i] << "\t " << sf_Reverse[i] << "\t " << sf_Finite[i] << std::endl;
            }
        }

        out << std::endl << std::endl;
    }
}

void jacobianPerformanceTest()
{
    std::cout << "\nTest adjoint Jacobian vs finite differences method:" << std::endl;

    out << "Test adjoint Jacobian vs finite differences method:" << std::endl;

    // Initialize random number generator.
    srand((unsigned int)time(NULL));

    // Set number of independent variables.
    int sizeof_indep = 1000;

    out << "Number of independent variables: " << sizeof_indep << std::endl;

    // Set number of dependent variables.
    int sizeof_dep = 2;

    out << "Number of dependent variables: " << sizeof_dep << std::endl;

    // Create vector of random doubles (0,1].
    std::vector<double> xd(sizeof_indep);
    for (int i = 0; i < sizeof_indep; i++)
    {
        xd[i] = (std::rand() % 100 + 1) / 100.;
    }

    // Create vector of independent variables.
    TapeDoubleVector X(sizeof_indep);
    for (int i = 0; i < sizeof_indep; i++)
    {
        X[i] = xd[i];
    }

    // Start timing of tape recording.
    boost::timer timer;

    // Start taping. Declare vector X as independent variable.
    Independent(X);

    out << "Start of tape recording: " << currentTime() << std::endl;

    // Create vector of dependent variables.
    TapeDoubleVector Y(sizeof_dep, 0);

    // Calculate functions.
    for (int i = 0; i < sizeof_indep; i++)
    {
        Y[0] += std::exp(-X[i]);
        Y[1] += std::exp(-(X[i] * X[i]));
    }

    // End of tape recording. Declare vector Y as dependent variable.
    TapeFunction<double> f(X, Y);

    double temp_t = timer.elapsed();
    std::cout << "\tTime for tape recording : " << temp_t << " s" << std::endl;
    out << "End of tape recording. " << std::endl;
    out << "Time for tape recording: " << temp_t << std::endl;

    // Start timing for calculating derivatives using adjoint Jacobian.
    timer.restart();

    out << "Start of differentiation using Jacobian: " << currentTime() << std::endl;

    // Compute derivatives using Jacobian.
    std::vector<double> Jacobian = f.Jacobian(xd);

    temp_t = timer.elapsed();
    std::cout << "\tTime for adjoint Jacobian : " << temp_t << " s" << std::endl;

    out << "End of differentiation." << std::endl;
    out << "Time for Jacobian: " << temp_t << std::endl;

    // Start timing for calculating derivatives by finite differences method.
    timer.restart();

    out << "Start of  differentiation using finite differences method: " << currentTime() << std::endl;

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
    std::vector<double> JacobianFinite(sizeof_dep * sizeof_indep);

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
            JacobianFinite[sizeof_indep*j + i] = (yf[j] - yd[j]) / h;
        }
    }

    temp_t = timer.elapsed();
    std::cout << "\tTime for finite differences method : " << temp_t << " s" << std::endl;

    out << "End of differentiation using finite  differences method." << std::endl;
    out << "Time for finite differences method: " << temp_t << std::endl;

    // Check derivatives calculated by adjoint and finite differences method.
    bool result = true;
    for (int i = 0; i < Jacobian.size(); i++)
    {
        double error = std::abs(Jacobian[i] - JacobianFinite[i]);
        double tollerance = 1e-2 * std::max(std::abs(Jacobian[i]), std::abs(JacobianFinite[i]));
        if (error > tollerance)
        {
            result = false;
            BOOST_ERROR("\nAdjoint derivative and finite differences derivative at position " << i << " mismatch."
                        << "\n  adjoint: " << Jacobian[i]
                        << "\n  finite differences method: " << JacobianFinite[i]
                        << "\n  tollerance: " << tollerance);
            out << "\nAdjoint derivative and finite differences derivative at position " << i << " mismatch."
                << "\n  adjoint: " << Jacobian[i]
                << "\n  finite differences method: " << JacobianFinite[i]
                << "\n  tollerance: " << tollerance;
        }
    }
    out << "Derivatives Jacobian adjoint and Jacobian finite differences: " << std::endl;
    if (result)
    {
        for (int i = 0; i < sizeof_indep; i++)
        {
            for (int j = 0; j < sizeof_dep; j++)
            {
                out << "dy[" << j << "] / dx[" << i << "] : "
                    << Jacobian[sizeof_indep*j + i] << " \t"
                    << JacobianFinite[sizeof_indep*j + i] << std::endl;
            }
        }
    }

    out << std::endl << std::endl;
}

BOOST_AUTO_TEST_SUITE(AdjointPerformanceTest)

BOOST_AUTO_TEST_CASE(AdjointPerformanceTest)
{
    adjointPerformanceTest();
}

BOOST_AUTO_TEST_CASE(JacobianPerformanceTest)
{
    jacobianPerformanceTest();
}

BOOST_AUTO_TEST_SUITE_END()