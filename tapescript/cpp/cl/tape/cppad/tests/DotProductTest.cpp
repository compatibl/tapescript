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
#include <cl/tape/util/testutil.hpp>
#include <cl/tape/util/testoutput.hpp>

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

// Calculates dot product of two AD<double> vectors and two tape_double vectors.
// Vectors are not declared as independent.
void performanceTestWithoutTape()
{
    std::ofstream out("..\\..\\..\\..\\..\\..\\..\\output\\en-us\\Tape\\CppAD\\Tests\\DotProductTest\\Log.csv",
        std::ofstream::out | std::ofstream::app);

    out << std::endl << "Test performance for dot product calculation without tape recording" << std::endl;
    out << "Dot product of two vectors (size " << dimension << " ) is being calculated" << std::endl;

    std::vector<CppAD::AD<double>> leftAD = vectorCast<CppAD::AD<double>>(leftVector);
    std::vector<CppAD::AD<double>> rightAD = vectorCast<CppAD::AD<double>>(rightVector);

    std::vector<cl::tape_double> lefttape_double = vectorCast<cl::tape_double>(leftVector);
    std::vector<cl::tape_double> righttape_double = vectorCast<cl::tape_double>(rightVector);

    CppAD::AD<double> ADResult = 0;
    cl::tape_double tape_doubleResult = 0;
    out << "Start of calculating: " << currentTime() << std::endl;
    boost::timer timer;
    ADResult = dotProduct<CppAD::AD<double>>(leftAD, rightAD);
    double ADtime = timer.elapsed();

    timer.restart();
    tape_doubleResult = dotProduct<cl::tape_double >(lefttape_double, righttape_double);
    double CppTime = timer.elapsed();
    out << "End of calculating" << std::endl;
    out << "\tTime for AD<double> " << ADtime << " s" << std::endl;
    out << "\tTime for tape_double " << CppTime << " s" << std::endl;
    out << "\tThe relative difference  " << 1.0 * std::abs(CppTime - ADtime) / std::max(CppTime, ADtime)
        << std::endl ;

}

// Calculates dot product of two AD<double> vectors and two tape_double vectors.
// Vectors are declared as independent.
void performanceTestWithTape()
{

    std::ofstream out("..\\..\\..\\..\\..\\..\\..\\output\\en-us\\Tape\\CppAD\\Tests\\DotProductTest\\Log.csv",
        std::ofstream::out | std::ofstream::app);

    out << std::endl << "Test performance for dot product calculation with tape recording" << std::endl;
    out << "Dot product of two vectors (size " << dimension << " ) is being calculated" << std::endl;
    std::vector<CppAD::AD<double>> leftAD = vectorCast<CppAD::AD<double>>(leftVector);
    std::vector<CppAD::AD<double>> rightAD = vectorCast<CppAD::AD<double>>(rightVector);

    std::vector<cl::tape_double> lefttape_double = vectorCast<cl::tape_double>(leftVector);
    std::vector<cl::tape_double> righttape_double = vectorCast<cl::tape_double>(rightVector);

    CppAD::AD<double> ADResult = 0;
    cl::tape_double tape_doubleResult = 0;

    CppAD::Independent(leftAD);
    out << "Start of calculating: " << currentTime() << std::endl;
    boost::timer timer;
    ADResult = dotProduct<CppAD::AD<double>>(leftAD, rightAD);
    CppAD::ADFun<double> adfun(leftAD, std::vector<CppAD::AD<double>>({ ADResult }));
    double ADtime = timer.elapsed();

    cl::Independent(lefttape_double);

    timer.restart();
    tape_doubleResult = dotProduct<cl::tape_double >(lefttape_double, righttape_double);
    cl::tape_function<double> fcl(lefttape_double, std::vector<cl::tape_double>({ tape_doubleResult }));
    double CppTime = timer.elapsed();
    out << "End of calculating" << std::endl;
    out << "\tTime for AD<double> " << ADtime << " s" << std::endl;
    out << "\tTime for tape_double " << CppTime << " s" << std::endl;
    out << "\tThe relative difference  " << 1.0 * std::abs(CppTime - ADtime) / std::max(CppTime, ADtime)
        << std::endl ;
}

BOOST_AUTO_TEST_SUITE(DotProductTest)

BOOST_AUTO_TEST_CASE(WithoutTapeRecordingDotProductTest)
{
    performanceTestWithoutTape();
}

BOOST_AUTO_TEST_CASE(WithTapeRecordingDotProductTest)
{
    performanceTestWithTape();
}

BOOST_AUTO_TEST_SUITE_END()
