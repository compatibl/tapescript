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

#ifndef cl_tape_examples_impl_performance_utils_hpp
#define cl_tape_examples_impl_performance_utils_hpp

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

    struct test_statistic
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
        // tfunc memory.
        size_t mem_;

        double total_time() const
        {
            return rec_ + init_ + forw_ + rev_;
        }
    };

    inline std::ostream& operator<<(std::ostream& os, test_statistic const& stat)
    {
        os << "Tape recording time:                     " << stat.rec_  << std::endl;
        os << "Ininial Forward(0) sweep time:           " << stat.init_ << std::endl;
        os << "Forward sweep calculations time:         " << stat.forw_ << std::endl;
        os << "Reverse sweep calculations time:         " << stat.rev_  << std::endl;
        os << "Total time for derivatives calculation:  " << stat.total_time() << std::endl;
        os << "tfunc memory (B): " << stat.mem_  << std::endl;
        return os;
    }

    struct time_columns
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

        static std::deque<double> get_y(test_statistic const& stat)
        {
            return{ stat.rec_, stat.init_, stat.forw_, stat.rev_ };
        }
    };

    struct mem_columns
    {
        static std::deque<std::string> get_columns()
        {
            static std::deque<std::string> const& columns =
            {
                "Input size"
                , "Optimazed tape memory"
                , "Non-optimazed tape memory"
            };

            return columns;
        }

        static std::deque<double> get_y(test_statistic const& array_stat, test_statistic const& double_stat)
        {
            return{ (double)array_stat.mem_, (double)double_stat.mem_ };
        }
    };

    struct total_time_columns
    {
        static std::deque<std::string> get_columns()
        {
            static std::deque<std::string> const& columns =
            {
                "Input size"
                , "Optimazed"
                , "Non-optimazed"
            };

            return columns;
        }

        static std::deque<double> get_y(test_statistic const& array_stat, test_statistic const& double_stat)
        {
            return{ array_stat.total_time(), double_stat.total_time() };
        }
    };

    template <class Columns = time_columns>
    struct plot_struct
    {
        plot_struct()
            : y_()
            , x_()
        {}

        template <class ... Args>
        plot_struct(test_statistic const& stat, Args... args)
            : y_(Columns::get_y(stat, args...))
            , x_(stat.size_)
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
        static cl::tape_serializer_base fake(stream);
        return fake;
    }

    template <class Inner>
    struct adjoint_task
    {
        typedef Inner inner_type;
        typedef tape_wrapper<Inner> tape_type;

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
    inline test_statistic adjoint_performance(AdjointTask const& task, std::ostream& out_stream = fake_stream())
    {
        typedef typename AdjointTask::inner_type inner_type;
        typedef typename AdjointTask::tape_type tape_type;
        test_statistic result;
        result.size_ = task.size_;

        // Input values initialization.
        std::vector<inner_type> x_val = task.X_;
        std::vector<tape_type> X(x_val.begin(), x_val.end());
        out_str << "Input vector: " << X << "\n";

        // Output vector.
        std::vector<tape_type> Y;
        // Tape function.
        cl::tfunc<inner_type> f;

        result.rec_ = test_performance(task.repeat_, [&X, &Y, &f, &task]()
        {
            // Declare the X vector as independent and start a tape recording.
            cl::tape_start(X);
            // Output calculations.
            Y = task.func_(X);
            //Stop tape recording.
            f.tape_read(X, Y);
        });

        out_str << "Output vector: " << Y << "\n\n";

        out_str << "Ininial Forward(0) sweep...\n\n";
        result.init_ = test_performance(task.repeat_, [&f, &x_val]()
        {
            f.forward(0, x_val);
        });

        // Forward sweep calculations.
        std::vector<inner_type> dx = task.dx_;
        out_str << "Forward(1, dx) sweep for dx = " << dx << "..." << std::endl;
        std::vector<inner_type> forw = f.forward(1, dx, out_stream);
        out_str << "Forward sweep result: " << forw << "\n\n";

        result.forw_ = test_performance(task.repeat_, [&f, &dx]()
        {
            f.forward(1, dx);
        });

        // Reverse sweep calculations.
        std::vector<inner_type> w = task.w_;
        out_str << "Reverse(1, w) sweep for w = " << w << "..." << std::endl;
        std::vector<inner_type> rev = f.reverse(1, w);
        out_str << "Reverse sweep result: " << rev << "\n\n\n";

        result.rev_ = test_performance(task.repeat_, [&f, &w]()
        {
            f.reverse(1, w);
        });

        result.mem_ = f.Memory();

        return result;
    }

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

    template <class TaskFactory>
    inline void compare_tape(TaskFactory factory, size_t n, std::ostream& out_stream = fake_stream())
    {
        auto array_task = factory.get_array_task(n);
        auto double_task = factory.get_double_task(n);

        out_str << "Arrays:\n\n";
        test_statistic array_stat = adjoint_performance(array_task, out_stream);

        out_str << "Doubles:\n\n";
        test_statistic double_stat = adjoint_performance(double_task, out_stream);
    }

    template <class TaskFactory>
    inline void performance_plot(std::string const& folder_name, TaskFactory factory, std::ostream& out_stream = fake_stream())
    {
        cl::tape_empty_test_output outMemory(folder_name, {
            { "title", "Tape size dependence on number of variables" }
            , { "filename", "TapeSize" }
            , { "not_clear", "Not" }
            , { "ylabel", "Memory (B)" }
            , { "cleanlog", "false" }
        });

        compare_tape(factory, 10, out_stream);

#if defined CL_GRAPH_GEN
        for (size_t i = 1; i <= 50; i++)
        {
            auto array_task = factory.get_array_task(i);
            auto double_task = factory.get_double_task(i);
            test_statistic array_stat = adjoint_performance(array_task, out_stream);
            test_statistic double_stat = adjoint_performance(double_task, out_stream);
            plot_struct<mem_columns> memory(array_stat, double_stat);
            outMemory << memory;
        }
#endif
    }

}

#endif // cl_tape_examples_impl_performance_utils_hpp