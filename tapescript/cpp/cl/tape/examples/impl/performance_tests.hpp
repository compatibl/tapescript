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

#ifndef cl_tape_examples_impl_performance_tests_hpp
#define cl_tape_examples_impl_performance_tests_hpp
#pragma once

#include <assert.h>
#include <random>
#include <boost/timer.hpp>

#define CL_BASE_SERIALIZER_OPEN
#include <cl/tape/tape.hpp>
#include <cl/tape/util/testoutput.hpp>

#include "impl/utils.hpp"


namespace cl
{
    template <class Func>
    inline double test_performance(size_t repeat_count, Func func)
    {
        assert(repeat_count != 0);
        boost::timer timer;
        for (size_t i = 0; i < repeat_count; i++)
        {
            func();
        }
        return timer.elapsed() / repeat_count;
    }

    struct performance_time
    {
        // Input size (used for plot imformation only).
        size_t size_;
        // Tape recording time.
        double rec_;
        // Ininial Forward(0) sweep time.
        double init_;
        // Forward sweep calculations time.
        double forw_;
        // Reverse sweep calculations time.
        double rev_;
        // TapeFunction memory.
        size_t mem_;
    };

    inline std::ostream& operator<<(std::ostream& os, performance_time perf_time)
    {
        os << "Tape recording time:             " << perf_time.rec_  << std::endl;
        os << "Ininial Forward(0) sweep time:   " << perf_time.init_ << std::endl;
        os << "Forward sweep calculations time: " << perf_time.forw_ << std::endl;
        os << "Reverse sweep calculations time: " << perf_time.rev_  << std::endl;
        os << "Total time for derivatives calculation: "
            << perf_time.rec_ + perf_time.init_ + perf_time.forw_ + perf_time.rev_ << std::endl;
        os << "TapeFunction memory (B): " << perf_time.mem_  << std::endl;
        return os;
    }

    struct column_keeper
    {
        static std::deque<std::string> get_columns()
        {
            static std::deque<std::string> const& columns =
            {
                "Input size"
                , "Tape recording time"
                , "Initial Forward(0) sweep time"
                , "Forward mode derivatives calculation time"
                , "Reverse mode derivatives calculation time"
            };

            return columns;
        }        
    };
    
    template <class Columns = column_keeper>
    struct plot_struct
    {
        plot_struct()
            : y_()
            , x_()
        {}

        plot_struct(performance_time const& perf_time)
            : y_{ perf_time.rec_, perf_time.init_, perf_time.forw_, perf_time.rev_ }
            , x_(perf_time.size_)
        {}

        static std::deque<std::string> get_columns()
        {
            return Columns::get_columns();
        }

        template <typename stream_type>
        friend inline stream_type& operator<<(stream_type& stm, plot_struct& v)
        {
            stm << v.x_;
            for (auto y : v.y_)
            {
                stm << ";" << y;
            }
            stm << std::endl;
            return stm;
        }

        std::deque<double> y_;
        size_t x_;
    };

    inline std::ostream& fake_stream()
    {
        static std::ostream stream(nullptr);
        return stream;
    }

    template <class Inner>
    struct adjoint_task
    {
        typedef Inner inner_type;
        typedef cl::tape_double<Inner> tape_type;

        // Task title.
        std::string title_;
        // Input size (used for plot imformation only).
        size_t size_;
        // Input values.
        std::vector<inner_type> X_;
        // Forward mode directions.
        std::vector<inner_type> dx_;
        // Revers mode weights.
        std::vector<inner_type> w_;
        // Numbers of times to repeat task for performance measurement.
        size_t repeat_;
        // Target function.
        std::function<std::vector<tape_type>(std::vector<tape_type> const&)> func_;
    };

    template <class AdjointTask>
    inline performance_time adjoint_performance(AdjointTask const& task, std::ostream& out_str = fake_stream())
    {
        typedef typename AdjointTask::inner_type inner_type;
        typedef typename AdjointTask::tape_type tape_type;
        performance_time result;
        result.size_ = task.size_;

        out_str << task.title_ << ":\n\n";

        // Input values initialization.
        std::vector<inner_type> x_val = task.X_;
        std::vector<tape_type> X(x_val.begin(), x_val.end());
        out_str << "Input vector: " << X << "\n";

        // Output vector.
        std::vector<tape_type> Y;
        // Tape function.
        cl::TapeFunction<inner_type> f;

        result.rec_ = test_performance(task.repeat_, [&X, &Y, &f, &task]()
        {
            // Declare the X vector as independent and start a tape recording.
            cl::Independent(X);
            // Output calculations.
            Y = task.func_(X);
            //Stop tape recording.
            f.Dependent(X, Y);
        });

        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        result.init_ = test_performance(task.repeat_, [&f, &x_val]()
        {
            f.Forward(0, x_val);
        });

        // Forward sweep calculations.
        std::vector<inner_type> dx = task.dx_;
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<inner_type> forw = f.Forward(1, dx, out_str);
        out_str << "Forward sweep result: " << forw << "\n\n";

        result.forw_ = test_performance(task.repeat_, [&f, &dx]()
        {
            f.Forward(1, dx);
        });

        // Reverse sweep calculations.
        std::vector<inner_type> w = task.w_;
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<inner_type> rev = f.Reverse(1, w);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";

        result.rev_ = test_performance(task.repeat_, [&f, &w]()
        {
            f.Reverse(1, w);
        });

        result.mem_ = f.Memory();

        return result;
    }

