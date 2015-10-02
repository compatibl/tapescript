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

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>

//#include <ql/quantlib.hpp>
#include <boost/timer.hpp>

#include "array_examples.hpp"


template <class Array>
void print_sizeofs()
{
    std::cout << std::setw(40) << typeid(Array).name() << ": \t";
    std::cout << sizeof(Array) << "\t";
    std::cout << sizeof(cl::inner_array<Array>) << "\t";
    std::cout << sizeof(CppAD::AD<cl::inner_array<Array>>) << "\t";
    std::cout << sizeof(cl::tape_double<cl::inner_array<Array>>) << std::endl;
}

void print_sizeofs_all()
{
    std::cout << "Sizeofs:" << std::endl;

    /*
    print_sizeofs<Eigen::ArrayXd>();
    print_sizeofs<Eigen::Array2d>();
    print_sizeofs<Eigen::Array4d>();
    print_sizeofs<std::valarray<double>>();
    */
}

int main()
{
    examples();
    //print_sizeofs_all();

    system("pause");
    return 0;
}
