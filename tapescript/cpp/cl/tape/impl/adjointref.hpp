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

#ifndef __cl_tape_impl_adjointref_hpp__
#define __cl_tape_impl_adjointref_hpp__

#include <cl/tape/impl/adjointrefoperator.hpp>

///     Adaptation segment
/// It is use for convertible esences providing
/// in general we can have many frameworks for using
namespace cl
{
#if defined CL_TAPE_CPPAD
    template <typename Base>
    using TapeInnerType = CppAD::AD<Base>;
#elif CL_TAPE_ADOLC
	template <typename Base>
	using TapeInnerType = Adolc::DoubleAdapter<Base>;
#else
    template <typename Base>
    struct TapeInnerType {    };
#endif

    template <typename Type>
    using ref_type = std::reference_wrapper<Type>;

    typedef cl::TapeDouble t_double;

    template <typename Base
        = typename cl::remove_ad<cl::TapeDouble::value_type >::type >
    struct TapeRef;

    template <typename Type>
    inline Type& deref(ref_type<Type> v)
    {
        return v.get();
    }

    /// Declaration deref for adjoint ref class
    template <typename Type>
    inline typename TapeRef<Type>::inner_type&
    deref(ref_type<TapeRef<Type> > v);

    template <typename Type>
    inline typename TapeRef<Type>::inner_type&
    deref(ref_type<TapeRef<Type> const> v);

    ///  This template is adapter from cl::TapeDouble functionality to adjoint functionality
    /// The Base class can be deferent
    template <typename Base>
    struct TapeRef
    {
        typedef TapeInnerType<Base> inner_type;
        typedef inner_type* inner_type_ptr;

        /// Default constructor
        TapeRef() : ptr_(0)
        {}

        /// This is created from adjoint type CppAD::AD, adolc and others
        TapeRef(inner_type& ref) : ptr_(&ref)
        {}

        /// copy constructor is default
        TapeRef(TapeRef const&) = default;

        /// operator to conversion from adjoint type
        TapeRef<Base>& operator = (TapeRef<Base>& v)
        {
            ptr_ = &v;
            return *this;
        }

        inner_type operator -()
        {
            assert(ptr_);
            return ptr_->operator -();
        }

        /// operator to conversion from adjoint type
        TapeRef<Base>& operator = (inner_type& v)
        {
            ptr_ = &v;
            return *this;
        }

        /// operator from conversation
        TapeRef<Base>& operator = (cl::TapeDouble const& tv)
        {
            if (!ptr_)
            {
                ptr_ = new inner_type(tv.operator inner_type());
            }
            else {
                *ptr_ = tv.value();
            }

            return *this;
        }

        template <typename Right>
        TapeRef<Base>& operator += (Right const& val)
        {
            ref() += deref(std::ref(val));
            return *this;
        }

        template <typename Right>
        TapeRef<Base>& operator -= (Right const& val)
        {
            ref() -= deref(std::ref(val));
            return *this;
        }

        operator t_double () const
        {
            assert(ptr_);
            return t_double(*ptr_);
        }

        inner_type_ptr ptr_;
    private:
        inline inner_type& ref()
        {
            assert(ptr_);
            return *ptr_;
        }
        // TODO: if we'll should notify to push_, resize, set and similar insert event
        // typedef std::tuple<inner_type_ptr, std::vector<inner_type> > agrs_functionality_type;
        // std::function<void (std::pair<inner_type_ptr, )>
    };

    template <typename Type>
    inline typename TapeRef<Type >::inner_type&
    deref(ref_type<TapeRef<Type > > v)
    {
        return *(v.get().ptr_);
    }

    template <typename Type>
    inline typename TapeRef<Type>::inner_type&
    deref(ref_type<TapeRef<Type> const> v)
    {
        return *(v.get().ptr_);
    }

