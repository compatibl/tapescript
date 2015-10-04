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
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

#ifndef cl_tape_impl_ad_tape_serializer_hpp
#define cl_tape_impl_ad_tape_serializer_hpp

namespace cl
{
    struct tape_serializer_base : std::ostream
    {
        struct impl {};
        struct io_typed {};

        tape_serializer_base(std::ostream& os = std::cout)
            : std::ostream(os.rdbuf())
            , first_call_(true)
        {}

        size_t io_type()
        {
            return cl::serializer_type::io_text;
        }

        ~tape_serializer_base()  { std::ostream::clear(); }

        static inline const char* OpName(CppAD::OpCode op)
        {
            static const char *OpNameTable[] = {
                "abs",
                "acos",
                "+",
                "+",
                "asin",
                "atan",
                "Begin",
                "? :",
                "cos",
                "cosh",
                "?", //"CSkip",
                "CSum",
                "Dis",
                "/",
                "/",
                "/",
                "End",
                "==",
                "==",
                "Erf",
                "exp",
                "Init",
                "Ldp",
                "Ldv",
                "Lepv",
                "Levp",
                "Levv",
                "log",
                "<",
                "<",
                "<",
                "*",
                "*",
                "!=",
                "!=",
                "Par",
                "pow",
                "pow",
                "pow",
                "Print",
                "sign",
                "sin",
                "sinh",
                "sqrt",
                "Stpp",
                "Stpv",
                "Stvp",
                "Stvv",
                "-",
                "-",
                "-",
                "tan",
                "tanh",
                "User",
                "Usrap",
                "Usrav",
                "Usrrp",
                "Usrrv"
            };

            return OpNameTable[op];            
        }

        static std::string header()
        {
            return "\nOp#  Res# Op       Operands#          Taylor coefficients\n";
        }

        static std::string footer()
        {
            return "\n";
        }

        void check_last_call(CppAD::OpCode op)
        {
            if (op == CppAD::BeginOp || op == CppAD::EndOp)
            {
                *this << footer();
                first_call_ = true;
            }            
        }

        void check_first_call()
        {
            if (first_call_)
            {
                *this << header();
                first_call_ = false;
            }
        }

        template <class Type>
        void saveOpField(
            const char *    leader,
            const Type     &value,
            size_t          width)
        {
            printOpField(*this, leader, value, width);
        }