    /*
    struct array_scalar_compare
    {
        void set_title(std::string const& title)
        {
            array_task_.title_ = title;
            double_task_.title_ = title;
        }

        void set_size(size_t size)
        {
            array_task_.size_ = size;
            double_task_.size_ = size;
        }

        void set_input(std::vector<InnerArray> const& x)
        {
            array_task_.X_ = x;

            double_task_.X_ = arrays_to_doubles(x);
        }

        adjoint_task<InnerArray> array_task_;
        adjoint_task<double> double_task_;

    private:
        static std::vector<double> arrays_to_doubles(std::vector<InnerArray> const& v)
        {
            std::vector<double> result;
            for (const InnerArray& a : v)
            {
                if (a.is_scalar())
                {
                    result.push_back(a.scalar_value_);
                }
                else
                {
                    for (double el : a.array_value_)
                    {
                        result.push_back(el);
                    }
                }
            }
            return result;
        }
    };
    */

    template <class Vector>
    inline Vector gen_vector(size_t n, std::mt19937& gen)
    {
        std::uniform_real_distribution<> dis(-10, 10);
        Vector vec;
        vec.resize(n);
        for (auto& x : vec)
        {
            x = dis(gen);
        }
        return vec;
    }

    struct plus_task_factory
    {
#if defined NDEBUG
        enum { repeat = 5000000 };
#else
        enum { repeat = 50000 };
#endif
        inline adjoint_task<InnerArray> get_array_task(size_t n)
        {
            std::mt19937 gen;
            gen.seed(0);
            adjoint_task<InnerArray> array_task;
            array_task.title_ = "Plus";
            array_task.size_ = n;
            array_task.X_ = { gen_vector<InnerArray>(n, gen), gen_vector<InnerArray>(n, gen) };
            array_task.dx_ = { gen_vector<InnerArray>(n, gen), gen_vector<InnerArray>(n, gen) };
            array_task.w_ = { gen_vector<InnerArray>(n, gen) };
            array_task.repeat_ = repeat / n;
            array_task.func_ = [](std::vector<TapeArray> const& v)
            {
                return std::vector<TapeArray>{ v[0] + v[1] };
            };
            return array_task;
        }

        inline adjoint_task<double> get_double_task(size_t n)
        {
            std::mt19937 gen;
            gen.seed(0);
            adjoint_task<double> double_task;
            double_task.title_ = "Plus";
            double_task.size_ = n;
            double_task.X_ = gen_vector<std::vector<double>>(2 * n, gen);
            double_task.dx_ = gen_vector<std::vector<double>>(2 * n, gen);
            double_task.w_ = gen_vector<std::vector<double>>(n, gen);
            double_task.repeat_ = repeat / n;
            double_task.func_ = [n](std::vector<TapeDouble> const& v)
            {
                auto middle = v.begin() + n;
                std::vector<TapeDouble> result(n);
                std::transform(v.begin(), middle, middle, result.begin(), std::plus<TapeDouble>());
                return result;
            };
            return double_task;            
        }
    };

    template <class TaskFactory>
    inline void compare_performance(TaskFactory factory, size_t n, std::ostream& out_str = fake_stream())
    {
        auto array_task = factory.get_array_task(n);
        auto double_task = factory.get_double_task(n);

        out_str << "Arrays:\n";
        performance_time array_perf = adjoint_performance(array_task, out_str);
        out_str << array_perf << "\n\n";

        out_str << "Doubles:\n";
        performance_time double_perf = adjoint_performance(double_task, out_str);
        out_str << double_perf << "\n\n";
    }

    inline void plus_performance(size_t n, std::ostream& out_str = fake_stream())
    {
        compare_performance(plus_task_factory(), n, out_str);        
    }

    inline void performance_tests()
    {
        std::ofstream of("performance_tests_output.txt");
        CppAD::tape_serializer<cl::InnerArray> serializer(of);
        serializer.precision(3);

        plus_performance(10, serializer);
    }
}

#endif // cl_tape_examples_impl_performance_tests_hpp