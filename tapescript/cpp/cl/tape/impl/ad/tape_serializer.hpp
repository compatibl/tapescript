
# ifndef TAPE_SERIALIZER_INCLUDED
#define TAPE_SERIALIZER_INCLUDED
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

namespace CppAD
{
    template <class Base>
    struct tape_serializer : std::ostream
    {
        struct impl {};
        struct impl_start{};

        tape_serializer(std::ostream& os = std::cout)
            : std::ostream(os.rdbuf())
        {}

        ~tape_serializer()  { std::ostream::clear(); }

        void start()
        {
            *this << "Op#    Res#   Op       Operands          Taylor coeficients \n";
        }

        template <class Type>
        void saveOpField(
            const char *    leader,
            const Type     &value,
            size_t          width)
        {
            printOpField(*this, leader, value, width);
        }

        template<typename Base>
        inline void saveOp(
            const player<Base>*    play,
            size_t                 i_op,
            size_t                 i_var,
            OpCode                 op,
            const addr_t*          ind)
        {
            std::stringstream ss;
            printOp(
                ss,
                play,
                i_op,
                i_var,
                op,
                ind);
            saveOpField("", ss.str(), 41);
        }

        template <class Value>
        void saveOpResult(
            size_t                 nfz,
            const  Value          *fz,
            size_t                 nrz,
            const  Value          *rz)
        {
            const char* prefix = "";// "\n                                         ";
            const size_t width = 20;

            std::ostringstream buffer;
            buffer << std::left;
            size_t k;
            if (nfz != 0)
                buffer << "| fz[" << 0 << "] = " << std::setw(width) << fz[0];
            for (k = 1; k < nfz; k++)
                buffer << prefix << "| fz[" << k << "] = " << std::setw(width) << fz[k];
            for (k = 0; k < nrz; k++)
                buffer << prefix << "| rz[" << k << "] = " << std::setw(width) << rz[k];
            *this << buffer.str();
        }
    };
}

#endif TAPE_SERIALIZER_INCLUDED