    namespace tapescript
    {
        template <typename Vector = AdjVectorBase>
        struct TapeIterator : std::pair<typename Vector::iterator
            , typename std::vector<TapeRef<> >::iterator >
            , std::random_access_iterator_tag
        {
            typedef std::pair<typename Vector::iterator
            , typename std::vector<TapeRef<> >::iterator > base;

            typedef std::random_access_iterator_tag iterator_category;

            TapeIterator() : base()
            {   }

            template <typename First, typename Second>
            TapeIterator(First const& f, Second const& s) : base(f, s)
            {   }

            template <typename First, typename Second>
            TapeIterator(std::pair<First, Second > const& p) : base(p)
            {   }

            inline TapeIterator<Vector>& operator ++()
            {
                ++first; ++second; return *this;
            }

            inline TapeIterator<Vector>
                operator ++(int)
            {
                return std::make_pair(first++; second++);
            }

            inline TapeIterator<Vector> operator + (std::size_t size)
            {
                return std::make_pair(first + size, second + size);
            }

            inline TapeIterator<Vector> operator - (std::size_t size)
            {
                return std::make_pair(first - size, second - size);
            }
        };
    }
}

namespace std
{
    template <typename Vector> struct _Is_iterator<cl::tapescript::TapeIterator<Vector> >
        : std::true_type{};
}

namespace cl
{
#if defined DEBUG

#   define CL_CHECK_ELEMENTS \
        assert(refs_.size() == vec_.size()); \
        assert(check_equals_elements_());

#else
#   define CL_CHECK_ELEMENTS assert(refs_.size() == vec_.size());
#endif
    namespace tapescript
    {
        class TapeRefVector
        {
            friend inline void Independent(TapeRefVector& v);
            friend inline void Independent(TapeRefVector& v, std::size_t abort_index);
            template <typename Base>
            friend class TapeFunction;

            typedef std::vector<cl::TapeDouble::value_type> TapeDoubleValueVector;
        public:
            typedef TapeIterator<> iterator;
            typedef TapeIterator<> const_iterator;
            typedef std::size_t size_type;

#if defined DEBUG
            inline bool
                check_equals_elements_() const
            {
                TapeDoubleValueVector::const_iterator begin = vec_.begin();
                bool result = refs_.size() ? false : true;
                std::for_each(refs_.begin(), refs_.end()
                    , [&result, &begin](TapeRef<> const& aa)
                { assert(result = (aa.ptr_ == &(*begin++))); });
                return result;
            }
#endif
            inline TapeIterator<> begin()
            {
                return std::make_pair(this->vec_.begin(), this->refs_.begin());
            }

            inline TapeIterator<> end()
            {
                return std::make_pair(this->vec_.end(), this->refs_.end());
            }

            TapeRefVector(std::size_t s = 0) :refs_(s)
                , vec_(s)
            {
                this->assign_refs_();
            }

            inline TapeRef<>& operator [](std::size_t ix)
            {
                return refs_[ix];
            }
        private:
            template <typename IterRef, typename IterValues>
            inline void assign_refs_(IterRef begin, IterRef end, IterValues start)
            {
                std::for_each(begin, end
                    , [&start](TapeRef<>& aa) {
                    aa = *start++;
                });
            }

            inline void assign_refs_()
            {
                if (refs_.size() != vec_.size())
                    refs_.resize(vec_.size());

                this->assign_refs_(refs_.begin()
                    , refs_.end(), vec_.begin());

                CL_CHECK_ELEMENTS;
            }
        public:
            inline void reserve(std::size_t size)
            {
                vec_.reserve(size);
                refs_.resize(size);
            }

            inline void resize(std::size_t size)
            {
                vec_.resize(size);
                refs_.resize(size);

                this->assign_refs_();
            }

            inline std::size_t size() const
            {
                CL_CHECK_ELEMENTS;

                return vec_.size();
            }

            template <typename Base>
            void push_back(TapeInnerType<Base> const& adj)
            {
                vec_.push_back(adj);
                refs_.push_back(vec_.back());
            }

