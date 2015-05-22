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

#ifndef cl_tape_util_testoutput_hpp
#define cl_tape_util_testoutput_hpp

#include <string>
#include <vector>
#include <sstream>
#include <ostream>

#include <boost/config.hpp>
#include <boost/range/end.hpp>
#include <boost/range/begin.hpp>

#if defined CL_TAPE_TEST_OUTPUT
#	include <boost/system/config.hpp>
#	include <boost/filesystem.hpp>
	namespace fs = boost::filesystem;
#endif

namespace cl
{
    namespace tapescript
    {
        struct dummy;

        template <typename > struct solve { typedef dummy type; };

        //  Default colors and additional settings calculation about
        // the users can performs custom model
        template <typename Type, typename D = dummy>
        struct is_has_colors
        {
            typedef std::false_type has_type;

            // Colors placeholder
            struct settings
            {
                // Return defaults colors
                static std::vector<std::string >& colors()
                {
                    static std::vector<std::string > v = { "#2971AF", "#77C043", "#9FC9EB", "#FF7F00"
                        , "#FFCC45", "#A0759F", "#00684D", "#77C043", "#0B4A7E", "#7EA9CD"
                        , "#1AB8C4", "#704A9B", "#009900", "#FF9100", "#9898D3"
                        , "#77C043", "#FF7F00", "#0B4A7E", "#FFCC45", "#7EA9CD"
                        , "#A0759F", "#77C043", "#FF7F00", "#0B4A7E" };

                    return v;
                }
            };
        };

        // Partial specialization to SFINAE calculation
        // this case use perform custom colors and settings
        template <typename Type >
        struct is_has_colors<Type, typename solve<typename Type::settings>::type >
        {
            // The user colors settings
            typedef typename
            Type::settings settings;

            typedef std::true_type has_type;
        };

        // default configuration
        struct config
        {
            std::string root_ouput_path_ = "tests-output";
        };

#if defined CL_TAPE_TEST_OUTPUT
        // Path maker
        // args: path_out - ouput path
        //       , not_clear - if true the folder will not clean before output
        // return: path and title pair
        inline std::pair<std::string, std::string>
        make_test_path(std::string const& path_out, bool not_clear = false)
        {
                try
                {
                    // current directory perhaps bin
                    // we should move to one level up
                    boost::filesystem::path initdir = boost::filesystem::initial_path().parent_path();

                    // make path to output string path 
                    fs::path testout =
                        (initdir.string() + "//" + path_out);

                    std::string phpath = testout.string();
                    std::size_t off = testout.parent_path().string().length();
                    std::string title = phpath.substr(off, phpath.length() - off);

                    // Title calculation
                    title.erase(std::remove_if(title.begin(), title.end()
                        , [](char c) { return c == '/' || c == '\\'; }), title.end());

                    // If exixts we try to remove previous versions
                    if (fs::exists(testout))
                    {
                        if (!not_clear)
                            fs::remove_all(testout);
                    }
                    else
                    {
                        std::list<fs::path > dirs;
                        fs::path cp = testout.parent_path();

                        while (cp.has_parent_path() && !fs::exists(cp))
                        {
                            dirs.push_front(cp);
                            cp = cp.parent_path();
                        }

                        std::for_each(dirs.begin()
                            , dirs.end(), [](fs::path &ph){ fs::create_directory(ph); });
                    }

                    fs::create_directory(testout);

                    return std::make_pair(testout.string() + "\\", title);
                }
                catch (std::exception& ex)
                {
                    std::cout << ex.what() << std::endl;
                }

                return std::make_pair(std::string(), std::string());
            }
#endif
    }

#if defined CL_TAPE_TEST_OUTPUT
    // Stream of output 
    template <typename stream_type = std::ofstream>
    struct adjoint_test_out
    {
        // Settings type 
        typedef std::map<std::string, std::string> settings_type;

        // Methods return certain setting
        // args: stt - map of settings
        //       , n - name of setting 
        //       , defualt_v - default value if setting isn't present
        // return: setting value
        static std::string setting(settings_type &stt, std::string const& n, std::string const& default_v)
        {
            settings_type::iterator where = stt.lower_bound(n);
            if (where != stt.end()
                && where->first == n)
                return where->second;

            return default_v;
        }

