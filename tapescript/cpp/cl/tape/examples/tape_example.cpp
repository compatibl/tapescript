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

#include <stdlib.h>
#include <iostream>

# define CL_BASE_SERIALIZER_OPEN

# define CL_USE_TAPE_SERIALIZATION

# include <map>
# include <fstream>

#include <cl/tape/impl/detail/enable_ad.hpp>

#include <cl/tape/impl/tape_fwd.hpp>

#include <cl/tape/util/testoutput.hpp>

#include <cl/tape/tape.hpp>

#include "impl/utils.hpp"

struct test_stream
{
    test_stream(std::ostream& o) : o_(o)
    {}

    template<typename O>
    test_stream & operator << (std::vector<O > const& v)
    {
        using namespace cl;
        o_ << v;
        std::cout << v;
        return *this;
    }


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
#include "impl/basic_int_examples.hpp"
#include "impl/basic_double_examples.hpp"
#include "impl/basic_tvalue_examples.hpp"
#include "impl/double_examples.hpp"
#include "impl/performance_tests.hpp"
#include "impl/polynomial_regression_examples.hpp"
#include "impl/array_performance_compare.hpp"
#include "impl/qr_decomposition_examples.hpp"
#include "impl/polynomial_regression_examples.hpp"
#include "impl/linear_regression_examples.hpp"
#include "impl/quadratic_regression_examples.hpp"
#include "impl/amc_simulation_examples.hpp"

extern void performance_without_struct();

namespace cl
{
    template <typename > struct acess_members;
    template <typename Ty_>
    struct class_members<acess_members<Ty_> >
    {
        class_members() { set(); }
        ~class_members() { reset(); }

        void set()
        {
            cl::tape_wrapper<Ty_>::check__(true, true, true);
        }

        void reset()
        {
            cl::tape_wrapper<Ty_>::check__(true, true, false);
        }
    };

    typedef class_members<acess_members<double> > enable_diff_checker;

#   define ENABLE_DIFF_CHECKER_SCOPED()             \
    cl::enable_diff_checker __FUNCTION__##_Var;
}

int main()
{
    typedef std::vector<std::pair<std::string, std::function<void()>>> tests_type;

    tests_type tests;
    tests.reserve(50);

    try
    {
//#       define  CL_CERTAIN_TEST cl::basket_pricing_example
#       if defined  CL_CERTAIN_TEST
            tests.push_back({ "Run certain test ..."
                , CL_CERTAIN_TEST });
#       endif

#       if !defined  CL_CERTAIN_TEST

        tests.push_back({ "Run basic_examples see tape data in output/basic_examples_output.txt ..."
            , cl::basic_examples });

        tests.push_back({ "Run basic_int_examples ..."
            , cl::basic_int_examples });

        tests.push_back({ "Run basic_double_examples ..."
            , cl::basic_double_examples });

        tests.push_back({ "Run basic_tvalue_examples ..."
            , cl::basic_tvalue_examples });

        tests.push_back({ "Run array_examples see tape data in output/array_examples_output.txt ..."
            , cl::array_examples });

        tests.push_back({ "Run non_optimized_array_examples see tape data in output/non_optimized_array_examples_output.txt ..."
            , cl::non_optimized_array_examples });

        tests.push_back({ "Run performance_tests see tape data in output/performance_tests_output.txt ..."
            , cl::performance_tests });

        tests.push_back({ "Run array_performance_compare_examples see tape data in output/performance/array_performance_compare_output.txt ..."
            , cl::array_performance_compare_examples });

        tests.push_back({ "Run qr_decomposition_examples see tape data in output / qr_decomposition_examples_output.txt ..."
            , cl::qr_decomposition_examples });

        tests.push_back({ "Run qr_decomposition_examples see tape data in output / qr_decomposition_examples_output.txt ..."
            , cl::qr_decomposition_examples });

        tests.push_back({ "Run performance test without using compatibl_ad_enabled ..."
            , performance_without_struct });

        tests.push_back({ "Run polynomial_regression_examples see tape data in output/performance/polynomial_regression_examples_output.txt ..."
            , cl::polynomial_regression_examples });

        /*tests.push_back({ "Run basket_pricing_example, see output in output/basket_pricing_example_output.txt ..."
            , cl::basket_pricing_example });
            */

        tests.push_back({ "Run linear_regression_examples, see output in output/linear_regression_output.txt ..."
            , cl::linear_regression_examples });

        tests.push_back({ "Run quadratic_regression_examples, see output in output/quadratic_regression_output.txt ..."
            , cl::quadratic_regression_examples });

        tests.push_back({ "Run amc_simulation_examples see tape data in output/basic_examples_output.txt ..."
            , cl::amc_simulation_examples });

#       endif

        for (tests_type::value_type v : tests)
        {
            std::cout << v.first << std::endl;
            try { v.second(); }
            catch (std::exception& e)
            {
                std::cout << "exception in test: " << e.what() << std::endl;

                system("pause");
            }
        }

#       if defined NDEBUG
          system("pause");
#       endif

    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