            template<class Iter>
            typename std::enable_if<std::_Is_iterator<Iter>::value, iterator>::type
                insert(const_iterator _Where, Iter first, Iter last)
            {
                    size_type _Off = _Where.first - this->vec_.begin();
                    vec_.insert(_Where.first, first.first, last.first);
                    this->assign_refs_();

                    CL_CHECK_ELEMENTS;
                    return begin() + _Off;
                }


            std::vector<TapeRef<> > refs_;

        private:
            TapeDoubleValueVector vec_;
        };

        /// The pointer adapter
        template <typename P>
        struct adapt_ptr
        {
            adapt_ptr() : ptr_(0)  {}
            adapt_ptr(P* p) : ptr_(p) {}
            adapt_ptr(adapt_ptr const&) = default;
            inline P* operator -> ()  { return ptr_; }
            inline P const* operator -> () const { return ptr_; }
            P* ptr_;
        };

    //    We should have convertible value type
    template <typename Type>
    struct vector_value
    {
        typedef Type type;
    };

    // calculation this place if vector has value, we can have different
    // value type and converter to it
    template <typename Type>
    struct vector_value<std::vector<Type, std::allocator<Type>>>
    {
        typedef Type type;
    };

    // Adapted type calculation inside AdaptVector
    template <typename Vector
                , typename Value>
    struct adapt_type_convention
    {
        // Perhaps it's std vector and we can get value_type from it
        typedef typename
            Vector::value_type Type;

        //  If requested value not equals of vector::value_type
        // it can happens inside Jacobian
        enum { is_convertible_value = !std::is_same<Type, Value>::value };

        // calculate convertible type
        typedef typename cl::detail::if_c<
            !is_convertible_value
                , typename Type::value_type
                , Value>::type converted_value_type;

        // When we return value for Independent adaptation we shuold return reference
        // otherway we should return converted value
        typedef typename cl::detail::if_c<is_convertible_value
            , converted_value_type&, converted_value_type&>::type ret_value_type;

        /// Convertion when we have requested
        /// value type is not equals vector::value type
        template <typename Type>
        static converted_value_type
        conv_2value__(Type const& v, std::true_type)
        {
            // should be check is it params
            // we can't make it from
#if defined CL_COMPILE_TIME_DEBUG_COMPLEX_
#pragma message ("conv_2value : " __FUNCSIG__)
            return converted_value_type();
#else
            return ext::Value(v);
#endif
        }

        // Get same value when Vector::value_type == Value
        template <typename Type>
        static converted_value_type&
        conv_2value__(Type& v, std::false_type)
        {
            // should be check is it params
            // we can't make it from
            return v;
        }

        template <typename Type, typename Branch>
        static converted_value_type&
        conv_2value(Type& v, Branch)
        {
            return conv_2value__(cl::detail::value(v), Branch());
        }

        ///
        template <typename Type>
        static converted_value_type
        cconv_2value__(Type const& v, std::true_type)
        {
            // should be check is it params
            // we can't make it from
#if defined CL_COMPILE_TIME_DEBUG_COMPLEX
#pragma message ("conv_2value : " __FUNCSIG__)
            return ext::Value(v);
#else
            return ext::Value(v);
#endif
        }

        // Get same value when Vector::value_type == Value
        template <typename Type>
        static converted_value_type const&
        cconv_2value__(Type const& v, std::false_type)
        {
#if defined CL_COMPILE_TIME_DEBUG_COMPLEX
#pragma message ("conv_2value : " __FUNCSIG__)
#endif
            // should be check is it params
            // we can't make it from
            return v;
        }

        //  Interface method to get value from
        // const value
        template <typename Type, typename Branch>
        static converted_value_type const&
            cconv_2value(Type const& v, Branch)
        {
            return cconv_2value__(cl::detail::cvalue(v), Branch());
        }
    };

