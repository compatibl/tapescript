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

#ifndef CL_SERIALIZATION_EXAMPLE_HPP
#define CL_SERIALIZATION_EXAMPLE_HPP


#pragma warning(disable:4503)

#include <iostream>
#include <fstream>

#include <cl/tape/tape.hpp>
#include <cl/tape/impl/tape_archive/tape_arhive.hpp>

namespace cl
{
    inline void serialization_example()
    {
        typedef double Base;
        typedef cl::tdouble Real;

        //  std::pow(std::cos(U[0]),2) + std::cos(std::pow(U[1], 2));

        std::string path = "c:\\ooo.bin";

        std::vector<Base> r0, r1;

        // Serialization
        {
            // independent variable vector
            std::vector<Real> U(2);

            U[0] = 8.;
            U[1] = 2.;
            cl::Independent(U);

            // dependent variable vector 
            std::vector<Real> Z(1);

            // the model
            // std::pow(std::cos(U[0]), 2);
            // std::pow(U[0], 2);
            Real a = std::pow(std::cos(U[0]), 2) + std::cos(std::pow(U[1], 2));

            Z[0] = a / 2;
            {
                // the model tape
                typedef cl::tape_archive<Base, boost::archive::binary_oarchive> tapewriter;

                tapewriter ss(path);

                cl::tfunc<Base> fun(U, Z, ss);

                {
                    std::vector<Base> dX = { 1, 0 };
                    r0 = fun.Forward(1, dX);
                }
            }
        }

        // Deserialization
        {
            typedef cl::tape_archive<Base, boost::archive::binary_iarchive> tapereader;
            typedef cl::tfunc<Base> tapefun;

            tapereader reader(path);

            std::vector<Base> dX = { 1, 0 };
            tapefun fun(reader);
            r1 = fun.Forward(1, dX);
        }

        assert(r0 == r1);
    }
}
#endif
