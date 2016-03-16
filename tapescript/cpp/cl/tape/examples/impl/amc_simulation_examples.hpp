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

#ifndef cl_tape_examples_impl_amc_simulation_examples_hpp
#define cl_tape_examples_impl_amc_simulation_examples_hpp

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>


namespace cl
{   
    template <class T>
    class amc_simulation
    {
    public:
        
        amc_simulation(T start_price, T strike_price, T volatility, T time, T rate) :
            start_price_(start_price),
            volatility_(volatility),
            time_(time),
            strike_price_(strike_price),
            rate_(rate),
            rng_(),
            generator_(rng_, boost::normal_distribution<double>(0.0, 1.0))
        {            
        }        

        // Generate paths. 
        // In case tarray paths[i] is tarray (vector of pricings portfolio).
        std::vector<T> generate_paths(int number_paths, std::ostream& out_stream = std::cout)
        {
            std::vector<T> paths;
            
            for (int i = 0; i < number_paths; i++)
            {
                double rand = generator_();
                T deltaPrice = time_* rate_ * start_price_ + start_price_ * std::sqrt(time_) * volatility_ * rand;
                paths.push_back(start_price_ + deltaPrice);                
            }
            
            
            return paths;
        }

        T mean_price(int number_paths)
        {
            //std::vector<T> paths = generate_paths(number_paths);
            std::vector<T> prices;
            T sum = null_;
            for (int i = 0; i < number_paths; i++)
            {           
                double rand = generator_();
                T price = start_price_ * (1 + time_* rate_ + std::sqrt(time_) * volatility_ * rand);
                sum = sum + std::max(null_, price - strike_price_);
            }
            return sum / double(number_paths);
        }

        T price(int number_paths)
        {
            return mean_price(number_paths) * std::exp(-rate_*time_);
        }

        void setNull(T& null)
        {
            null_ = null;
        }

    private:
        T start_price_, volatility_, time_, strike_price_, null_, rate_;       
        boost::random::mt19937 rng_;
        boost::variate_generator<boost::mt19937, boost::normal_distribution<double> > generator_;
    };

    inline void amc_tdouble_simulation(int size_portfolio, std::ostream& out_stream = std::cout, bool serialize = false)
    {
        boost::timer timer;
        
        for (int i = 0; i < size_portfolio; i++)
        {
            tdouble start_price = 100.0 + i * 0.1;
            tdouble volatility = 0.1 + i * 0.001;
            tdouble strike_price = 120.0 + i * 0.1;
            tdouble time = 2.0;
            tdouble rate = 0.05;
            std::vector<tdouble> X = { start_price, strike_price, volatility, time, rate};
            cl::tape_start(X);

            amc_simulation<tdouble> simulation(X[0], X[1], X[2], X[3], X[4]);
            simulation.setNull(tdouble(0.0));

            tdouble option_price = simulation.price(1000);
            
            if (serialize)
                out_stream << "price " << option_price << std::endl;

            std::vector<tdouble> Y = { option_price };
            // Declare a tape function and stop the tape recording.
            cl::tfunc<double> f(X, Y);

            // Forward sweep calculations.
            std::vector<double> dx = { 1, 0, 0, 0, 0 };
            if (serialize)
                out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
            std::vector<double> forw = f.forward(1, dx);
            if (serialize)
                out_str << "Forward sweep result: Delta equal " << forw << "\n\n";
        }        
        double time_res = timer.elapsed();
        
        // out_stream << "tdouble performance result: " << time_res << " for size portfolio = " << size_portfolio << std::endl;
    }

    inline void amc_tarray_simulation(int size_portfolio, std::ostream& out_stream = std::cout, bool serialize = false)
    {
        boost::timer timer;        

        std::valarray<double> start_price_vector(size_portfolio);
        std::valarray<double> volatility_vector(size_portfolio);
        std::valarray<double> strike_price_vector(size_portfolio);
        std::valarray<double> time_vector(size_portfolio);
        std::valarray<double> rate_vector(size_portfolio);
        std::valarray<double> null_vector(size_portfolio);
        std::valarray<double> unity_vector(size_portfolio);

        for (int i = 0; i < size_portfolio; i++)
        {
            start_price_vector[i] = 100.0 + i * 0.1;
            volatility_vector[i] = 0.1 + i * 0.001;
            strike_price_vector[i] = 120.0 + i * 0.1;
            time_vector[i] = 2.0;
            rate_vector[i] = 0.05;
            null_vector[i] = 0.0;
            unity_vector[i] = 1.0;
        }
        
        cl::tobject start_price = start_price_vector;
        cl::tobject volatility = volatility_vector;
        cl::tobject strike_price = strike_price_vector;
        cl::tobject time = time_vector;
        cl::tobject rate = rate_vector;
        cl::tobject null = null_vector;
        cl::tobject unity = unity_vector;


        std::vector<cl::tobject> X = { start_price, strike_price, volatility, time, rate };

        cl::tape_start(X);        
        
        amc_simulation<cl::tobject> simulation(X[0], X[1], X[2], X[3], X[4]);
        simulation.setNull(null);
        cl::tobject option_price = simulation.price(1000);
        if (serialize)
            out_stream << option_price << std::endl;

        std::vector<cl::tobject> Y = { option_price };
        // Declare a tape function and stop the tape recording.
        cl::tfunc<cl::tvalue> f(X, Y);

        // Forward sweep calculations.
        std::vector<cl::tvalue> dx = { unity_vector, null_vector, null_vector, null_vector, null_vector };
        if (serialize)
            out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<cl::tvalue> forw = f.forward(1, dx);
        if (serialize)
            out_str << "Forward sweep result: Delta equal " << forw << "\n\n";

        double time_res = timer.elapsed();
        // out_stream << "tarray performance result: " << time_res << " for size portfolio = " << size_portfolio << std::endl;
    }
    
    inline void amc_simulation_examples()
    {
        std::ofstream of("output/amc_simulation_output.txt");
        tape_serializer<tvalue> serializer(of);
        
        serializer.precision(3);
        
        // Start test for 5 options in portfolio.
        int portfolio_size = 5;        

        amc_tdouble_simulation(portfolio_size, of, true);
        amc_tarray_simulation(portfolio_size, of, true);

        // Start test for 1000 options in portfolio.
        portfolio_size = 1000;      

        amc_tdouble_simulation(portfolio_size, of);        
        amc_tarray_simulation(portfolio_size, of);
    }
}

#endif // cl_tape_examples_impl_amc_simulation_examples_hpp