    //  Adapted type calculation inside AdaptVector
    // we should provide convert from complex<TapeInnerType<Base> > to TapeInnerType<complex<Base>>
    // it can help to configure behaviour of adjoint logic with
    template <typename Value>
    struct adapt_type_convention <std::vector<std::complex<cl::TapeDouble>
                        , std::allocator<std::complex<cl::TapeDouble> > >, Value>
    {
        // Vector type
        typedef std::vector<std::complex<cl::TapeDouble> > Vector;

        // Perhaps it's std vector and we can get value_type from it
        typedef typename
            Vector::value_type Type;

        //  If requested value not equals of vector::value_type
        // it can happens inside Jacobian
        enum { is_convertible_value = !std::is_same<Type, Value>::value };

        // calculate convertible type
        typedef typename cl::detail::if_c<
            !is_convertible_value
                , typename Type::value_type
                , Value>::type converted_value_type;

        // When we return value for Independent adaptation we should return reference
        // otherway we should return converted value
        typedef typename cl::detail::if_c<is_convertible_value
            , converted_value_type&, converted_value_type&>::type ret_value_type;

        /// Convertion when we have requested
        /// value type is not equals vector::value type
        template <typename Type>
        static ret_value_type
        conv_2value(Type& v, std::true_type)
        {
            // should be check is it params
            // we can't make it from
#if defined CL_COMPILE_TIME_DEBUG_COMPLEX_
#pragma message ("conv_2value : " __FUNCSIG__)
            return converted_value_type();
#else
            return v.value_;
#endif
        }

        // Get same value when Vector::value_type == Value
        template <typename Type>
        static ret_value_type
        conv_2value(Type& v, std::false_type)
        {
            // should be check is it params
            // we can't make it from
            return v;
        }

        ///
        template <typename Type>
        static converted_value_type const&
        cconv_2value(Type& v, std::true_type)
        {
            // should be check is it params
            // we can't make it from
#if defined CL_COMPILE_TIME_DEBUG_COMPLEX
#pragma message ("conv_2value : " __FUNCSIG__)
            return ext::Value(v);
#else
            return v.value_;
#endif
        }

        // Get same value when Vector::value_type == Value
        template <typename Type>
        static converted_value_type const&
        cconv_2value(Type const& v, std::false_type)
        {
#if defined CL_COMPILE_TIME_DEBUG_COMPLEX
#pragma message ("conv_2value : " __FUNCSIG__)
#endif
            // should be check is it params
            // we can't make it from
            return v;
        }

    };

        /// <summary>Class which is functionality
        /// about adaptation </summary>
    template <typename Vector
            , typename Value = typename vector_value<Vector>::type >
    struct AdapterVector : adapt_type_convention<typename std::remove_const<Vector>::type, Value>
    {
        // typedef std::vector<Type, std::allocator<Type>> Vector;
        typedef typename
        std::remove_const<Vector>::type orig_vector;

        // using by inside
        template <typename P>
        using adapt_type = adapt_ptr<P>;

        // convention type
        typedef adapt_type_convention<orig_vector, Value> Convention;

        // We should expose the value_type to the adjoint functionality
        typedef typename
            Convention::converted_value_type value_type;

        // get size type
        // in general case we wait std vector this place
        typedef typename
            Vector::size_type size_type;

        ///  Constructor
        /// we should initialize from pointer, but shared_ptr couldn't
        AdapterVector(adapt_type<Vector> vc_ref)
            : ref_(vc_ref), ptr_()
        {   }

        ///  Constructor
        /// we should create instance value will put to ref
        AdapterVector(size_type size) : ptr_(new orig_vector(size))
            , ref_(ptr_.get())
        {   }

        /// Constructor
        /// we should create instance value will put to ref
        AdapterVector() : ptr_(new orig_vector())
            , ref_(ptr_.get())
        {   }

        /// Constructor
        /// we should create instance and copy it from copy value
        AdapterVector(AdapterVector const& vc) : ptr_(new orig_vector(*vc.ref_.ptr_))
            , ref_(ptr_.get())
        {    }

        /// <summary> Const operator [] </summary>
        inline converted_value_type const&
            operator[](size_type ix) const
        {
            typedef std::integral_constant<bool, is_convertible_value> is_Conv;
            return Convention::cconv_2value(ref_->operator [](ix), is_Conv());
        }

