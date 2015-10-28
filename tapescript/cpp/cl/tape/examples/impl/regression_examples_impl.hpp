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

#ifndef cl_tape_examples_impl_regression_examples_impl_hpp
#define cl_tape_examples_impl_regression_examples_impl_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include "linear_regression.hpp"

namespace cl
{
    // Check derivatives with respect to a.
    void check_derivatives_dA(cl::LinearRegression& lin_regr, std::vector<cl::tvalue>& forw, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dA() - forw[0].scalar_value_) > eps)
            throw std::exception("Adjoint alpha derivative w.r.t a differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dA() - forw[1].scalar_value_) > eps)
            throw std::exception("Adjoint beta derivative w.r.t a differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dA(j) - forw[2].element_at(j)) > eps)
            throw std::exception("Adjoint y_estimate derivative w.r.t a differs from expectation.");
    }

    // Check derivatives with respect to a.
    void check_derivatives_dA(cl::LinearRegression& lin_regr, std::vector<double>& forw, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dA() - forw[0]) > eps)
            throw std::exception("Adjoint alpha derivative w.r.t a differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dA() - forw[1]) > eps)
            throw std::exception("Adjoint beta derivative w.r.t a differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dA(j) - forw[2 + j]) > eps)
            throw std::exception("Adjoint y_estimate derivative w.r.t a differs from expectation.");
    }

    // Check derivatives with respect to b.
    void check_derivatives_dB(cl::LinearRegression& lin_regr, std::vector<cl::tvalue>& forw, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dB() - forw[0].scalar_value_) > eps)
            throw new std::exception("Adjoint alpha derivative w.r.t b differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dB() - forw[1].scalar_value_) > eps)
            throw new std::exception("Adjoint beta derivative w.r.t b differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dB(j) - forw[2].element_at(j)) > eps)
            throw new std::exception("Adjoint y_estimate derivative w.r.t b differs from expectation.");
    }

    // Check derivatives with respect to b.
    void check_derivatives_dB(cl::LinearRegression& lin_regr, std::vector<double>& forw, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dB() - forw[0]) > eps)
            throw std::exception("Adjoint alpha derivative w.r.t a differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dB() - forw[1]) > eps)
            throw std::exception("Adjoint beta derivative w.r.t a differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dB(j) - forw[2 + j]) > eps)
            throw std::exception("Adjoint y_estimate derivative w.r.t a differs from expectation.");
    }

    // Check derivatives with respect to c.
    void check_derivatives_dC(cl::LinearRegression& lin_regr, std::vector<cl::tvalue>& forw, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dC() - forw[0].scalar_value_) > eps)
            throw new std::exception("Adjoint alpha derivative w.r.t c differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dC() - forw[1].scalar_value_) > eps)
            throw new std::exception("Adjoint beta derivative w.r.t c differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dC(j) - forw[2].element_at(j)) > eps)
            throw new std::exception("Adjoint y_estimate derivative w.r.t c differs from expectation.");
    }

    // Check derivatives with respect to c.
    void check_derivatives_dC(cl::LinearRegression& lin_regr, std::vector<double>& forw, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dC() - forw[0]) > eps)
            throw std::exception("Adjoint alpha derivative w.r.t a differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dC() - forw[1]) > eps)
            throw std::exception("Adjoint beta derivative w.r.t a differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dC(j) - forw[2 + j]) > eps)
            throw std::exception("Adjoint y_estimate derivative w.r.t a differs from expectation.");
    }

    // Check derivatives with respect to x_i.
    void check_derivatives_dX(cl::LinearRegression& lin_regr, std::vector<cl::tvalue>& forw, int i, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dX(i) - forw[0].scalar_value_) > eps)
            throw new std::exception("Adjoint alpha derivative w.r.t x_i differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dX(i) - forw[1].scalar_value_) > eps)
            throw new std::exception("Adjoint beta derivative w.r.t x_i differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dX(j, i) - forw[2].element_at(j)) > eps)
            throw new std::exception("Adjoint y_estimate derivative w.r.t x_i differs from expectation.");
    }

    // Check derivatives with respect to x_i.
    void check_derivatives_dX(cl::LinearRegression& lin_regr, std::vector<double>& forw, int i, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dX(i) - forw[0]) > eps)
            throw new std::exception("Adjoint alpha derivative w.r.t x_i differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dX(i) - forw[1]) > eps)
            throw new std::exception("Adjoint beta derivative w.r.t x_i differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dX(j, i) - forw[2 + j]) > eps)
            throw new std::exception("Adjoint y_estimate derivative w.r.t x_i differs from expectation.");
    }

    // Check derivatives with respect to y_i.
    void check_derivatives_dY(cl::LinearRegression& lin_regr, std::vector<cl::tvalue>& forw, int i, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dY(i) - forw[0].scalar_value_) > eps)
            throw new std::exception("Adjoint alpha derivative w.r.t x_i differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dY(i) - forw[1].scalar_value_) > eps)
            throw new std::exception("Adjoint beta derivative w.r.t x_i differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dY(j, i) - forw[2].element_at(j)) > eps)
            throw new std::exception("Adjoint y_estimate derivative w.r.t x_i differs from expectation.");
    }

    // Check derivatives with respect to y_i.
    void check_derivatives_dY(cl::LinearRegression& lin_regr, std::vector<double>& forw, int i, double eps)
    {
        int n = lin_regr.GetN();

        // Checking alpha derivatives.
        if (std::fabs(lin_regr.GetDerivate_dAlpha_dY(i) - forw[0]) > eps)
            throw new std::exception("Adjoint alpha derivative w.r.t x_i differs from expectation.");

        // Checking beta derivatives.
        if (std::fabs(lin_regr.GetDerivate_dBeta_dY(i) - forw[1]) > eps)
            throw new std::exception("Adjoint beta derivative w.r.t x_i differs from expectation.");

        // Checking y_estimate derivatives.
        for (int j = 0; j < n; j++)
        if (std::fabs(lin_regr.GetDerivate_dYEstim_dY(j, i) - forw[2 + j]) > eps)
            throw new std::exception("Adjoint y_estimate derivative w.r.t x_i differs from expectation.");
    }
}

#endif // cl_tape_examples_impl_regression_examples_impl_hpp