        template <class Base>
        static void saveOp(
            std::ostream&          os,
            const CppAD::player<Base>*    play,
            size_t                 i_op,
            size_t                 i_var,
            CppAD::OpCode                 op,
            const CppAD::addr_t*          ind)
        {
            size_t i;

            using namespace CppAD;

            CPPAD_ASSERT_KNOWN(
                !thread_alloc::in_parallel(),
                "cannot print trace of AD operations in parallel mode"
                );
            static std::string CompareOpName[] =
            { " < ", "<= ", "== ", ">= ", " > ", "!= " };

            // print operator
            printOpField(os, "", i_op, 5);
            if (NumRes(op) > 0 && op != BeginOp)
                printOpField(os, "", i_var, 5);
            else	printOpField(os, "", "", 5);
            if (op == CExpOp || op == CSkipOp)
            {
                printOpField(os, "", CompareOpName[ind[0]] + OpName(op), 8);
            }
            else	printOpField(os, "", OpName(op), 8);

            // print other fields
            size_t ncol = 5;
            switch (op)
            {
            case CSkipOp:
                /*
                ind[0]     = the Rel operator: Lt, Le, Eq, Ge, Gt, or Ne
                ind[1] & 1 = is left a variable
                ind[1] & 2 = is right a variable
                ind[2]     = index correspoding to left
                ind[3]     = index correspoding to right
                ind[4] = number of operations to skip if CExpOp comparision is true
                ind[5] = number of operations to skip if CExpOp comparision is false
                ind[6] -> ind[5+ind[4]]               = skip operations if true
                ind[6+ind[4]] -> ind[5+ind[4]+ind[5]] = skip operations if false
                ind[6+ind[4]+ind[5]] = ind[4] + ind[5]
                */
                CPPAD_ASSERT_UNKNOWN(ind[6 + ind[4] + ind[5]] == ind[4] + ind[5]);
                CPPAD_ASSERT_UNKNOWN(ind[1] != 0);
                if (ind[1] & 1)
                    printOpField(os, " vl=", ind[2], ncol);
                else	printOpField(os, " pl=", play->GetPar(ind[2]), ncol);
                if (ind[1] & 2)
                    printOpField(os, " vr=", ind[3], ncol);
                else	printOpField(os, " pr=", play->GetPar(ind[3]), ncol);
                if (size_t(ind[4]) < 3)
                {
                    for (i = 0; i < size_t(ind[4]); i++)
                        printOpField(os, " ot=", ind[6 + i], ncol);
                }
                else
                {
                    printOpField(os, "\n\tot=", ind[6 + 0], ncol);
                    for (i = 1; i < size_t(ind[4]); i++)
                        printOpField(os, " ot=", ind[6 + i], ncol);
                }
                if (size_t(ind[5]) < 3)
                {
                    for (i = 0; i < size_t(ind[5]); i++)
                        printOpField(os, " of=", ind[6 + ind[4] + i], ncol);
                }
                else
                {
                    printOpField(os, "\n\tof=", ind[6 + ind[4] + 0], ncol);
                    {	for (i = 1; i < size_t(ind[5]); i++)
                        printOpField(os, " of=", ind[6 + ind[4] + i], ncol);
                    }
                }
                break;

            case CSumOp:
                /*
                ind[0] = number of addition variables in summation
                ind[1] = number of subtraction variables in summation
                ind[2] = index of parameter that initializes summation
                ind[3], ... , ind[2+ind[0]] = index for positive variables
                ind[3+ind[0]], ..., ind[2+ind[0]+ind[1]] = negative variables
                ind[3+ind[0]+ind[1]] == ind[0] + ind[1]
                */
                CPPAD_ASSERT_UNKNOWN(ind[3 + ind[0] + ind[1]] == ind[0] + ind[1]);
                printOpField(os, " pr=", play->GetPar(ind[2]), ncol);
                for (i = 0; i < size_t(ind[0]); i++)
                    printOpField(os, " +v=", ind[3 + i], ncol);
                for (i = 0; i < size_t(ind[1]); i++)
                    printOpField(os, " -v=", ind[3 + ind[0] + i], ncol);
                break;

            case LdpOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                printOpField(os, "off=", ind[0], ncol);
                printOpField(os, "idx=", ind[1], ncol);
                break;

            case LdvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                printOpField(os, "off=", ind[0], ncol);
                printOpField(os, "  v=", ind[1], ncol);
                break;

            case StppOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                printOpField(os, "off=", ind[0], ncol);
                printOpField(os, "idx=", ind[1], ncol);
                printOpField(os, " pr=", play->GetPar(ind[2]), ncol);
                break;

            case StpvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                printOpField(os, "off=", ind[0], ncol);
                printOpField(os, "idx=", ind[1], ncol);
                printOpField(os, " vr=", ind[2], ncol);
                break;

            case StvpOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                printOpField(os, "off=", ind[0], ncol);
                printOpField(os, " vl=", ind[1], ncol);
                printOpField(os, " pr=", play->GetPar(ind[2]), ncol);
                break;

            case StvvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                printOpField(os, "off=", ind[0], ncol);
                printOpField(os, " vl=", ind[1], ncol);
                printOpField(os, " vr=", ind[2], ncol);
                break;

            case AddvvOp:
            case DivvvOp:
            case LevvOp:
            case LtvvOp:
            case EqvvOp:
            case NevvOp:
            case MulvvOp:
            case PowvvOp:
            case SubvvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 2);
                printOpField(os, " vl=", ind[0], ncol);
                printOpField(os, " vr=", ind[1], ncol);
                break;

            case AddpvOp:
            case LepvOp:
            case LtpvOp:
            case EqpvOp:
            case NepvOp:
            case SubpvOp:
            case MulpvOp:
            case PowpvOp:
            case DivpvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 2);
                printOpField(os, " pl=", play->GetPar(ind[0]), ncol);
                printOpField(os, " vr=", ind[1], ncol);
                break;

            case DivvpOp:
            case LevpOp:
            case LtvpOp:
            case PowvpOp:
            case SubvpOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 2);
                printOpField(os, " vl=", ind[0], ncol);
                printOpField(os, " pr=", play->GetPar(ind[1]), ncol);
                break;