        // default ctor
        adjoint_test_out(std::string const& outpath, settings_type const& settings = settings_type())
            : settings_(settings)
            , path_(tapescript::make_test_path(config().root_ouput_path_ + "//" + outpath, !settings_["not_clear"].empty()))
            , of_(path_.first + setting(settings_, "filename", "AdjointPerf") + ".csv")
            , plt_of_(path_.first + setting(settings_, "filename", "AdjointPerf") + ".plt")
            , log_(path_.first + "Log.csv")
            , title_(setting(settings_, "title", path_.second))
            , graphics_()
            , columns_()
            , struct_start_(-1)
        { }

        // default destructor
        ~adjoint_test_out()
        {   }

        template <typename Struct>
        inline void print_plt_header()
        {
            plt_of_ << "set terminal pdfcairo font \"sans,11\"" << std::endl;
            plt_of_ << "set terminal windows font \"helvetica,16\"" << std::endl;
            plt_of_ << "set locale \"English_US\"" << std::endl;
            plt_of_ << "set encoding utf8" << std::endl;
            plt_of_ << "set decimalsign '.'" << std::endl;
            plt_of_ << "set datafile separator ';'" << std::endl;

            plt_of_ << "set size 0.7, 1" << std::endl;
            plt_of_ << "unset grid" << std::endl;
            plt_of_ << "set border 1 + 2" << std::endl;
            plt_of_ << "set xtics nomirror font \",16\"" << std::endl;
            plt_of_ << "set ytics nomirror font \",16\"" << std::endl;

            typedef typename
                tapescript::is_has_colors<Struct>::settings settings;

            // print_<0, std::pair<typename tapescript::is_has_colors<Struct>::has_type, Struct>>();

            std::size_t line = 1;
            if (columns_.size() > 2)
            {
                std::find_if(boost::begin(settings::colors()), boost::end(settings::colors())
                    , [this, &line](std::string& color)
                {
                    plt_of_ << "set style line "
                        << line++
                        << " linecolor rgb \""
                        << color << "\" lt 3 lw " << setting(this->settings_, "lw", "3") << std::endl;

                    return line >= columns_.size();
                });

                plt_of_ << std::endl;
                std::string width = setting(settings_, "line_box_width", "10");
                plt_of_ << "set key left width " << width << " height 0.3 font \", 16\" box lw 0.7" << std::endl;
            }

            std::string xlabel = setting(this->settings_, "xlabel", columns_[0]);
            plt_of_ << "set xlabel \"" << xlabel << "\"" << std::endl;
            std::string yl = setting(settings_, "ylabel", "");

            if (!yl.empty())
                plt_of_ << "set ylabel \"" << yl << "\"" << std::endl;

            plt_of_ << "set title \"" << title_ << "\"" << std::endl;

            std::string lw = setting(this->settings_, "lw", "3");
            int lt_ix = 2;

            // Output graphic lines style 
            auto color_where = boost::begin(settings::colors()), end_color = boost::end(settings::colors());
            plt_of_ << "plot ";
            std::for_each(columns_.begin() + 1, columns_.end(), [this, &lt_ix, &color_where, &end_color, &lw](std::string& n)
            {
                std::stringstream lt;
                lt << lt_ix++;
                std::string const& color = color_where == end_color ? std::string("blue") : *color_where++;
                plt_of_ << "\"-\" using 1:2 w lines title \"" << n << "\" lt " << lt.str() << " lw " << lw << " lc \"" << color << "\",\\" << std::endl;
            });
        }

        inline stream_type& log()
        {
            return log_;
        }

        template <int, typename > void print_()
        {
#pragma message (__FUNCSIG__)
        }

