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

#define BOOST_FILESYSTEM_NO_LIB
#define BOOST_SYSTEM_NO_LIB

#include <string>
#include <vector>
#include <sstream>
#include <ostream>

#include <boost/config.hpp>
#include <boost/range/end.hpp>
#include <boost/range/begin.hpp>

#if defined CL_GRAPH_GEN

#define CL_GRAPHPDF_GEN // Enable pdf file generator

#include <boost/system/config.hpp>
#include <boost/filesystem.hpp>

#if defined CL_GRAPHPDF_GEN
#   include <windows.h>

namespace
{
    inline bool shell_execute(std::string const& filename)
    {
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        SHELLEXECUTEINFOA shexinfo = { 0, 0, GetDesktopWindow(), NULL, filename.c_str(), NULL, NULL };

        shexinfo.cbSize = sizeof (SHELLEXECUTEINFO);
        shexinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shexinfo.nShow = SW_SHOWNORMAL;

        ShellExecuteEx(&shexinfo);

        WaitForSingleObject(shexinfo.hProcess, INFINITE);

        return shexinfo.hProcess != INVALID_HANDLE_VALUE;
    }
}

#endif

namespace fs = boost::filesystem;
#endif

//#include
namespace cl
{
    namespace tapescript
    {
        struct dummy;

        template <typename > struct solve { typedef dummy type; };

        // Default colors and additional settings calculation about
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

#if defined CL_GRAPH_GEN
        // Path maker
        // args: path_out - ouput path
        //     , not_clear - if true the folder will not clean before output
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

                    // If it already exists we try to remove previous versions
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

#if defined CL_GRAPHPDF_GEN
        inline void execute_(std::string const& file_path, std::string const& filename)
        {
#if defined CL_TRACE_OUTPUT
            std::cout << "file path: " << (file_path + ".plt").c_str() << std::endl;
#endif
            shell_execute(file_path + ".plt");

            // Copy pdf files from output directory
            // perhaps it is in a current directory
            std::string result_pdf_path = fs::current_path().string() + std::string("//") + filename;
            if (fs::exists(result_pdf_path))
            {
                fs::copy_file(result_pdf_path, file_path + ".pdf", fs::copy_option::overwrite_if_exists);
                fs::remove(result_pdf_path);
            }
        }
#endif

#endif

    }

#if defined CL_GRAPH_GEN
    // Stream of output
    template <typename stream_type = std::ofstream>
    struct tape_test_output
    {
        // Settings type
        typedef std::map<std::string, std::string> settings_type;

        // Methods return certain setting
        // args: stt - map of settings
        //     , n - name of setting
        //     , defualt_v - default value if setting isn't present
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
        tape_test_output(std::string const& outpath, settings_type const& settings = settings_type())
            : settings_(settings)
            , path_(tapescript::make_test_path(tapescript::config().root_ouput_path_ + "//" + outpath, !settings_["not_clear"].empty()))
            , file_path_(path_.first + setting(settings_, "filename", "AdjointPerformance"))
            , of_(file_path_ + ".csv")
            , plt_of_(file_path_ + ".plt")
            , log_(path_.first + "Log.txt", setting(settings_, "cleanlog", "true") == "true" ? std::ofstream::out : std::ofstream::out | std::ofstream::app)
            , title_(setting(settings_, "title", path_.second))
            , graphics_()
            , columns_()
            , struct_start_(-1)
        { }

        // default destructor
        ~tape_test_output()
        {   }
    private:
        inline void save_pdf()
        {
            this->plt_of_.close();

            tapescript::execute_(file_path_, setting(settings_, "filename", "AdjointPerformance") + ".pdf");
            this->plt_of_.open(file_path_ + ".plt");
        }

        template <typename Struct>
        inline void write_pdf_header()
        {
            std::string na = setting(settings_, "filename", "AdjointPerformance") + ".pdf";
            plt_of_ << "set terminal pdf color font \"helvetica,16\" dashed size 7, 5" << std::endl;
            plt_of_ << "set output '" << na << "'" << std::endl;
            plt_of_ << "set size 1, 1" << std::endl;
        }

        template <typename Struct>
        inline void write_pdf_bottom()
        {
            plt_of_ << "unset output" << std::endl;
            plt_of_ << "set terminal pop" << std::endl;
            plt_of_ << "exit gnuplot" << std::endl;
        }

