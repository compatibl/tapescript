/*
Copyright (C) 2015-present CompatibL

Performance test results and finance-specific examples are available at:

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

#ifndef cl_array_performance_compare_hpp
#define cl_array_performance_compare_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include "impl/utils.hpp"

namespace cl
{
    // Compare performance plus different arrays.
    inline void array_performance_compare_plus_example(std::ostream& out_stream = std::cout)
    {
        int n = 10000000;
        double* arr = new double[n];


        cl::tvalue x0 = cl::tvalue(std::valarray<double>(arr, n));

        cl::tobject xObj0(x0);
        CppAD::AD<tape_inner<tape_array>> ccpAdArray0 = CppAD::AD<tape_inner<tape_array>>(x0);


        boost::timer timer1;
        cl::tobject& xObj1 = cl::tobject(xObj0 + 0.5);
        double time1 = timer1.elapsed();
        out_str << "tobject performance result: " << time1 << std::endl;


        boost::timer timer2;
        cl::tvalue& x1 = cl::tvalue(x0 + 0.5);
        double time2 = timer2.elapsed();
        out_str << "tvalue performance result: " << time2 << std::endl;

        boost::timer timer3;
        CppAD::AD<tape_inner<tape_array>>& ccpAdArray1 = CppAD::AD<tape_inner<tape_array>>(ccpAdArray0 + 0.5);
        double time3 = timer3.elapsed();
        out_str << "CppAD::AD<tape_inner<tape_array>> performance result: " << time3 << std::endl;


        delete[] arr;
    }


    inline void array_performance_compare_examples()
    {
        std::ofstream of("output/performance/array_performance_compare_output.txt");
        cl::tape_serializer<cl::tvalue> serializer(of);
        serializer.precision(3);

        array_performance_compare_plus_example(serializer);
    }
}

#endif // cl_array_performance_compare_hpp