        // method write generate plt graphics 
        // args:
        // return:
        inline void write_graphics()
        {
            if (!columns_.size())
                return;

            std::string independent = columns_[0];
            std::size_t colcount = columns_.size();
            std::deque<std::string>::iterator ind_values = graphics_[independent].begin();
            for (unsigned int col = 1; col < colcount; ++col)
            {
                std::deque<std::string >& col_values = graphics_[columns_[col]];
                std::for_each(col_values.begin(), col_values.end(),
                    [&ind_values, this](std::string& v)
                {
                    this->plt_of_ << ind_values->c_str() << ";" << v.c_str() << std::endl;
                    ++ind_values;
                });

                this->plt_of_ << "end" << std::endl;
                ind_values = graphics_[independent].begin();
            }

            plt_of_.clear();
            of_.clear();
        }

        template <typename Type>
        inline bool is_delimiter(Type v) { return false; }

        inline bool is_delimiter(char const* s)
        {
            return std::string(s) == ";";
        }

        template <typename Type>
        adjoint_test_out& operator << (Type const& v)
        {
            of_ << v;
            if (struct_start_ >= 0 && is_delimiter(v))
                return *this;

            // Mask to skip certain row
            static int mask_print[] = { 0, 1, 2, 3 };

            if (struct_start_ >= 0)
            {
                // plt_of_ << v;
                std::stringstream ss_;
                ss_ << v;

                graphics_[columns_[struct_start_]]
                    .push_back(ss_.str());
            }

            ++struct_start_;

            return *this;
        }

        // this is the type of std::cout
        typedef std::basic_ostream<char, std::char_traits<char> > cout_manip_type;

        // this is the function signature of std::endl
        typedef cout_manip_type& (*standard_manipulator)(cout_manip_type&);

        // define an operator<< to take in std::endl
        adjoint_test_out& operator<<(standard_manipulator mnp)
        {
            of_ << mnp;

            if (struct_start_ < 0)
                plt_of_ << mnp;

            return *this;
        }

        // General output operator for vectors 
        template <typename Struct>
        void operator << (std::vector<Struct >& ouput)
        {
            columns_ = Struct::get_columns();

            print_plt_header<Struct>();

            plt_of_ << "#State;Row" << std::endl;

            for each(std::string const &cs in columns_)
            {
                of_ << cs << ";";
            }

            of_ << std::endl;

            typedef std::vector <Struct> otype;

            std::for_each(ouput.begin(), ouput.end()
                , [this](Struct& v)
            {
                struct_start_ = std::is_class<Struct>::type::value ? 0 : -1;
                *this << v;
                struct_start_ = -1;
            });

            this->write_graphics();
        }

        /// <summary> setting field can be provided by user </summary>
        settings_type settings_;

        std::pair<std::string, std::string> path_;
        stream_type of_;
        stream_type plt_of_;
        stream_type log_;
        std::string title_;

        std::map<std::string, std::deque<std::string >> graphics_;
        std::deque<std::string> columns_;

        int struct_start_;
    };


    typedef adjoint_test_out<> AdjointTestOutput;
#else
    struct fake_stream
    {
        typedef std::map<std::string, std::string> settings_type;

        fake_stream(std::string const& outpath, settings_type const& settings = settings_type())
        {   }

        template <typename Type>
        fake_stream& operator << (Type const& v)
        {
            return *this;
        }

        // this is the type of std::cout
        typedef std::basic_ostream<char, std::char_traits<char> > cout_manip_type;

        // this is the function signature of std::endl
        typedef cout_manip_type& (*standard_manipulator)(cout_manip_type&);

        // define an operator<< to take in std::endl
        fake_stream& operator<<(standard_manipulator mnp)
        {
            return *this;
        }

        fake_stream& log() { return *this; }
    };

    typedef fake_stream AdjointTestOutput;
#endif

    struct logger_traceout
    {
        logger_traceout(std::string const& fn) : log_(fn)
        { }

        template <typename Type>
        inline logger_traceout& operator << (Type const& v)
        {
            log_ << v;
            return *this;
        }
        // this is the type of std::cout
        typedef std::basic_ostream<char, std::char_traits<char> > cout_manip_type;

        // this is the function signature of std::endl
        typedef cout_manip_type& (*standard_manipulator)(cout_manip_type&);

        // define an operator<< to take in std::endl
        logger_traceout& operator<<(standard_manipulator mnp)
        {
            log_ << mnp;
            return *this;
        }

        std::ofstream log_;
    };
}


#endif // cl_tape_util_testoutput_hpp