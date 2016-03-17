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

#ifndef cl_basic_double_examples_hpp
#define cl_basic_double_examples_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>

namespace cl
{
    inline void double_add_example()
    {
        // Input values initialization.
        std::vector<tdouble> X(2, 3.0);
        X[0] = X[0] + 1.5;
        X[1] = 2.5 + X[1];
        // Declare the X vector as independent and start tape recording.
        cl::tape_start(X);

        // Output calculations.
        std::vector<tdouble> Y(1);

        Y[0] = 2.5 + X[0];
        Y[0] += X[1] + 1.5;
        tdouble y_exp = 14.0;
        CL_ASSERT(Y[0] == y_exp, "Calculated and expected values are different.");

        // Declare a tape function and stop the tape recording.
        cl::tfunc<double> f(X, Y);

        // Calculate derivative of Y with respect to X[0] in Forward mode.
        std::vector<double> sy_forw, sx(2, 0.0);
        sx[0] = 1;

        sy_forw = f.forward(1, sx);

        // Calculate derivatives in Reverse mode.
        std::vector<double> sy_rev, sw(1, 1.0);

        sy_rev = f.reverse(1, sw);
        CL_ASSERT(sy_forw[0] == sy_rev[0], "Calculated and expected values are different.");
    }

    inline void double_minus_example()
    {
        // Input values initialization.
        std::vector<tdouble> X(2, 3.0);
        X[0] = X[0] - 2.5;
        X[1] = 1.5 - X[1];
        // Declare the X vector as independent and start tape recording.
        cl::tape_start(X);

        // Output calculations.
        std::vector<tdouble> Y(1);

        Y[0] = 2 - X[0];
        Y[0] -= X[1] - 1;
        tdouble y_exp = 4.0;
        CL_ASSERT(Y[0] == y_exp, "Calculated and expected values are different.");

        // Declare a tape function and stop the tape recording.
        cl::tfunc<double> f(X, Y);

        // Calculate derivative of Y with respect to X[0] in Forward mode.
        std::vector<double> sy_forw, sx(2, 0.0);
        sx[0] = 1;

        sy_forw = f.forward(1, sx);

        // Calculate derivatives in Reverse mode.
        std::vector<double> sy_rev, sw(1, 1.0);

        sy_rev = f.reverse(1, sw);
        CL_ASSERT(sy_forw[0] == sy_rev[0], "Calculated and expected values are different.");
    }

    inline void double_mult_example()
    {
        // Input values initialization.
        std::vector<tdouble> X(2, 1.0);
        X[0] = 2.5 * X[0];
        X[1] = X[1] * 3.5;

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        std::vector<tdouble> Y(1);

        Y[0] = 3.5 * X[0];
        Y[0] += X[1] * 2.5;
        tdouble y_exp = 17.5;
        CL_ASSERT(Y[0] == y_exp, "Calculated and expected values are different.");

        // Declare a tape function and stop the tape recording.
        cl::tfunc<double> f(X, Y);

        // Calculate derivative of Y with respect to X[0] in Forward mode.
        std::vector<double> sy_forw, sx(2, 0.0);
        sx[0] = 1;

        sy_forw = f.forward(1, sx);

        // Calculate derivatives in Reverse mode.
        std::vector<double> sy_rev, sw(1, 1.0);

        sy_rev = f.reverse(1, sw);
        CL_ASSERT(sy_forw[0] == sy_rev[0], "Calculated and expected values are different.");
    }


    inline void double_div_example()
    {
        // Input values initialization.
        std::vector<tdouble> X(2, 1.0);
        X[0] = 2.5 / X[0];
        X[1] = X[1] / 4.0;

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        std::vector<tdouble> Y(1);

        Y[0] = 4.0 / X[0];
        Y[0] += X[1] / 2.5;
        tdouble y_exp = 4 / 2.5 + 0.25 / 2.5;
        CL_ASSERT(Y[0] == y_exp, "Calculated and expected values are different.");

        // Declare a tape function and stop the tape recording.
        cl::tfunc<double> f(X, Y);

        // Calculate derivative of Y with respect to X[0] in Forward mode.
        std::vector<double> sy_forw, sx(2, 0.0);
        sx[0] = 1;

        sy_forw = f.forward(1, sx);

        // Calculate derivatives in Reverse mode.
        std::vector<double> sy_rev, sw(1, 1.0);

        sy_rev = f.reverse(1, sw);
        CL_ASSERT(std::abs(sy_forw[0] - sy_rev[0]) < 1e-10, "Calculated and expected values are different.");
    }

