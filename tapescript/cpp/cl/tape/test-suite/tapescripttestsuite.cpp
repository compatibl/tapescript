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

//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_MAIN


#include <boost/test/unit_test.hpp>
#include <boost/test/impl/unit_test_main.ipp>

using namespace boost::unit_test_framework;

#include "impl/adapter_arithmetic_test.hpp"
#include "impl/adapter_performance_test.hpp"


test_suite* init_unit_test_suite(int, char*[])
{
    std::string header =
        "Testing Cl.Tape.Cpp11.2013.x64";
    std::string rule = std::string(35, '=');

    BOOST_TEST_MESSAGE(rule);
    BOOST_TEST_MESSAGE(header);
    BOOST_TEST_MESSAGE(rule);
    test_suite* test = BOOST_TEST_SUITE("Cl tapescript test suite");
    
    test->add(cl::ClAdapterArithmeticTestSuite());
#ifdef NDEBUG
    test->add(cl::ClAdapterPerformanceTestSuite());
#endif
        
    return test;
}



