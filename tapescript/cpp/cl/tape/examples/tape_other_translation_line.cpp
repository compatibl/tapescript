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
#include <algorithm>

# define CL_BASE_SERIALIZER_OPEN

# define CL_USE_TAPE_SERIALIZATION


//# include <cl/tape/impl/reflection/mem_access.hpp>
# include <cl/tape/tape.hpp>

# include <ctime>

template <class T>
inline void multiply(cl::tape_wrapper<T> *a, cl::tape_wrapper<T> *b, cl::tape_wrapper<T>* s, int N)
{
     for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            for (int j = 0; j < N; ++j) {
                s[i * N + j] += a[i * N + k] *b[k * N + j];
            }
        }
    }
}

template <class T>
inline void multiply(T *a, T *b, T* s, int N)
{
    for (int i = 0; i < N; ++i) {
        for (int k = 0; k < N; ++k) {
            for (int j = 0; j < N; ++j) {
                s[i * N + j] += a[i * N + k]* b[k * N + j];
            }
        }
    }
}

template <class T>
inline void trigonometry(cl::tape_wrapper<T> *a, cl::tape_wrapper<T> *b, cl::tape_wrapper<T>* s, int N)
{
    for (int i = 0; i < N; ++i)
    {
        for (int k = 0; k < N; ++k)
        {
            for (int j = 0; j < N; ++j)
            {
                s[i * N + j] += std::sin(a[i * N + k]) + std::cos(b[k * N + j]);
            }
        }
    }
}

template <class T>
inline void trigonometry(T *a, T *b, T* s, int N)
{
    for (int i = 0; i < N; ++i)
    {
        for (int k = 0; k < N; ++k)
        {
            for (int j = 0; j < N; ++j)
            {
                s[i * N + j] += sin(a[i * N + k]) + cos(b[k * N + j]);
            }
        }
    }
}

namespace std
{
    extern double diffclock(clock_t clock1, clock_t clock2)
    {
        double diffticks = clock1 - clock2;
        double diffms = (diffticks) / (CLOCKS_PER_SEC / 1000);
        return diffms;
    }
}

template <class T>
inline std::pair<double, cl::tdouble> calc(int N)
{
    using namespace cl;

    T *a1 = new T[3 * N * N], *b1 = 0, *s1 = 0;
    
    b1 = a1 + N * N;
    s1 = b1 + N * N;

    clock_t _0 = std::clock();
    multiply(a1, b1, s1, N);
    clock_t _1 = std::clock();
    cl::tdouble r = a1[0];
    delete[] a1;
    return std::make_pair(std::diffclock(_1, _0), r);
}

template <class T>
inline std::pair<double, cl::tdouble> calc_trigonometry(int N)
{
    using namespace cl;

    T *a1 = new T[3 * N * N], *b1 = 0, *s1 = 0;
    b1 = a1 + N * N;
    s1 = b1 + N * N;

    clock_t _0 = std::clock();
    trigonometry(a1, b1, s1, N);
    clock_t _1 = std::clock();
    cl::tdouble r = a1[0];
    delete[] a1;
    return std::make_pair(std::diffclock(_1, _0), r);
}

extern void performance_without_struct()
{
    int S = 200;
#   if !defined NDEBUG
    S = 100;
#   endif
    std::cout << "  Multiplication" << std::endl;
    std::cout << "  double:" << calc<double>(S).first << std::flush;
    std::cout << " cl::tdouble:" << calc<cl::tdouble>(S).first << std::flush;
    std::cout << " cl::qdouble " << calc<CppAD::AD<double>>(S).first << std::endl;

    std::cout << "  Trigonometry" << std::endl;
    std::cout << "  double:" << calc_trigonometry<double>(S).first << std::flush;
    std::cout << " cl::tdouble:" << calc_trigonometry<cl::tdouble>(S).first << std::flush;
    std::cout << " cl::qdouble " << calc_trigonometry<CppAD::AD<double>>(S).first << std::endl;
}