    inline void double_pow_example()
    {
        // Input values initialization.
        std::vector<tdouble> X(2, 2.0);
        X[0] = std::pow(2.0, X[0]);
        X[1] = std::pow(X[1], 3.5);

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        std::vector<tdouble> Y(1);

        Y[0] = std::pow(3.5, X[0]);
        Y[0] += std::pow(X[1], 2.0);
        tdouble y_exp = std::pow(3.5, 4.0) + std::pow(2.0, 7.0);
        CL_ASSERT(Y[0] == y_exp, "Calculated and expected values are different.");

        // Declare a tape function and stop the tape recording.
        cl::tfunc<double> f(X, Y);

        // Calculate derivative of Y with respect to X[0] in Forward mode.
        std::vector<double> sy_forw, sx(2, 0.0);
        sx[0] = 1;

        sy_forw = f.forward(1, sx);

        // Calculate derivatives in Reverse mode.
        std::vector<double> sy_rev, sw(1, 1.0);

        sy_rev = f.reverse(1, sw);
        CL_ASSERT(sy_forw[0] == sy_rev[0], "Calculated and expected values are different.");
    }

    inline void double_max_example()
    {
        // Input values initialization.
        std::vector<tdouble> X(2, 2.0);
        X[0] = std::max(-2.0, X[0]);
        X[1] = std::max(X[1], 3.5);

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        std::vector<tdouble> Y(1);

        tdouble y0 = std::max(3.5, X[0]);
        tdouble y1 = std::max(X[1], 1.0);
        Y[0] = std::max(y0, y1);
        tdouble y_exp = std::max(std::max(3.5, 2.0), std::max(3.5, 1.0));
        CL_ASSERT(Y[0] == y_exp, "Calculated and expected values are different.");

        // Declare a tape function and stop the tape recording.
        cl::tfunc<double> f(X, Y);

        // Calculate derivative of Y with respect to X[0] in Forward mode.
        std::vector<double> sy_forw, sx(2, 0.0);
        sx[0] = 1;

        sy_forw = f.forward(1, sx);

        // Calculate derivatives in Reverse mode.
        std::vector<double> sy_rev, sw(1, 1.0);

        sy_rev = f.reverse(1, sw);
        CL_ASSERT(sy_forw[0] == sy_rev[0], "Calculated and expected values are different.");
    }

    inline void double_min_example()
    {
        // Input values initialization.
        std::vector<tdouble> X(2, 2.0);
        X[0] = std::min(-2.0, X[0]);
        X[1] = std::min(X[1], 3.5);

        // Declare the X vector as independent and start a tape recording.
        cl::tape_start(X);

        // Output calculations.
        std::vector<tdouble> Y(1);

        tdouble y0 = std::min(3.5, X[0]);
        tdouble y1 = std::min(X[1], 1.0);
        Y[0] = std::min(y0, y1);
        tdouble y_exp = std::min(std::min(3.5, -2.0), std::min(2.0, 1.0));
        CL_ASSERT(Y[0] == y_exp, "Calculated and expected values are different.");

        // Declare a tape function and stop the tape recording.
        cl::tfunc<double> f(X, Y);

        // Calculate derivative of Y with respect to X[0] in Forward mode.
        std::vector<double> sy_forw, sx(2, 0.0);
        sx[0] = 1;

        sy_forw = f.forward(1, sx);

        // Calculate derivatives in Reverse mode.
        std::vector<double> sy_rev, sw(1, 1.0);

        sy_rev = f.reverse(1, sw);
        CL_ASSERT(sy_forw[0] == sy_rev[0], "Calculated and expected values are different.");
    }

    inline void basic_double_examples()
    {
        double_add_example();
        double_minus_example();
        double_mult_example();
        double_div_example();
        double_pow_example();
        double_max_example();
        double_min_example();
    }
}
#endif 