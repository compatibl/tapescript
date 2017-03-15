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

#ifndef cl_tape_impl_qr_decomposition_examples_hpp
#define cl_tape_impl_qr_decomposition_examples_hpp
#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/format.hpp>
namespace cl {
    typedef boost::numeric::ublas::vector<tdouble> tvector;
    typedef boost::numeric::ublas::matrix<tdouble, boost::numeric::ublas::column_major> tmatrix;
    struct qr_decomp_input_data;
    void decomposeMatrix(std::vector<tdouble> &elementsA, tmatrix &Q, tmatrix &R, std::ostream& out_stream);
    void printMatrix(tmatrix const&m, std::ostream& out_stream);
    void qr_decomposition_start(qr_decomp_input_data const& inputData, std::ostream& out_stream);
    void finiteDifferenceCheck(std::vector<tdouble> &indep, tmatrix &Q, tmatrix &R, std::ostream& out_stream);


    // A struct to create and store input data.
    struct qr_decomp_input_data {
        tmatrix data;
        qr_decomp_input_data(std::vector<std::vector<tdouble>> const& rows) {
            int n = rows.size();
            data.resize(n, n);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    data(i, j) = rows[i][j];
                }
            }
        }
        qr_decomp_input_data(tmatrix const& input) {
            int n = input.size1();
            data(n, n);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    data(i, j) = input(i, j);
                }
            }
        }
        tmatrix const& getData() const {
            return data;
        }
    };

    // Defines data sets and calls
    inline void qr_decomposition_examples() {
        std::ofstream of("output/qr_decomposition_examples_output.txt");
        tape_serializer<tvalue> serializer(of);
        serializer.precision(3);

        // Define data set.
        qr_decomposition_start(qr_decomp_input_data(std::vector<std::vector<tdouble>>{
            { 12, -51, 4 },
            { 6, 167, -68 },
            { -4, 24, -41 }
        }
        ), serializer);

        // Define data set.
        std::vector<std::vector<tdouble>> data2;
        data2.push_back({ 1, 3, 4 });
        data2.push_back({ 6, 167, -68 });
        data2.push_back({ -4, 24, -41 });
        qr_decomposition_start(qr_decomp_input_data(data2), serializer);
    }

    // Starts the qr decomposition tests from received input data corresponding to left side matrix A.
    inline void qr_decomposition_start(qr_decomp_input_data const& inputData, std::ostream& out_stream = std::cout) {
        out_stream << std::endl;
        out_stream << " Start of decomposition " << std::endl << std::endl;
        // Get input matrix.
        tmatrix const& A = inputData.getData();
        int n = A.size1();
        tmatrix Q(n, n);
        tmatrix R(n, n);
        // Differentiation.
        std::vector<tdouble> indep(n*n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                indep[i*n + j] = A(i, j);
            }
        }
        // Forward.
        for (int i = 0; i < n * n; i++) {
            cl::tape_start(indep);
            decomposeMatrix(indep, Q, R, out_stream);
            std::vector<tdouble> dependent(2 * n);
            for (int j = 0; j < n; j++) {
                dependent[j] = Q(j, j);
                dependent[n + j] = R(j, j);
            }
            cl::tfunc<double> f(indep, dependent);
            // Calculate derivative in Forward mode.
            std::vector<double> sy, sx(n*n);
            sx[i] = 1;
            // First n values are Q(j,j), last n are R(j,j),
            out_stream << "[FORWARD] Tape operations sequence for differentiation of R,Q(j,j) with respect to A[" << i << "]: " << std::endl;
            sy = f.forward(1, sx, out_stream);
            out_stream << sy << std::endl;
        }
        out_stream << std::endl << std::endl;
        // Reverse.
        for (int i = 0; i < 2 * n; i++) {
            cl::tape_start(indep);
            decomposeMatrix(indep, Q, R, out_stream);
            std::vector<tdouble> dependent(2 * n);
            for (int j = 0; j < n; j++) {
                dependent[j] = Q(j, j);
                dependent[n + j] = R(j, j);
            }
            cl::tfunc<double> f(indep, dependent);

            // Calculate derivative in Reverse mode.
            std::vector<double> sy, sx(2 * n);
            sx[i] = 1;
            std::string message_matrix = (i < n) ? "Q" : "R";
            int message_index = (i < n) ? i : i - n;
            out_stream << "[REVERSE] Differentials corresponding to dependent element " << message_matrix << message_index << " with respect to A " << std::endl;

            sy = f.reverse(1, sx, out_stream);
            out_stream << sy << std::endl;
        }
        out_stream << std::endl << std::endl;
        finiteDifferenceCheck(indep, Q, R, out_stream);
        // General decomposition info.
        out_stream << std::endl;
        out_stream << "TESTING QR DECOMPOSITION ACCURACY  FOR MATRICES :" << std::endl;;
        out_stream << "A = " << std::endl;
        printMatrix(A, out_stream);
        out_stream << "R = " << std::endl;
        printMatrix(R, out_stream);
        out_stream << "Q = " << std::endl;
        printMatrix(Q, out_stream);
        out_stream << "Q * R =  " << std::endl;
        // Checking QR = A.
        tmatrix QR = prod(Q, R);
        printMatrix(QR, out_stream);
        tdouble error = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                error += abs((double)(QR(j, i) - A(j, i)));
            }
        }
        out_stream << "Total multiplication difference error : " << error << std::endl;
        if (n>2) {
            // Checking orthogonality of Q.
            out_stream << "Dot products of Q: ";
            tdouble totalError = 0;
            for (int i = 0; i < n; i++) {
                boost::numeric::ublas::matrix_row<tmatrix > firstRow(Q, i);
                for (int j = i + 1; j < n; j++) {
                    boost::numeric::ublas::matrix_row<tmatrix> secondRow(Q, j);
                    double localError = abs((double)inner_prod(firstRow, secondRow));
                    out_stream << "Dot product of vectors Q[" << i << "] and Q[" << j << "] : " << localError << std::endl;
                    totalError += localError;
                }
            }
            out_stream << "Sum of orthogonality errors of all vectors : " << totalError << std::endl;
            out_stream << "Average orthogonality error per vector : " << totalError / (n * (n + 1) / 2) << std::endl;
        }
        out_stream << "End of decomposition" << std::endl;
        out_stream << std::endl << std::endl << std::endl;
    }

    // Calculates derivatives using finite difference method.
    void finiteDifferenceCheck(std::vector<tdouble> &indep, tmatrix &Q, tmatrix &R, std::ostream& out_stream) {
        // Finite difference.
        int n = sqrt(indep.size());
        out_stream << "Calculating same values using finite difference: " << std::endl;
        tdouble stepX = 0.01;
        std::vector<tdouble> dA(indep);
        tmatrix dQ(n, n), dR(n, n);
        for (int j = 0; j < n*n; j++) {
            out_stream << "Calculating derivatives of diagonal elements of Q, R corresponding to A[" << j << "]  :" << std::endl;
            std::vector<tdouble> dDep(2 * n);
            for (int k = 0; k < n; k++) {
                dDep[k] = dQ(k, k);
                dDep[n + k] = dR(k, k);
            }
            dA[j] += stepX;
            // Retrieve new QR decomposition for the initial matrix A with one incremented entry dA[j].
            decomposeMatrix(dA, dQ, dR, out_stream);
            std::vector<tdouble> sy(2 * n);
            for (int k = 0; k < n; k++) {
                sy[k] = (dQ(k, k) - Q(k, k)) / stepX;
                sy[n + k] = (dR(k, k) - R(k, k)) / stepX;
            }
            out_stream << sy << std::endl;
            dA[j] -= stepX;
        }
    }

    // Decomposition itself, collects matrix A from a vector and fills corresponding matrices Q and R.
    // QR decomposition of matrix A = QR with Q - orthogonal and
    // R - upper triangular using Householder transformations.
    void decomposeMatrix(std::vector<tdouble> &elementsA, tmatrix &Q, tmatrix &R, std::ostream& out_stream = std::cout) {
        int m = sqrt(elementsA.size());
        tmatrix A(m, m);
        // Retrieving the matrix.
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < m; j++) {
                A(i, j) = elementsA[i*m + j];
            }
        }
        // Support vectors.
        std::vector<tvector> ui(m);
        std::vector<tmatrix> Hi(m);
        // Deep copy of A.
        tmatrix yi(A);
        // Main calculations.
        for (int i = 0; i < m - 1; i++) {
            ui[i] = tvector(m);
            // Decreasing valuable size of matrix.
            for (int j = 0; j < i; j++) {
                for (int w = 0; w < m; w++) {
                    yi(j, w) = 0;
                    yi(w, j) = 0;
                }
            }
            for (int j = 0; j < i; j++)
                yi(j, j);
            tvector e(m, 0);
            tvector y = boost::numeric::ublas::matrix_column<tmatrix>(yi, i);
            e[i] = norm_2(y);
            tvector temp = y - e;
            tdouble norm = norm_2(temp);
            // ui = (temp) / (||temp||).
            ui[i] = temp * (1 / norm);

            // Identity matrix.
            boost::numeric::ublas::identity_matrix<tdouble> I(m, m);
            // P = u*u^{T}.
            tmatrix P = outer_prod(ui[i], ui[i]);
            //Hi = I- 2*P.
            tmatrix rightTerm = P * 2;
            Hi[i] = I - rightTerm;
            // A:= H_{i}*A.
            tmatrix tempyi = prod(trans(Hi[i]), yi);
            yi = tempyi;
            m = yi.size1();
        }
        // Retrieving matrices.
        // Q = H_{1} * H_{2} * H_{3}*...H_{m-1}*H_{m}.
        Q = Hi[0];
        for (int i = 1; i < m - 1; i++)
        {
            tmatrix temp = prod(Q, Hi[i]);
            Q = temp;
        }

        // R = H_{m}*H_{m-1}*H_{m-2}*...*H_{1} * A.
        R = Hi[m - 2];
        for (int i = m - 3; i >= 0; i--) {
            tmatrix temp = prod(R, Hi[i]);
            R = temp;
        }
        tmatrix temp = prod(R, A);
        // Correcting matrix to ensure its upper triangular.
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < i; j++) {
                temp(i, j) = 0;
            }
        }
        R = temp;
    }

    // Formatted matrix output.
    void printMatrix(tmatrix const&m, std::ostream & out_stream)
    {
        for (unsigned i = 0; i < m.size1(); ++i)
        {
            out_stream << "| ";
            for (unsigned j = 0; j < m.size2(); ++j)
            {
                out_stream << std::setw(14) << std::setprecision(7) << m(i, j) << " | ";
            }
            out_stream << "|" << std::endl;
        }
    }
}
#endif