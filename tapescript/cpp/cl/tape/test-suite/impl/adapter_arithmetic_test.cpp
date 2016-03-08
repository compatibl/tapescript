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

#include "adapter_arithmetic_test.hpp"
#include "double_copy.hpp"
#include <cl/tape/tape.hpp>

namespace cl
{
    
    namespace    
    {
        void arithmetic_test()
        {
            BOOST_TEST_MESSAGE("Testing simple arithmetic operations with tape_wrapper adapter");

            tape_wrapper<double> a = 3.0;
            tape_wrapper<double> b = 4.0;

            BOOST_CHECK(a + b == 7.0);
            BOOST_CHECK(a - b == -1.0);
            BOOST_CHECK(a * b == 12.0);
            BOOST_CHECK(a / b == 0.75);
        }
    }
    
    test_suite* ClAdapterArithmeticTestSuite()
    {
        test_suite* suite = BOOST_TEST_SUITE("ClAdapterArithmetic test");
        suite->add(BOOST_TEST_CASE(&arithmetic_test));
        return suite;
    }
}
