
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
    template <typename T>
    struct tape_serializer : std::ostream
    {
        tape_serializer(std::ostream& os = std::cout) 
            : std::ostream(0)
            , os_(os)
        {}

        ~tape_serializer()  { std::ostream::clear(); }

        struct impl {};

        template <typename Ty_>
        inline tape_serializer& operator << (Ty_ const& v)
        {
            os_ << v;
            return *this;
        }

        template <class Type>
        void saveOpField(
            const char *   leader,
            const Type     &value,
            size_t          width)
        {
            std::ostringstream buffer;
            std::string        str;

            // first print the leader
            *this << leader;

            // print the value into an internal buffer
            buffer << std::setw(width) << value;
            str = buffer.str();

            // length of the string
            size_t len = str.size();
            if (len > width)
            {
                size_t i;
                for (i = 0; i < width - 1; i++)
                    *this << str[i];
                *this << "*";
                return;
            }

            // count number of spaces at begining
            size_t nspace = 0;
            while (str[nspace] == ' ' && nspace < len)
                nspace++;

            // left justify the string
            size_t i = nspace;
            while (i < len)
                *this << str[i++];

            i = width - len + nspace;
            while (i--)
                *this << " ";
        }

        template<typename Base> 
        inline void saveOp(
            const player<Base>*    play,
            size_t                 i_op,
            size_t                 i_var,
            OpCode                 op,
            const addr_t*          ind)
        {
            size_t i;
            /*CPPAD_ASSERT_KNOWN(
                !thread_alloc::in_parallel(),
                "cannot print trace of AD operations in parallel mode"
                );*/
            static const char *CompareOpName[] =
            { "Lt", "Le", "Eq", "Ge", "Gt", "Ne" };

            // print operator
            saveOpField("o=", i_op, 5);
            if (NumRes(op) > 0 && op != BeginOp)
                saveOpField("v=", i_var, 5);
            else	saveOpField("v=", "", 5);
            if (op == CExpOp || op == CSkipOp)
            {
                saveOpField("", OpName(op), 5);
                saveOpField("", CompareOpName[ind[0]], 3);
            }
            else	saveOpField("", OpName(op), 8);

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
                    saveOpField(" vl=", ind[2], ncol);
                else	saveOpField(" pl=", play->GetPar(ind[2]), ncol);
                if (ind[1] & 2)
                    saveOpField(" vr=", ind[3], ncol);
                else	saveOpField(" pr=", play->GetPar(ind[3]), ncol);
                if (size_t(ind[4]) < 3)
                {
                    for (i = 0; i < size_t(ind[4]); i++)
                        saveOpField(" ot=", ind[6 + i], ncol);
                }
                else
                {
                    saveOpField("\n\tot=", ind[6 + 0], ncol);
                    for (i = 1; i < size_t(ind[4]); i++)
                        saveOpField(" ot=", ind[6 + i], ncol);
                }
                if (size_t(ind[5]) < 3)
                {
                    for (i = 0; i < size_t(ind[5]); i++)
                        saveOpField(" of=", ind[6 + ind[4] + i], ncol);
                }
                else
                {
                    saveOpField("\n\tof=", ind[6 + ind[4] + 0], ncol);
                    {	for (i = 1; i < size_t(ind[5]); i++)
                        saveOpField(" of=", ind[6 + ind[4] + i], ncol);
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
                saveOpField(" pr=", play->GetPar(ind[2]), ncol);
                for (i = 0; i < size_t(ind[0]); i++)
                    saveOpField(" +v=", ind[3 + i], ncol);
                for (i = 0; i < size_t(ind[1]); i++)
                    saveOpField(" -v=", ind[3 + ind[0] + i], ncol);
                break;

            case LdpOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                saveOpField("off=", ind[0], ncol);
                saveOpField("idx=", ind[1], ncol);
                break;

            case LdvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                saveOpField("off=", ind[0], ncol);
                saveOpField("  v=", ind[1], ncol);
                break;

            case StppOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                saveOpField("off=", ind[0], ncol);
                saveOpField("idx=", ind[1], ncol);
                saveOpField(" pr=", play->GetPar(ind[2]), ncol);
                break;

            case StpvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                saveOpField("off=", ind[0], ncol);
                saveOpField("idx=", ind[1], ncol);
                saveOpField(" vr=", ind[2], ncol);
                break;

            case StvpOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                saveOpField("off=", ind[0], ncol);
                saveOpField(" vl=", ind[1], ncol);
                saveOpField(" pr=", play->GetPar(ind[2]), ncol);
                break;

            case StvvOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                saveOpField("off=", ind[0], ncol);
                saveOpField(" vl=", ind[1], ncol);
                saveOpField(" vr=", ind[2], ncol);
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
                saveOpField(" vl=", ind[0], ncol);
                saveOpField(" vr=", ind[1], ncol);
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
                saveOpField(" pl=", play->GetPar(ind[0]), ncol);
                saveOpField(" vr=", ind[1], ncol);
                break;

            case DivvpOp:
            case LevpOp:
            case LtvpOp:
            case PowvpOp:
            case SubvpOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 2);
                saveOpField(" vl=", ind[0], ncol);
                saveOpField(" pr=", play->GetPar(ind[1]), ncol);
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
                saveOpField("  v=", ind[0], ncol);
                break;

            case ErfOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 3);
                // ind[1] points to the parameter 0
                // ind[2] points to the parameter 2 / sqrt(pi)
                saveOpField("  v=", ind[0], ncol);
                break;

            case ParOp:
            case UsrapOp:
            case UsrrpOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 1);
                saveOpField("  p=", play->GetPar(ind[0]), ncol);
                break;

            case UserOp:
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 4);
                {	std::string name = atomic_base<Base>::class_name(ind[0]);
                saveOpField(" f=", name.c_str(), ncol);
                saveOpField(" i=", ind[1], ncol);
                saveOpField(" n=", ind[2], ncol);
                saveOpField(" m=", ind[3], ncol);
                }
                break;

            case PriOp:
                CPPAD_ASSERT_NARG_NRES(op, 5, 0);
                if (ind[0] & 1)
                    saveOpField(" v=", ind[1], ncol);
                else	saveOpField(" p=", play->GetPar(ind[1]), ncol);
                *this << "before=\"" << play->GetTxt(ind[2]) << "\"";
                if (ind[0] & 2)
                    saveOpField(" v=", ind[3], ncol);
                else	saveOpField(" p=", play->GetPar(ind[3]), ncol);
                *this << "after=\"" << play->GetTxt(ind[4]) << "\"";
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
                saveOpField(" f=", name, ncol);
                saveOpField(" x=", ind[1], ncol);
                }
                break;


            case CExpOp:
                CPPAD_ASSERT_UNKNOWN(ind[1] != 0);
                CPPAD_ASSERT_UNKNOWN(NumArg(op) == 6);
                if (ind[1] & 1)
                    saveOpField(" vl=", ind[2], ncol);
                else	saveOpField(" pl=", play->GetPar(ind[2]), ncol);
                if (ind[1] & 2)
                    saveOpField(" vr=", ind[3], ncol);
                else	saveOpField(" pr=", play->GetPar(ind[3]), ncol);
                if (ind[1] & 4)
                    saveOpField(" vt=", ind[4], ncol);
                else	saveOpField(" pt=", play->GetPar(ind[4]), ncol);
                if (ind[1] & 8)
                    saveOpField(" vf=", ind[5], ncol);
                else	saveOpField(" pf=", play->GetPar(ind[5]), ncol);
                break;

            default:
                CPPAD_ASSERT_UNKNOWN(0);
            }
        }

        template <class Value>
        void saveOpResult(
            size_t                 nfz,
            const  Value          *fz,
            size_t                 nrz,
            const  Value          *rz)
        {
            size_t k;
            for (k = 0; k < nfz; k++)
                *this << "| fz[" << k << "]=" << fz[k];
            for (k = 0; k < nrz; k++)
                *this << "| rz[" << k << "]=" << rz[k];
        }

        std::ostream& os_;

        // this is the type of std::cout
        typedef std::basic_ostream<char, std::char_traits<char> > standard_cout;

        // this is the function signature of std::endl
        typedef standard_cout& (*endl_type_standard)(standard_cout&);

        // define an operator<< to take in std::endl
        tape_serializer<T>& 
        operator<<(endl_type_standard f_ptr)
        {
            // call the function, but we cannot return it's value
            os_ << f_ptr;
            return *this;
        }
    };
}

#endif TAPE_SERIALIZER_INCLUDED