/*
Copyright (C) 2015-present CompatibL

Documentation and quant finance examples are available at:

http://www.modval.org/adjoint

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
        // Common variables
        int vector_size = 1000000;
        int repeat_number = 1000;

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
    
        void dot_product_test()
        {
            BOOST_TEST_MESSAGE("Testing performance of tape_wrapper adapter using dot product calculation");

            std::vector<double> leftNative(vector_size);
            std::vector<double> rightNative(vector_size);

            std::vector<tape_wrapper<double>> lefttape_double(vector_size);
            std::vector<tape_wrapper<double>> righttape_double(vector_size);

            srand((unsigned int)time(nullptr));
            for (int i = 0; i < vector_size; ++i)
            {
                lefttape_double[i] = leftNative[i] = rand() % 10 + (rand() % 1000 / 1000.0);
                righttape_double[i] = rightNative[i] = rand() % 10 + (rand() % 1000 / 1000.0);
            }

            double nativeResult = 0;
            tape_wrapper<double> tapeDoubleResult = 0;

            long _0 = GetTickCount();

            for (int i = 0; i < repeat_number; ++i)
                dotProduct(lefttape_double, righttape_double, tapeDoubleResult);
            
            long _1 = GetTickCount();

            for (int i = 0; i < repeat_number; ++i)
                dotProduct(leftNative, rightNative, nativeResult);
            
            long _2 = GetTickCount();

            BOOST_CHECK(tapeDoubleResult == nativeResult);
            
            double tape_wrapper_time = _1 - _0;
            double native_double_time = _2 - _1;

            // Relative difference between tape_wrapper time and native double time
            // should be less than 10%
            BOOST_CHECK(std::abs(tape_wrapper_time - native_double_time) / std::max(tape_wrapper_time, native_double_time) < 0.10);
        }

        void math_functions_test()
        {
            BOOST_TEST_MESSAGE("Testing performance of tape_wrapper adapter using math functions");

            std::vector<double> vecNative(vector_size);
            std::vector<tape_wrapper<double>> vectape_double(vector_size);

            srand((unsigned int)time(nullptr));
            for (int i = 0; i < vector_size; ++i)
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

            BOOST_CHECK(tapeDoubleResult == nativeResult);

            double tape_wrapper_time = _1 - _0;
            double native_double_time = _2 - _1;

            // Relative difference between tape_wrapper time and native double time
            // should be less than 10%
            BOOST_CHECK(std::abs(tape_wrapper_time - native_double_time) / std::max(tape_wrapper_time, native_double_time) < 0.10);
        }

}
    test_suite* ClAdapterPerformanceTestSuite()
    {
        test_suite* suite = BOOST_TEST_SUITE("ClAdapterPerformance test");
        suite->add(BOOST_TEST_CASE(&dot_product_test));
        suite->add(BOOST_TEST_CASE(&math_functions_test));
        return suite;
    }
}
