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

#include <stdlib.h>
#include <iostream>

# define CL_BASE_SERIALIZER_OPEN

#include <cl/tape/tape.hpp>

#include "impl\utils.hpp"

struct test_stream
{
    test_stream(std::ostream& o) : o_(o)
    {}

    template<typename O>
    test_stream & operator << (O const& v)
    {
        using namespace cl;
        o_ << v;
        std::cout << v;
        return *this;
    }

    test_stream& operator<<(std::ostream& (*f)(std::ostream&))
    {
        f(o_);
        f(std::cout);

        return *this;
    }

    std::ostream& o_;
};

#  define out_str test_stream(out_stream)

#include <boost/timer.hpp>

#include "impl/array_examples.hpp"
#include "impl/basic_examples.hpp"
#include "impl/double_examples.hpp"
#include "impl/performance_tests.hpp"
#include "impl/regression_examples.hpp"
#include "impl/quadratic_regression_examples.hpp"

int main()
{
    std::cout << "Run basic_examples see tape data in output/basic_examples_output.txt ..." << std::endl;
    cl::basic_examples();

    std::cout << std::endl << "Run array_examples see tape data in output/array_examples_output.txt ..." << std::endl;
    cl::array_examples();

    std::cout << std::endl << "Run non_optimized_array_examples see tape data in output/non_optimized_array_examples_output.txt ..." << std::endl;
    cl::non_optimized_array_examples();

    std::cout << std::endl << "Run performance_tests see tape data in output/performance_tests_output.txt ..." << std::endl;
    cl::performance_tests();

    std::cout << std::endl << "Run regression_examples see tape data in output/regression_examples_output.txt ..." << std::endl;
    cl::regression_examples();

    std::cout << std::endl << "Run quadratic_regression_examples see tape data in output/quadratic_regression_examples_output.txt ..." << std::endl;
    cl::quadratic_regression_examples();

    system("pause");
    return 0;
}