            case AbsOp:
            case AcosOp:
            case AsinOp:
            case AtanOp:
            case CosOp:
            case CoshOp:
            case ExpOp:
            case LogOp:
            case SignOp:
            case SinOp:
            case SinhOp:
            case SqrtOp:
            case UsravOp:
            case TanOp:
            case TanhOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 1);
                printOpField(os, "  v=", ind[0], ncol);
                break;

            case ErfOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                // ind[1] points to the parameter 0
                // ind[2] points to the parameter 2 / sqrt(pi)
                printOpField(os, "  v=", ind[0], ncol);
                break;

            case ParOp:
            case UsrapOp:
            case UsrrpOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 1);
                printOpField(os, "  p=", play->GetPar(ind[0]), ncol);
                break;

            case UserOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 4);
                {	std::string name = atomic_base<Base>::class_name(ind[0]);
                    os << " " + name;
                }
                break;

            case PriOp:
                CPPAD_ASSERT_NARG_NRES(op, 5, 0);
                if (ind[0] & 1)
                    printOpField(os, " v=", ind[1], ncol);
                else	printOpField(os, " p=", play->GetPar(ind[1]), ncol);
                os << "before=\"" << play->GetTxt(ind[2]) << "\"";
                if (ind[0] & 2)
                    printOpField(os, " v=", ind[3], ncol);
                else	printOpField(os, " p=", play->GetPar(ind[3]), ncol);
                os << "after=\"" << play->GetTxt(ind[4]) << "\"";
                break;

            case BeginOp:
                // argument not used (created by independent)
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 1);
                break;

            case EndOp:
            case InvOp:
            case UsrrvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 0);
                break;

            case DisOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 2);
                {	const char* name = discrete<Base>::name(ind[0]);
                printOpField(os, " f=", name, ncol);
                printOpField(os, " x=", ind[1], ncol);
                }
                break;


            case CExpOp:
                CPPAD_ASSERT_UNKNOWN(ind[1] != 0);
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 6);
                if (ind[1] & 1)
                    printOpField(os, " vl=", ind[2], ncol);
                else	printOpField(os, " pl=", play->GetPar(ind[2]), ncol);
                if (ind[1] & 2)
                    printOpField(os, " vr=", ind[3], ncol);
                else	printOpField(os, " pr=", play->GetPar(ind[3]), ncol);
                if (ind[1] & 4)
                    printOpField(os, " vt=", ind[4], ncol);
                else	printOpField(os, " pt=", play->GetPar(ind[4]), ncol);
                if (ind[1] & 8)
                    printOpField(os, " vf=", ind[5], ncol);
                else	printOpField(os, " pf=", play->GetPar(ind[5]), ncol);
                break;

            default:
                CPPAD_ASSERT_UNKNOWN(0);
            }
        }

        template<typename Base>
        inline void saveOp(
            const CppAD::player<Base>*    play,
            size_t                 i_op,
            size_t                 i_var,
            CppAD::OpCode                 op,
            const CppAD::addr_t*          ind)
        {
            using namespace CppAD;

            check_first_call();
            size_t length = (op == UserOp) ? 80 : 37;

            std::stringstream ss;
            saveOp(
                ss,
                play,
                i_op,
                i_var,
                op,
                ind);
            *this << std::left << std::setw(length) << ss.str();

            check_last_call(op);
        }

        template <class Value>
        void saveOpResult(
            size_t                 nfz,
            const  Value          *fz,
            size_t                 nrz,
            const  Value          *rz)
        {
            using namespace CppAD;

            const char* prefix = "";// "\n                                         ";
            const size_t width = 20;

            std::ostringstream buffer;
            buffer.precision(this->precision());
            buffer << std::left;
            size_t k;
            if (nfz != 0)
                buffer << " fz[" << 0 << "] = " << std::setw(width) << fz[0];
            for (k = 1; k < nfz; k++)
                buffer << prefix << " fz[" << k << "] = " << std::setw(width) << fz[k];
            for (k = 0; k < nrz; k++)
                buffer << prefix << " rz[" << k << "] = " << std::setw(width) << rz[k];
            *this << buffer.str();
        }

        bool first_call_;
    };
    
    template <class Base>
    struct tape_serializer
        : tape_serializer_base
    {
        tape_serializer(std::ostream& os = std::cout)
            : tape_serializer_base(os)
        {}
    };
}

#endif // cl_tape_impl_ad_tape_serializer_hpp