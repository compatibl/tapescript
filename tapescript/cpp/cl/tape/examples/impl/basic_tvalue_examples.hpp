/*
Copyright (C) 2015-present CompatibL

Performance test results and finance-specific examples are available at:

http://www.tapescript.org

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

#ifndef cl_basic_tvalue_examples_hpp
#define cl_basic_tvalue_examples_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>

namespace cl
{
    inline void tvalue_add_example()
    {
        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        cl::tvalue x1 = { 0, 1 };
        cl::tvalue a = { 0.5, -1.2 };
        cl::tvalue b = { 1, 1.1 };
        std::vector<cl::tobject> X(2);
        X[0] = x0 + a;
        X[1] = b + x1;

        // Declare the X vector as independent and start tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject  s = 2.5 + X[0] + x0;
        s += X[1] + 1.5;
        cl::tvalue y_exp = { 7.5, 8.9 };
        CL_ASSERT(s == y_exp, "Calculated and expected values are different.");
        std::vector<cl::tobject> Y = { s };

        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 2, 5 }, { 1, -1 } };

        std::vector<cl::tvalue> forw = f.forward(1, dx);

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 3, 7 } };
        std::vector<cl::tvalue> rev = f.reverse(1, w);
    }

    inline void tvalue_minus_example()
    {
        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        cl::tvalue x1 = { 0, 1 };
        cl::tvalue a = { 0.5, -1.2 };
        cl::tvalue b = { 1, 1.1 };
        std::vector<cl::tobject> X(2);
        X[0] = x0 - a;
        X[1] = b - x1;

        // Declare the X vector as independent and start tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject  s = 2.5 - X[0] - x0;
        s -= (X[1] - 1.5);
        cl::tvalue y_exp = { 1.5, -1.3 };
        cl::tvalue err = { 1e-10, 1e-10 };
        CL_ASSERT(std::abs(s - y_exp) < err, "Calculated and expected values are different.");
        std::vector<cl::tobject> Y = { s };

        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 2, 5 }, { 1, -1 } };

        std::vector<cl::tvalue> forw = f.forward(1, dx);

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 3, 7 } };
        std::vector<cl::tvalue> rev = f.reverse(1, w);
    }

    inline void tvalue_mult_example()
    {
        // Input values initialization.
        cl::tvalue x0 = { 1, 2 };
        cl::tvalue x1 = { 0, 1 };
        cl::tvalue a = { 0.5, -1.2 };
        cl::tvalue b = { 1, 1.1 };
        std::vector<cl::tobject> X(2);
        X[0] = x0 * a;
        X[1] = b * x1;

        // Declare the X vector as independent and start tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject  s = 2.0 * X[0] * x0;
        s += X[1] * 1.5;
        cl::tvalue y_exp = {1, -7.95 };
        cl::tvalue err = { 1e-10, 1e-10 };
        CL_ASSERT(std::abs(s - y_exp) < err, "Calculated and expected values are different.");
        std::vector<cl::tobject> Y = { s };

        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 2, 5 }, { 1, -1 } };

        std::vector<cl::tvalue> forw = f.forward(1, dx);

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 3, 7 } };
        std::vector<cl::tvalue> rev = f.reverse(1, w);
    }


    inline void tvalue_div_example()
    {
        // Input values initialization.
        cl::tvalue x0 = { 1, 3 };
        cl::tvalue x1 = { 0, 1.5 };
        cl::tvalue a = { 0.5, -1.2 };
        cl::tvalue b = { 1, 1 };
        std::vector<cl::tobject> X(2);
        X[0] = a / x0;
        X[1] = x1 / b;

        // Declare the X vector as independent and start tape recording.
        cl::tape_start(X);

        // Output calculations.
        cl::tobject  s = 2.0 / X[0] / x0;
        s += X[1] / 1.5;
        cl::tvalue y_exp = { 4, -2.0/3 };
        cl::tvalue err = { 1e-10, 1e-10 };
        CL_ASSERT(std::abs(s - y_exp) < err, "Calculated and expected values are different.");
        std::vector<cl::tobject> Y = { s };

        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { { 2, 5 }, { 1, -1 } };

        std::vector<cl::tvalue> forw = f.forward(1, dx);

        // Reverse sweep calculations.
        std::vector<cl::tvalue> w = { { 3, 7 } };
        std::vector<cl::tvalue> rev = f.reverse(1, w);
    }

    inline void basic_tvalue_examples()
    {
        tvalue_add_example();
        tvalue_minus_example();
        tvalue_mult_example();
        tvalue_div_example();
    }
}
#endif 