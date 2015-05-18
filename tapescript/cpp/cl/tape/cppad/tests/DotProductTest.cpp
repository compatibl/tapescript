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
#include <cl/tape/tests/testutil.hpp>

static int dimension = (int)1e+7;

static std::vector<double> leftVector = getRandomVector(dimension);
static std::vector<double> rightVector = getRandomVector(dimension);

// Returns dot product of two vectors.
template <typename ResultType, typename LType, typename RType >
inline ResultType dotProduct(std::vector<LType> const& lhs, std::vector<RType> const& rhs)
{
    ResultType result = 0.0;
    typename
        std::vector<RType>::const_iterator begin = rhs.begin();

    for (LType const& lh : lhs)
    {
        result += lh * *begin;
        ++begin;
    }
    return result;
}

// Calculates dot product of two AD<double> vectors and two CppDouble vectors.
// Vectors are not declared as independent.
void performanceTestWithoutTape()
{
    std::cout << std::endl << "Test performance for dot product calculation without tape recording" << std::endl;
    std::vector<CppAD::AD<double>> leftAD = vectorCast<CppAD::AD<double>>(leftVector);
    std::vector<CppAD::AD<double>> rightAD = vectorCast<CppAD::AD<double>>(rightVector);

    std::vector<cl::CppDouble> leftCppDouble = vectorCast<cl::CppDouble>(leftVector);
    std::vector<cl::CppDouble> rightCppDouble = vectorCast<cl::CppDouble>(rightVector);

    CppAD::AD<double> ADResult = 0;
    cl::CppDouble CppDoubleResult = 0;

    boost::timer timer;
    ADResult = dotProduct<CppAD::AD<double>>(leftAD, rightAD);
    double ADtime = timer.elapsed();

    timer.restart();
    CppDoubleResult = dotProduct<cl::CppDouble >(leftCppDouble, rightCppDouble);
    double CppTime = timer.elapsed();

    std::cout << "\tTime for AD<double> " << ADtime << " s" << std::endl;
    std::cout << "\tTime for CppDouble " << CppTime << " s" << std::endl;
    std::cout << "\tThe relative difference  " << 1.0 * std::abs(CppTime - ADtime) / std::max(CppTime, ADtime)
        << std::endl ;

}

// Calculates dot product of two AD<double> vectors and two CppDouble vectors.
// Vectors are declared as independent.
void performanceTestWithTape()
{
    std::cout << std::endl << "Test performance for dot product calculation with tape recording" << std::endl;
    std::vector<CppAD::AD<double>> leftAD = vectorCast<CppAD::AD<double>>(leftVector);
    std::vector<CppAD::AD<double>> rightAD = vectorCast<CppAD::AD<double>>(rightVector);

    std::vector<cl::CppDouble> leftCppDouble = vectorCast<cl::CppDouble>(leftVector);
    std::vector<cl::CppDouble> rightCppDouble = vectorCast<cl::CppDouble>(rightVector);

    CppAD::AD<double> ADResult = 0;
    cl::CppDouble CppDoubleResult = 0;

    CppAD::Independent(leftAD);

    boost::timer timer;
    ADResult = dotProduct<CppAD::AD<double>>(leftAD, rightAD);
    CppAD::ADFun<double> adfun(leftAD, std::vector<CppAD::AD<double>>({ ADResult }));
    double ADtime = timer.elapsed();

    cl::Independent(leftCppDouble);

    timer.restart();
    CppDoubleResult = dotProduct<cl::CppDouble >(leftCppDouble, rightCppDouble);
    cl::CppAdjFun<double> fcl(leftCppDouble, std::vector<cl::CppDouble>({ CppDoubleResult }));
    double CppTime = timer.elapsed();

    std::cout << "\tTime for AD<double> " << ADtime << " s" << std::endl;
    std::cout << "\tTime for CppDouble " << CppTime << " s" << std::endl;
    std::cout << "\tThe relative difference  " << 1.0 * std::abs(CppTime - ADtime) / std::max(CppTime, ADtime)
        << std::endl ;
}

BOOST_AUTO_TEST_SUITE(DotProductTest)

    BOOST_AUTO_TEST_CASE(WithoutTapeRecord)
{
        performanceTestWithoutTape();
}

BOOST_AUTO_TEST_CASE(WithTapeRecord)
{
    performanceTestWithTape();
}

BOOST_AUTO_TEST_SUITE_END()
