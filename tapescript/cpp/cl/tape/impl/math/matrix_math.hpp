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

#ifndef cl_tape_impl_matrix_math_hpp
#define cl_tape_impl_matrix_math_hpp

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace cl
{
    // Matrix operations.
    namespace tapescript
    {
        // Invert symmetric matrix using Cholesky decomposition from the Boost library.
        // This template method works with both element_type = tobject and element_type = tdouble.
        template<typename element_type>
        static std::vector<std::vector<element_type>> invert_sym_matrix_boost(const std::vector<std::vector<element_type>>& mat)
        {
            // Matrix size.
            int m = mat.size();
            boost::numeric::ublas::matrix<element_type> input(m, m);
            for (int i = 0; i < m; i++)
            for (int j = 0; j < m; j++)
                input.operator ()(i, j) = mat[i][j];
            // Create a permutation matrix for the LU-factorization
            boost::numeric::ublas::permutation_matrix<std::size_t> pm(m);
            // Perform LU-factorization
            if (boost::numeric::ublas::lu_factorize(input, pm))
                throw std::runtime_error("Singular matrix");
            // Create identity matrix of for inverse
            boost::numeric::ublas::matrix<element_type> inverse(m, m);
            inverse.assign(boost::numeric::ublas::identity_matrix<element_type>(m));
            boost::numeric::ublas::lu_substitute(input, pm, inverse);
            std::vector<std::vector<element_type>> mat_inv = mat;
            for (int i = 0; i < m; i++)
            for (int j = 0; j < m; j++)
                mat_inv[i][j] = inverse(i, j);
            return mat_inv;
        }

        // Find transpose matrix.
        template<typename element_type>
        static std::vector<std::vector<element_type>> transpose_matrix(const std::vector<std::vector<element_type>>& mat)
        {
            // Matrix size.
            int m = mat.size();
            if (m == 0)
                throw std::runtime_error("Matrix has 0 rows");
            int n = mat[0].size();

            // Matrix trasnposition.
            std::vector<std::vector<element_type>> transpose(n);
            for (int row = 0; row < n; row++)
            {
                transpose[row].resize(m);
                for (int col = 0; col < m; col++)
                    transpose[row][col] = mat[col][row];
            }
            return transpose;
        }

        // Multiply matrices.
        template<typename element_type>
        static std::vector<std::vector<element_type>> mult_matrix(const std::vector<std::vector<element_type>>& mat1, const std::vector<std::vector<element_type>>& mat2)
        {
            // Number of rows in ouput matrix.
            int m = mat1.size();
            if (m == 0)
                throw std::runtime_error("Matrix has 0 rows");

            // Check inner matrix dimension.
            int n = mat1[0].size();
            if (n != mat2.size())
                throw std::runtime_error("Inner matrix dimensions must agree");

            // Number of columns in ouput matrix.
            if (n == 0)
                throw std::runtime_error("Matrix has 0 rows");
            int k = mat2[0].size();

            // Matrix multiplication.
            std::vector<std::vector<element_type>> mult(m);
            for (int row = 0; row < m; row++)
            {
                mult[row].resize(k);
                for (int col = 0; col < k; col++)
                {
                    mult[row][col] = 0;
                    for (int i = 0; i < n; i++)
                        mult[row][col] += mat1[row][i] * mat2[i][col];
                }
            }
            return mult;
        }

        // Find pseudoinverse matrix.
        template<typename element_type>
        static std::vector<std::vector<element_type>> pseudoinverse_matrix(const std::vector<std::vector<element_type>>& mat)
        {
            // Matrix size.
            int m = mat.size();
            if (m == 0)
                throw std::runtime_error("Matrix has 0 rows");
            int n = mat[0].size();

            // Find transpose matrix.
            std::vector<std::vector<element_type>> transpose_mat = transpose_matrix(mat);

            // Find largest dimension.
            if (m <= n)
            {
                std::vector<std::vector<element_type>> mult = mult_matrix(mat, transpose_mat);
                std::vector<std::vector<element_type>> inv = invert_sym_matrix_boost(mult);
                return mult_matrix(transpose_mat, inv);
            }
            else
            {
                std::vector<std::vector<element_type>> mult = mult_matrix(transpose_mat, mat);
                std::vector<std::vector<element_type>> inv = invert_sym_matrix_boost(mult);
                return mult_matrix(inv, transpose_mat);
            }
        }
    }
}

#endif // cl_tape_impl_matrix_math_hpp