    private:
        /// In cpp AD didn't call std::remove_const<>
        /// If it's const
        template <typename If_Need_Compile__>
        inline ret_value_type
            get__(size_type ix, std::false_type)
        {
            typedef std::integral_constant<bool, is_convertible_value> is_Conv;
            return Convention::conv_2value(ptr_->operator[](ix), is_Conv());
        }

        template <typename If_Need_Compile__>
        inline ret_value_type
            get__(size_type ix, std::true_type)
        {
            typedef std::integral_constant<bool, is_convertible_value> is_Conv;
            return Convention::conv_2value(ref_->operator[](ix), is_Conv());
        }
    public:

        /// If it's constant vector the instance should be placed in the shared ptr
        /// not constant operator []
        inline ret_value_type
            operator[](size_type ix)
        {
            return this->get__<struct try__>(ix, std::is_same<orig_vector, Vector>());
        }

        inline typename Vector::size_type
            size() const
        {
            return ref_->size();
        }

        template <typename Size>
        void resize(Size s)
        {
            assert(ptr_);
            ptr_->resize(s);
        }

        inline AdapterVector& operator = (AdapterVector const& v)
        {
            ptr_ = std::shared_ptr<orig_vector>(new orig_vector(*v.ref_.ptr_));
            ref_ = adapt_type<Vector>(ptr_.get());
            return *this;
        }

        std::shared_ptr<orig_vector > ptr_;
        adapt_type<Vector> ref_;
    };

    template <typename Type
        , typename Value = typename vector_value<typename std::remove_const<Type>::type >::type >
        struct Adapter;

    template <typename Type, typename Value>
    struct Adapter<std::vector<Type, std::allocator<Type>> const, Value>
        : AdapterVector<std::vector<Type, std::allocator<Type>> const, Value>
        {
            typedef std::vector<Type, std::allocator<Type>> vector_type;
            Adapter(adapt_type<vector_type const> vc_ref)
                : AdapterVector(vc_ref)
            {}

            Adapter() : AdapterVector()
            {}

            Adapter(Adapter const& v)
                : AdapterVector(static_cast<AdapterVector const&>(v))
            { }

            Adapter(size_type size)
                : AdapterVector(size)
            {}
        };

    template <typename Type, typename Value>
    struct Adapter<std::vector<Type, std::allocator<Type>>, Value >
        : AdapterVector<std::vector<Type, std::allocator<Type>>, Value >
        {
            typedef std::vector<Type, std::allocator<Type>> vector_type;
            Adapter(adapt_type<vector_type> vc_ref)
                : AdapterVector(vc_ref)
            {}

            Adapter() : AdapterVector()
            {}

            Adapter(Adapter const& v)
                : AdapterVector(static_cast<AdapterVector const&>(v))
            {}

            Adapter(size_type size)
                : AdapterVector(size)
            {}
        };

        template <typename Type>
        inline Adapter<Type >
            adapt(Type& v) {
                return Adapter<Type>(adapt_ptr<Type>(&v));
            }

        template <typename Type>
        inline Adapter<Type const>
            adapt(Type const& v) {
                return Adapter<Type const>(adapt_ptr<Type const>(&v));
            }

    template <typename Conv, typename Type>
    inline Adapter<Type, Conv>
    adapt_typed(Type& v) {
        return Adapter<Type, Conv>(adapt_ptr<Type>(&v));
    }

    template <typename Conv, typename Type>
    inline Adapter<Type const, Conv const>
    adapt_typed(Type const& v) {
        return Adapter<Type const, Conv const>(adapt_ptr<Type const>(&v));
    }

    }
    /// Currently we use this approach to
    /// adaptation the extern type vectors to inside TapeInnerType
    typedef std::vector<cl::TapeDouble> TapeDoubleVector;

    template <typename Base>
    class TapeFunction : public TapeFunctionBase<Base>
    {
    public:
        TapeFunction(tapescript::TapeRefVector const& x, tapescript::TapeRefVector const& y)
            : TapeFunctionBase<Base>(x.vec_, y.vec_)
        { }

