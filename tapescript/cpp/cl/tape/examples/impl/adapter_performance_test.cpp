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

# undef CL_TAPE_COMPLEX_ENABLED
//# define cl_tape_impl_doublemath_hpp
# define cl_tape_impl_double_hpp
# define cl_tape_impl_std_fwd_hpp
# define cl_tape_impl_adjointref_hpp

#include "adapter_performance_test.hpp"
#include "double_copy.hpp"
#include <cl/tape/tape.hpp>
#include <Windows.h>

namespace cl
{
    namespace
    {
        // Returns dot product of two vectors.
        inline void dotProduct(std::vector<double> const& lhs, std::vector<double> const& rhs, double & result)
        {
            result = 0;
            std::vector<double>::const_iterator begin = rhs.begin();

            for (double const& lh : lhs)
            {
                result += lh * (*begin);
                ++begin;
            }

        }

        // Returns dot product of two vectors.
        inline void dotProduct(std::vector<tape_wrapper<double>> const& lhs, std::vector<tape_wrapper<double>> const& rhs, tape_wrapper<double> & result)
        {
            result = 0;
            std::vector<tape_wrapper<double>>::const_iterator begin = rhs.begin();

            for (tape_wrapper<double> const& lh : lhs)
            {
                result += lh * (*begin);
                ++begin;
            }
        }

        void dot_product_test(std::ostream& out = std::cout)
        {
            int n = 100000000;
            out << std::endl << "Test performance of tape_wrapper adapter using dot product calculation" << std::endl;
            out << "Dot product of two vectors (size " << n << " ) is being calculated" << std::endl;

            std::vector<double> leftNative(n);
            std::vector<double> rightNative(n);

            std::vector<tape_wrapper<double>> lefttape_double(n);
            std::vector<tape_wrapper<double>> righttape_double(n);

            srand((unsigned int)time(nullptr));
            for (int i = 0; i < n; ++i)
            {
                lefttape_double[i] = leftNative[i] = rand() % 10 + (rand() % 1000 / 1000.0);
                righttape_double[i] = rightNative[i] = rand() % 10 + (rand() % 1000 / 1000.0);
            }

            double nativeResult = 1;
            tape_wrapper<double> tapeDoubleResult = 1;

            int repeats = 100;

            long _0 = GetTickCount();

            for (int i = 0; i < repeats; ++i)
                dotProduct(lefttape_double, righttape_double, tapeDoubleResult);

            long _1 = GetTickCount();

            for (int i = 0; i < repeats; ++i)
                dotProduct(leftNative, rightNative, nativeResult);

            long _2 = GetTickCount();

            if (tapeDoubleResult != nativeResult)
                out << "Results are not the same" << std::endl;

            out << "Calculation end" << std::endl;
            out << "\tTime for tape_wrapper<double> " << int(_1 - _0) << " ticks" << std::endl;
            out << "\tTime for double " << int(_2 - _1) << " ticks" << std::endl;
            out << "\tThe relative difference  " << 1.0 * std::abs(int((_1 - _0) - (_2 - _1))) / std::max(_1 - _0, _2 - _1)
                << std::endl << std::endl;
        }

        void math_functions_test(std::ostream& out = std::cout)
        {
            int n = 100000000;

            out << std::endl << "Test performance of tape_wrapper adapter using math functions" << std::endl;
            out << "Calcilation start" << std::endl;

            std::vector<double> vecNative(n);
            std::vector<tape_wrapper<double>> vectape_double(n);

            srand((unsigned int)time(nullptr));
            for (int i = 0; i < n; ++i)
            {
                vectape_double[i] = vecNative[i] = rand() % 10 + (rand() % 1000 / 1000.0);
            }

            double nativeResult = 1;
            tape_wrapper<double> tapeDoubleResult = 1;

            long _0 = GetTickCount();

            std::for_each(vectape_double.begin(), vectape_double.end(), [&tapeDoubleResult](tape_wrapper<double> item)
                {
                tapeDoubleResult += std::sin(item * std::pow(tapeDoubleResult, std::sqrt(std::pow(item, 2.0) * std::exp(std::cos(tapeDoubleResult * std::tan(item))))));
                });

            long _1 = GetTickCount();

            std::for_each(vecNative.begin(), vecNative.end(), [&nativeResult](double item)
            {
                nativeResult += std::sin(item * std::pow(nativeResult, std::sqrt(std::pow(item, 2.0) * std::exp(std::cos(nativeResult * std::tan(item))))));
            });

            long _2 = GetTickCount();

            if (tapeDoubleResult != nativeResult)
                out << "Results are not the same" << std::endl;

            out << "Calculation end" << std::endl;
            out << "\tTime for tape_wrapper<double> " << int(_1 - _0) << " ticks" << std::endl;
            out << "\tTime for double " << int(_2 - _1) << " ticks" << std::endl;
            out << "\tThe relative difference  " << 1.0 * std::abs(int((_1 - _0) - (_2 - _1))) / std::max(_1 - _0, _2 - _1)
                << std::endl << std::endl;
        }

}
    void adapter_performance_test()
    {
        std::ofstream output("output/adapter_performance_test_output.txt");

        dot_product_test(output);
        math_functions_test(output);
    }
}
