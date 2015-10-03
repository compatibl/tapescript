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

#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <cl/tape/tape.hpp>
#include <cl/tape/util/testutil.hpp>

namespace
{
    std::ofstream out("..\\..\\..\\..\\..\\output\\en-us\\Tape\\CppAD\\Tests\\ArithmeticPerformanceTest\\Log.csv");
}

using namespace std;

template <typename T>
inline void func(vector<T>& x, vector<T>& y)
{
    y[0] = x[0] + x[1];
    y[1] = x[1] - x[2];
    y[2] = x[2] * x[3];
    y[3] = x[3] / x[0];
}

// Tests performance of arithmetic operations for AD<double> and tape_double
void arithmeticPerformanceTest()
{
    const size_t iterNum = 100000;

    string sh = "Test performance of arithmetic operations for AD<double> and tape_double " + currentTime() + "\n\n";
    cout << endl << sh;
    out << sh;

    // AD<double> time of the start and the stop of tape recording
    boost::timer timer;
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<CppAD::AD<double>> x(4), y(4);
        CppAD::Independent(x);
        CppAD::ADFun<double> f(x, y);
    }
    double cppADStartStopTime = timer.elapsed() / iterNum;

    // tape_double time of the start and the stop of tape recording
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<cl::tape_double> x(4), y(4);
        cl::Independent(x);
        cl::tape_function<double> f(x, y);
    }
    double clStartStopTime = timer.elapsed() / iterNum;

    // AD<double> time of tape recording
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<CppAD::AD<double>> x = { 1.0, 2.0, 3.0, 4.0 }, y(4);
        CppAD::Independent(x);
        func(x, y);
        CppAD::ADFun<double> f(x, y);
    }
    double cppADTapeTime = timer.elapsed() / iterNum;

    // tape_double time of tape recording
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<cl::tape_double> x = { 1.0, 2.0, 3.0, 4.0 }, y(4);
        cl::Independent(x);
        func(x, y);
        cl::tape_function<double> f(x, y);
    }
    double clTapeTime = timer.elapsed() / iterNum;

    // AD<double> time of the forward mode
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<CppAD::AD<double>> x = { 1.0, 2.0, 3.0, 4.0 }, y(4);
        CppAD::Independent(x);
        func(x, y);
        CppAD::ADFun<double> f(x, y);
        vector<double> dx(4, 1.0), dy;
        dy = f.Forward(1, dx);
    }
    double cppADForwardTime = timer.elapsed() / iterNum - cppADTapeTime;

    // tape_double time of the forward mode
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<cl::tape_double> x = { 1.0, 2.0, 3.0, 4.0 }, y(4);
        cl::Independent(x);
        func(x, y);
        cl::tape_function<double> f(x, y);
        vector<double> dx(4, 1.0), dy;
        dy = f.Forward(1, dx);
    }
    double clForwardTime = timer.elapsed() / iterNum - clTapeTime;

    // AD<double> time of the reverse mode
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<CppAD::AD<double>> x = { 1.0, 2.0, 3.0, 4.0 }, y(4);
        CppAD::Independent(x);
        func(x, y);
        CppAD::ADFun<double> f(x, y);
        vector<double> dv(4, 1.0), du;
        du = f.Reverse(1, dv);
    }
    double cppADReverseTime = timer.elapsed() / iterNum - cppADTapeTime;

    // tape_double time of the reverse mode
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<cl::tape_double> x = { 1.0, 2.0, 3.0, 4.0 }, y(4);
        cl::Independent(x);
        func(x, y);
        cl::tape_function<double> f(x, y);
        vector<double> dv(4, 1.0), du;
        du = f.Reverse(1, dv);
    }
    double clReverseTime = timer.elapsed() / iterNum - clTapeTime;

    // AD<double> time of the Jacobian calculation
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<CppAD::AD<double>> x = { 1.0, 2.0, 3.0, 4.0 }, y(4);
        CppAD::Independent(x);
        func(x, y);
        CppAD::ADFun<double> f(x, y);
        vector<double> x0(4, 1.0), y0;
        y0 = f.Jacobian(x0);
    }
    double cppADJacobianTime = timer.elapsed() / iterNum - cppADTapeTime;

    // tape_double time of the Jacobian calculation
    timer.restart();
    for (size_t i = 0; i < iterNum; ++i)
    {
        vector<cl::tape_double> x = { 1.0, 2.0, 3.0, 4.0 }, y(4);
        CppAD::Independent(x);
        func(x, y);
        cl::tape_function<double> f(x, y);
        vector<double> x0(4, 1.0), y0;
        y0 = f.Jacobian(x0);
    }
    double clJacobianTime = timer.elapsed() / iterNum - clTapeTime;

    stringstream ss;
    ss << "\tStart and stop of tape recording" << endl
        << "\tTime for AD<double> : " << cppADStartStopTime << " s" << endl
        << "\tTime for tape_double : " << clStartStopTime << " s" << endl
        << "\tTimes ratio tape_double / AD<double> : " << clStartStopTime / cppADStartStopTime << endl
        << endl
        << "\tTape recording" << endl
        << "\tTime for AD<double> : " << cppADTapeTime << " s" << endl
        << "\tTime for tape_double : " << clTapeTime << " s" << endl
        << "\tTimes ratio tape_double / AD<double> : " << clTapeTime / cppADTapeTime << endl
        << endl
        << "\tForward mode" << endl
        << "\tTime for AD<double> : " << cppADForwardTime << " s" << endl
        << "\tTime for tape_double : " << clForwardTime << " s" << endl
        << "\tTimes ratio tape_double / AD<double> : " << clForwardTime / cppADForwardTime << endl
        << endl
        << "\tReverse mode" << endl
        << "\tTime for AD<double> : " << cppADReverseTime << " s" << endl
        << "\tTime for tape_double : " << clReverseTime << " s" << endl
        << "\tTimes ratio tape_double / AD<double> : " << clReverseTime / cppADReverseTime << endl
        << endl
        << "\tJacobian calculation" << endl
        << "\tTime for AD<double> : " << cppADJacobianTime << " s" << endl
        << "\tTime for tape_double : " << clJacobianTime << " s" << endl
        << "\tTimes ratio tape_double / AD<double> : " << clJacobianTime / cppADJacobianTime << endl
        << endl;
    string const& sout = ss.str();
    cout << sout;
    out << sout;
}

BOOST_AUTO_TEST_CASE(ArithmeticPerformanceTest)
{
    arithmeticPerformanceTest();
}


