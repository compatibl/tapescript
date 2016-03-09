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

#ifndef cl_tape_examples_impl_polynomial_regression_data_hpp
#define cl_tape_examples_impl_polynomial_regression_data_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>

namespace cl
{

    template <class T>
    struct caller
    {
    };

    template<>
    struct caller<tobject>
    {
        static tobject CreateEquidistantXVector(size_t n)
        {
            double step = 10.0 / n;
            tarray vec_ref(n);
            for (int i = 0; i < n; i++)
                vec_ref[i] = (i + 1) * step - 2.0;
            tobject vec = (tvalue)vec_ref;
            return vec;
        }

        static tobject FunctionPolynomPlusExp(const tobject& x, const std::vector<tobject>& par, int deriv_par)
        {
            int order = par.size() - 1;
            tobject y(0);
            if (deriv_par == -1)
            {
                for (int i = 0; i < order; i++)
                    y += par[i] * std::pow(x, i);
                y += exp(-1 * par[order] * x);
            }
            else if (deriv_par >= 0 && deriv_par < par.size() - 1)
                y += std::pow(x, deriv_par);
            else if (deriv_par == par.size() - 1)
                y += -1 * x * exp(-1 * par[deriv_par] * x);
            else
                throw std::runtime_error("Wrong parameter number for analytical derivative calculation.");

            return y;
        }
    
    };    
    
    template<>
    struct caller<std::vector<tdouble> >
    {
        static std::vector<tdouble> CreateEquidistantXVector(size_t n)
        {
            double step = 10.0 / n;
            std::vector<tdouble> vec(n);
            for (int i = 0; i < n; i++)
                vec[i] = (i + 1) * step - 2.0;
            return vec;
        }

        static std::vector<tdouble> FunctionPolynomPlusExp(const std::vector<tdouble>& x, const std::vector<std::vector<tdouble>>& par, int deriv_par)
        {
            // Number of data points.
            int npoints = x.size();
            int order = par.size() - 1;
            std::vector<tdouble> y(npoints);
            if (deriv_par == -1)
            {
                for (int j = 0; j < npoints; j++)
                {
                    for (int i = 0; i < order; i++)
                        y[j] += par[i][0] * std::pow(x[j], i);
                    y[j] += std::exp(-1 * par[order][0] * x[j]);
                }
            }
            else if (deriv_par >= 0 && deriv_par < par.size() - 1)
                for (int j = 0; j < npoints; j++)
                    y[j] = std::pow(x[j], deriv_par);
            else if (deriv_par == par.size() - 1)
                for (int j = 0; j < npoints; j++)
                    y[j] = -1 * x[j] * std::exp(-1 * par[deriv_par][0] * x[j]);
            else
                throw std::runtime_error("Wrong parameter number for analytical derivative calculation.");

            return y;
        }
    };
    
    // Class to store steering parameters for polynomial regression examples.
    // Also provides service methods for generation of input data.
    class polynomial_regression_data
    {
    private:
        // Order of polynomial regression.
        const int order_;

        // Number of input points {x_i, y_i}.
        size_t n_;

        // Parameters for generation of y_i:
        // y_i = par_polynom_[0] + par_polynom_[1] * x_i + ... + par_polynom_[order_ - 1] * x_i^(order_ - 1) + exp(-1 * par_exponent_ * x_i).
        std::vector<double> par_polynom_;
        double par_exponent_;

        // Tolerance for analytical vs. adjoint derivative check.
        double tol_;

        // Control production of tape output to serializer.
        bool flag_serializer_;

        // Flasg for forward and reverse derivative calculation.
        bool flag_forward_;
        bool flag_reverse_;

        // Flag for all derivatives (including estimations) calculation in reverse mode (might be slow for large number of points).
        bool flag_reverse_all_;

        // Check derivatives analytically.
        bool flag_check_;

    public:
        polynomial_regression_data(int order) : order_(order)
        {
            // Default settings for debug/release mode.
#if defined NDEBUG
            n_ = 1000000;
            flag_serializer_ = false;
            flag_check_ = false;
            flag_reverse_all_ = false;
#else
            n_ = 10;
            flag_serializer_ = true;
            flag_check_ = true;
            flag_reverse_all_ = true;
#endif

            // Default settings for parameters.
            par_polynom_.resize(order + 1);
            // Polynomial coefficients are set to 1.0, 0.5, 0.25, 0.125 etc.
            par_polynom_[0] = 1;
            for (int i = 1; i < par_polynom_.size(); i++)
                par_polynom_[i] = par_polynom_[i - 1] / 2;
            par_exponent_ = 0.5;

            // Flags for forward and reverse derivative calculation.
            flag_forward_ = true;
            flag_reverse_ = true;

            // Default settings for tolerance, serializer and check flags.
            tol_ = 1e-6;
        }

        // Get and set number of input points.
        int GetPointsNumber() const { return n_; }
        void SetPointsNumber(int n) { n_ = n; }

        // Get order of polynomial regerssion.
        int GetOrder() const { return order_; }

        // Get and set polynomial parameters.
        std::vector<double> GetPolynomParameters() const { return par_polynom_; }
        void SetPolynomParameter(int par_number, double par_value) { par_polynom_[par_number] = par_value; }

        // Get and set exponential parameter.
        double GetExponentParameter() const { return par_exponent_; }
        void SetExponentialParameter(double value) { par_exponent_ = value; }

        // Get and set tolerance.
        double GetTolerance() const { return tol_; }
        void SetTolerance(double tol) { tol_ = tol; }

        // Get and set serializer flag.
        bool GetFlagSerializer() const { return flag_serializer_; }
        void SetFlagSerializer(bool flag) { flag_serializer_ = flag; }

        // Get and set check flag.
        bool GetFlagCheck() const { return flag_check_; }
        void SetFlagCheck(bool flag) { flag_check_ = flag; }

        // Get and set forward derivatives flag.
        bool GetFlagForward() const { return flag_forward_; }
        void SetFlagForward(bool flag) { flag_forward_ = flag; }

        // Get and set reversed derivatives flag.
        bool GetFlagReverse() const { return flag_reverse_; }
        void SetFlagReverse(bool flag) { flag_reverse_ = flag; }

        // Get and set reversed derivatives flag.
        bool GetFlagReverseAll() const { return flag_reverse_all_; }
        void SetFlagReverseAll(bool flag) { flag_reverse_all_ = flag; }

        // Create equidistant vector { -1, ..., 9} as tobject.
        template<typename element_type>
        element_type CreateEquidistantXVector() const
        {
            return caller<element_type>::CreateEquidistantXVector(n_);
        }

        // Static method to create y vector using provided x vector, pointer to function y = func(x, pars) and its parameters pars.
        template<typename element_type>
        static element_type CreateYVector(element_type x, element_type(*func)(const element_type& x, const std::vector<element_type>& pars), const std::vector<element_type>& pars)
        {
            element_type y = func(x, pars);
            return y;
        }

        // Static method to create y vector using provided x vector:
        // y = par[0] + par[1] * x + ... + par[order - 1] * x^(order - 1) + exp(-1 * par[order] * x),
        // where order = par.size() - 1.
        template<typename element_type>
        static element_type FunctionPolynomPlusExp(const element_type& x, const std::vector<element_type>& par, int deriv_par = -1)
        {
            return caller<element_type>::FunctionPolynomPlusExp(x, par, deriv_par);
        }

    };
}

#endif // cl_tape_examples_impl_polynomial_regression_data_hpp