        template <typename Struct>
        inline void write_plt_header()
        {
            plt_of_ << "set terminal pdfcairo font \"sans,11\"" << std::endl;
            plt_of_ << "set terminal windows font \"helvetica,16\"" << std::endl;
        }

        template <typename Struct>
        inline void write_graph_header()
        {
            plt_of_ << "set locale \"English_US\"" << std::endl;
            plt_of_ << "set encoding utf8" << std::endl;
            plt_of_ << "set decimalsign '.'" << std::endl;
            plt_of_ << "set datafile separator ';'" << std::endl;

            plt_of_ << "set for[i = 1:25] linetype i dt i" << std::endl;

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
            static int line_types[] = { 1, 4, 5, 3, 6, 7, 8 };
            int lt_ix = 0;

            // Output graphic lines style
            auto color_where = boost::begin(settings::colors()), end_color = boost::end(settings::colors());
            plt_of_ << "plot ";
            std::for_each(columns_.begin() + 1, columns_.end(), [this, &lt_ix, &color_where, &end_color, &lw](std::string& n)
            {
                std::stringstream lt;
                lt << ((lt_ix < (sizeof(line_types) / sizeof(int))) ? line_types[lt_ix++] : lt_ix++);
                std::string const& color = color_where == end_color ? std::string("blue") : *color_where++;
                plt_of_ << "\"-\" using 1:2 w lines title \"" << n << "\" lt " << lt.str() << " lw " << lw << " lc \"" << color << "\",\\" << std::endl;
            });
        }

        template <int, typename > void print_()
        {
#           pragma message (__FUNCSIG__)
        }

    public:

        inline stream_type& log()
        {
            return log_;
        }

        // method writes generated plt graphics
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
        }

        template <typename Type>
        inline bool is_delimiter(Type v) { return false; }

        inline bool is_delimiter(char const* s)
        {
            return std::string(s) == ";";
        }

        template <typename Type>
        tape_test_output& operator << (Type const& v)
        {
            of_ << v;
            if (struct_start_ >= 0 && is_delimiter(v))
                return *this;

            // Mask to skip certain row
            static int mask_print[] = { 0, 1, 2, 3 };

            if (struct_start_ >= 0)
            {
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
        tape_test_output& operator<<(standard_manipulator mnp)
        {
            of_ << mnp;

            if (struct_start_ < 0)
                plt_of_ << mnp;

            return *this;
        }
    private:
        template <typename Struct>
        inline void write_vector_2stms_(std::vector<Struct >& output, bool fill_graphics_date = true)
        {
            plt_of_ << "#State;Row" << std::endl;
            typedef std::vector <Struct> otype;
            if (fill_graphics_date)
                std::for_each(output.begin(), output.end()
                , [this](Struct& v)
            {
                struct_start_ = std::is_class<Struct>::type::value ? 0 : -1;
                *this << v;
                struct_start_ = -1;
            });

            makeSmooth(graphics_, setting(settings_, "smooth", "0")
                , [this](const std::string& s){ return s != columns_[0]; });
        }
    public:
        // General output operator for vectors
        template <typename Struct>
        inline void operator << (std::vector<Struct >& output)
        {
            columns_ = Struct::get_columns();
            for each(std::string const &cs in columns_)
            {
                of_ << cs << ";";
            }
            of_ << std::endl;

            // Generate pdf file
#if defined CL_GRAPHPDF_GEN
            this->write_pdf_header<Struct>();
            this->write_graph_header<Struct>();

            this->write_vector_2stms_(output);
            this->write_graphics();

            this->save_pdf();
#endif
            // generate plt
            this->write_plt_header<Struct>();
            this->write_graph_header<Struct>();

            this->write_vector_2stms_(output, false);

            this->write_graphics();
        }

        /// <summary>Setting field can be provided by user.</summary>
        settings_type settings_;

        std::pair<std::string, std::string> path_;
        std::string file_path_;
        stream_type of_;
        stream_type plt_of_;
        stream_type log_;
        std::string title_;

        std::map<std::string, std::deque<std::string >> graphics_;
        std::deque<std::string> columns_;

        int struct_start_;
    };

    //#if defined CL_GRAPH_GEN
    typedef tape_test_output<> tape_empty_test_output;
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

    typedef fake_stream tape_empty_test_output;
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