        TapeFunction(std::vector<cl::TapeDouble> const& x, std::vector<cl::TapeDouble> const& y)
            : TapeFunctionBase<Base>(tapescript::adapt(x), tapescript::adapt(y))
        { }
    };

    inline void
    Independent(std::vector<cl::TapeDouble>& v_tape, std::size_t abort_index)
    {
        ext::Independent(tapescript::adapt(v_tape), abort_index);
    }

    inline void
    Independent(std::vector<cl::TapeDouble>& v_tape)
    {
        ext::Independent(tapescript::adapt(v_tape));
    }

    inline void
    Independent(std::vector<std::complex<cl::TapeDouble>> &x, std::size_t abort_index)
    {
#if defined CL_TAPE_COMPLEX_ENABLED
        ext::Independent(cl::tapescript::adapt_typed<cl::TapeInnerType<std::complex<double> > >(v_tape), abort_index);
#endif
    }

    template <typename Type>
    inline void print_type()
    {
#pragma message (__FUNCSIG__)
    }

    inline void
    Independent(std::vector<std::complex<cl::TapeDouble>> &x)
    {
#if defined CL_COMPILE_TIME_DEBUG
        print_type<decltype(adapt_typed<TapeInnerType<std::complex<double> > >(x)[0])>();
#endif
#if defined CL_TAPE_COMPLEX_ENABLED
        ext::Independent(cl::tapescript::adapt_typed<cl::TapeInnerType<std::complex<double> > >(x));
#endif
    }

}

namespace cl_ext
{
    template <int const_, typename Then, typename Else>
    using if_c = cl::detail::if_c<const_, Then, Else>;

    struct TapeRefOperators;

    // This is can be also declared in external
    // TODO check possibility to change extern namespace to the other
    // as a variant we should use clearly top namespace
    // it can use, but conflicts possibility very hight
    template <typename Base, typename Right>
    struct custom_operator<cl::TapeRef<Base>, Right>
    {
        typedef TapeRefOperators type;
    };

    template <typename Left, typename Base>
    struct custom_operator<Left, cl::TapeRef<Base> >
    {
        typedef TapeRefOperators type;
    };

    template <typename Base>
    struct custom_operator<cl::TapeRef<Base>, cl::TapeRef<Base> >
    {
        typedef TapeRefOperators type;
    };

    template <typename Base, typename Right>
    struct custom_operator<std::vector<Base, std::allocator<Base> >, Right >
    {
        typedef TapeRefOperators type;
    };

    template <typename Return, typename Oper, typename Constr>
    struct operator_ref_ {};

    ADJOINT_REF_OPERATOR_IMPL(oper_minus, -);
    ADJOINT_REF_OPERATOR_IMPL(oper_plus, +);
    ADJOINT_REF_OPERATOR_IMPL(oper_mult, *);
    ADJOINT_REF_OPERATOR_IMPL(oper_div, / );
    ADJOINT_REF_OPERATOR_IMPL(struct oper_plus_eq, += );
    ADJOINT_REF_OPERATOR_IMPL(struct oper_minus_eq, -= );

    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_minus);
    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_plus);
    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_mult);
    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_div);
    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_plus_eq);

#if defined EXT_UNARY_OPERATORS
    template <typename Left, typename Right>
    inline typename operator_traits<Left, Right, struct oper_plus_eq>::type
        & operator += (Left& l, Right const& right)
    {
        typedef operator_traits<Left, Right, oper_plus_eq> o;
        o()(l, right);
        return deref(std::ref(l));
    }

    template <typename Left, typename Right>
    inline typename operator_traits<Left, Right, oper_plus_eq>::type
        & operator -= (Left& l, Right right)
    {
        typedef operator_traits<Left, Right, oper_plus_eq> o;
        o()(l, right);
        return deref(std::ref(l));
    }
#endif


    template <typename Base>
    inline std::ostream& operator << (std::ostream &o, cl::TapeRef<Base> const& adj)
    {
        return (o << *adj.ptr_);
    }
}

#endif // __cl_tape_impl_adjointref_hpp__