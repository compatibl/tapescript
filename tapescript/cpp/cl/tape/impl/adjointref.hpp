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

#ifndef cl_tape_impl_adjointref_hpp
#define cl_tape_impl_adjointref_hpp

#include <cl/tape/impl/adjointrefoperator.hpp>

/// <summary>adapter for types convertible to double.</summary>
namespace cl
{
    namespace tapescript
    {
        /// <summary>Need for referenced values</summary>
        template <typename Base
            = typename cl::remove_ad<cl::tape_double::value_type >::type >
        struct tape_ref;
    }

    /// <summary>Reference extractor.</summary>
    template <typename Type>
    inline Type& deref(ref_type<Type> v)
    {
        return v.get();
    }

    /// <summary>Declaration of dereference for adjoint reference class.</summary>
    template <typename Type>
    inline typename tapescript::tape_ref<Type>::inner_type&
    deref(ref_type<tapescript::tape_ref<Type> > v);

    /// <summary>Declaration of dereference for adjoint reference const class.</summary>
    template <typename Type>
    inline typename tapescript::tape_ref<Type>::inner_type&
    deref(ref_type<tapescript::tape_ref<Type> const> v);

    namespace tapescript
    {
        /// This template is an adapter of cl::tape_double functionality to adjoint functionality
        /// The Base class can be different
        template <typename Base>
        struct tape_ref
        {
            typedef tape_inner_type<Base> inner_type;
            typedef inner_type* inner_type_ptr;

            /// Default constructor
            tape_ref() : ptr_(0)
            {}

            /// This is created from adjoint type CppAD::AD, adolc and others
            tape_ref(inner_type& ref) : ptr_(&ref)
            {}

            /// copy constructor is default
            tape_ref(tape_ref const&) = default;

            /// conversion operator from adjoint type
            tape_ref<Base>& operator = (tape_ref<Base>& v)
            {
                ptr_ = &v;
                return *this;
            }

            inner_type operator -()
            {
                assert(ptr_);
                return ptr_->operator -();
            }

            /// conversion operator from adjoint type
            tape_ref<Base>& operator = (inner_type& v)
            {
                ptr_ = &v;
                return *this;
            }

            /// conversion operator
            tape_ref<Base>& operator = (cl::tape_double const& tv)
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
            tape_ref<Base>& operator += (Right const& val)
            {
                ref() += deref(std::ref(val));
                return *this;
            }

            template <typename Right>
            tape_ref<Base>& operator -= (Right const& val)
            {
                ref() -= deref(std::ref(val));
                return *this;
            }

            operator tape_ref<Base>() const
            {
                assert(ptr_);
                return tape_ref<Base>(*ptr_);
            }

            inner_type_ptr ptr_;
        private:
            inline inner_type& ref()
            {
                assert(ptr_);
                return *ptr_;
            }
        };

        /// <summary>Iterator over tape double accessors,
        /// used for the algorithmic adjoint.</summary>
        template <typename Vector = std::vector<cl::tape_double> >
        struct tape_iterator : std::pair<typename Vector::iterator
            , typename std::vector<tape_ref<> >::iterator >
            , std::random_access_iterator_tag
        {
            typedef std::pair<typename Vector::iterator
            , typename std::vector<tape_ref<> >::iterator > base;

            typedef std::random_access_iterator_tag iterator_category;

            tape_iterator() : base()
            {   }

            template <typename First, typename Second>
            tape_iterator(First const& f, Second const& s) : base(f, s)
            {   }

            template <typename First, typename Second>
            tape_iterator(std::pair<First, Second > const& p) : base(p)
            {   }

            inline tape_iterator<Vector>& operator ++()
            {
                ++first; ++second; return *this;
            }

            inline tape_iterator<Vector>
                operator ++(int)
            {
                return std::make_pair(first++; second++);
            }

            inline tape_iterator<Vector> operator + (std::size_t size)
            {
                return std::make_pair(first + size, second + size);
            }

            inline tape_iterator<Vector> operator - (std::size_t size)
            {
                return std::make_pair(first - size, second - size);
            }
        };
    }

    /// <summary>Dereference implementation.</summary>
    template <typename Type>
    inline typename tapescript::tape_ref<Type >::inner_type&
    deref(ref_type<tapescript::tape_ref<Type > > v)
    {
        return *(v.get().ptr_);
    }

    /// <summary>Dereference implementation.</summary>
    template <typename Type>
    inline typename tapescript::tape_ref<Type>::inner_type&
    deref(ref_type<tapescript::tape_ref<Type> const> v)
    {
        return *(v.get().ptr_);
    }

}

namespace std
{
    /// <summary>Implementation of std traits for algorithmic use.</summary>
    template <typename Vector> struct _Is_iterator<cl::tapescript::tape_iterator<Vector> >
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
        class tape_ref_vector
        {
            friend inline void Independent(tape_ref_vector& v);
            friend inline void Independent(tape_ref_vector& v, std::size_t abort_index);
            template <typename Base>
            friend class tape_function;

            typedef std::vector<cl::tape_double::value_type> tape_double_value_vector;
        public:
            typedef tape_iterator<> iterator;
            typedef tape_iterator<> const_iterator;
            typedef std::size_t size_type;

#if defined DEBUG
            inline bool
                check_equals_elements_() const
            {
                tape_double_value_vector::const_iterator begin = vec_.begin();
                bool result = refs_.size() ? false : true;
                std::for_each(refs_.begin(), refs_.end()
                    , [&result, &begin](tape_ref<> const& aa)
                { assert(result = (aa.ptr_ == &(*begin++))); });
                return result;
            }
#endif
            inline tape_iterator<> begin()
            {
#if defined CL_REF_ITERATOR_ENABLE
                return std::make_pair(this->vec_.begin(), this->refs_.begin());
#else
                return tape_iterator<>();
#endif
            }

            inline tape_iterator<> end()
            {
#if defined CL_REF_ITERATOR_ENABLE
                return std::make_pair(this->vec_.end(), this->refs_.end());
#else
                return tape_iterator<>();
#endif

            }

            tape_ref_vector(std::size_t s = 0) :refs_(s)
                , vec_(s)
            {
                this->assign_refs_();
            }

            inline tape_ref<>& operator [](std::size_t ix)
            {
                return refs_[ix];
            }
        private:
            template <typename IterRef, typename IterValues>
            inline void assign_refs_(IterRef begin, IterRef end, IterValues start)
            {
                std::for_each(begin, end
                    , [&start](tape_ref<>& aa) {
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
            void push_back(tape_inner_type<Base> const& adj)
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


            std::vector<tapescript::tape_ref<> > refs_;

        private:
            tape_double_value_vector vec_;
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

        // We should have convertible value type
        template <typename Type>
        struct vector_value
        {
            typedef Type type;
        };

        // If a vector has a value_type, we can convert to it
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

            // If requested value not equals to vector::value_type
            // it can happen inside Jacobian
            enum { is_convertible_value = !std::is_same<Type, Value>::value };

            // find convertible type
            typedef typename cl::tapescript::if_c<
                !is_convertible_value
                    , typename Type::value_type
                    , Value>::type converted_value_type;

            // If we return a value for Independent adaptation we should return a reference
            // otherwise we should return a converted value
            typedef typename cl::tapescript::if_c<is_convertible_value
                , converted_value_type&, converted_value_type&>::type ret_value_type;

            /// Convertion if the requested value type
            /// not equals to vector::value_type
            template <typename Type>
            static converted_value_type
            conv_2value__(Type const& v, std::true_type)
            {
                // should be checked whether it is param
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
                // should be checked whether it is param
                return v;
            }

            template <typename Type, typename Branch>
            static converted_value_type&
            conv_2value(Type& v, Branch)
            {
                return conv_2value__(cl::tapescript::value(v), Branch());
            }

            template <typename Type>
            static converted_value_type
            cconv_2value__(Type const& v, std::true_type)
            {
                // should be checked whether it is param
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
                // should be checked whether it is param
                return v;
            }

            // Interface method for retrieving value from
            // const value
            template <typename Type, typename Branch>
            static converted_value_type const&
                cconv_2value(Type const& v, Branch)
            {
                return cconv_2value__(cl::tapescript::cvalue(v), Branch());
            }
        };

#if defined CL_TAPE_COMPLEX_ENABLED
        // Adapted type convertion inside AdaptVector
        // we should provide convertion from complex<tape_inner_type<Base> > to tape_inner_type<complex<Base>>
        // it can help to configure behaviour of adjoint logic
        template <typename Inner, typename Value>
        struct adapt_type_convention <std::vector<std::complex<cl::tape_wrapper<Inner>>
                , std::allocator<std::complex<cl::tape_wrapper<Inner>>>>, Value>
        {
            // Type of tape holder
            typedef cl::tape_wrapper<Inner> tape_type;

            // Vector type
            typedef std::vector<std::complex<tape_type> > Vector;

            // Perhaps it's std vector and we can get value_type from it
            typedef typename
                Vector::value_type Type;

            // If a requested value not equals to vector::value_type
            // it can happen inside Jacobian
            enum { is_convertible_value = !std::is_same<Type, Value>::value };

            // find convertible type
            typedef typename cl::tapescript::if_c<
                !is_convertible_value
                    , typename Type::complex_based_type
                    , Value>::type converted_value_type;

            // If we return a value for Independent adaptation we should return a reference
            // otherwise we should return a converted value
            typedef typename cl::tapescript::if_c<is_convertible_value
                , converted_value_type&, converted_value_type&>::type ret_value_type;

            /// Convertion if the requested value_type
            /// not equals to vector::value_type
            template <typename Type>
            static ret_value_type
            conv_2value(Type& v, std::true_type)
            {
                // should be checked whether it is param
    #if defined CL_COMPILE_TIME_DEBUG_COMPLEX_
    #pragma message ("conv_2value : " __FUNCSIG__)
                return converted_value_type();
    #else
                return v.complex_base();
    #endif
            }

            // Get same value when Vector::value_type == Value
            template <typename Type>
            static ret_value_type
            conv_2value(Type& v, std::false_type)
            {
                // should be checked whether it is param
                return v;
            }

            ///
            template <typename Type>
            static converted_value_type const&
            cconv_2value(Type& v, std::true_type)
            {
                // should be checked whether it is param
    #if defined CL_COMPILE_TIME_DEBUG_COMPLEX
    #pragma message ("conv_2value : " __FUNCSIG__)
                return ext::Value(v);
    #else
                return v.complex_base();
    #endif
            }

            // Get same value if Vector::value_type == Value
            template <typename Type>
            static converted_value_type const&
            cconv_2value(Type const& v, std::false_type)
            {
    #if defined CL_COMPILE_TIME_DEBUG_COMPLEX
    #pragma message ("conv_2value : " __FUNCSIG__)
    #endif
                // should be checked whether it is param
                return v;
            }

        };
#endif

            /// <summary>Class which provides
            /// adaptation </summary>
        template <typename Vector
                , typename Value = typename vector_value<Vector>::type >
        struct adapter_vector : adapt_type_convention<typename std::remove_const<Vector>::type, Value>
        {
            // typedef std::vector<Type, std::allocator<Type>> Vector;
            typedef typename
            std::remove_const<Vector>::type orig_vector;

            // for internal use
            template <typename P>
            using adapt_type = adapt_ptr<P>;

            // convention type
            typedef adapt_type_convention<orig_vector, Value> Convention;

            // We should provide the adjoint functionality for a value_type
            typedef typename
                Convention::converted_value_type value_type;

            // get size_type
            // in general case std vector is expected
            typedef typename
                Vector::size_type size_type;

            /// Constructor
            /// Pointer can initialize adapter_vector, but shared_ptr couldn't
            adapter_vector(adapt_type<Vector> vc_ref)
                : ref_(vc_ref), ptr_()
            {   }

            /// Constructor
            /// we should create the instance and value will be putted to ref
            adapter_vector(size_type size) : ptr_(new orig_vector(size))
                , ref_(ptr_.get())
            {   }

            /// Constructor
            /// we should create the instance and value will be putted to ref
            adapter_vector() : ptr_(new orig_vector())
                , ref_(ptr_.get())
            {   }

            /// Constructor
            /// we should create the instance and value will be putted to ref
            adapter_vector(adapter_vector const& vc) : ptr_(new orig_vector(*vc.ref_.ptr_))
                , ref_(ptr_.get())
            {    }

            /// <summary>Const operator []</summary>
            inline converted_value_type const&
                operator[](size_type ix) const
            {
                typedef std::integral_constant<bool, is_convertible_value> is_Conv;
                return Convention::cconv_2value(ref_->operator [](ix), is_Conv());
            }

        private:
            /// In CppAD std::remove_const<> didn't used
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

            inline adapter_vector& operator = (adapter_vector const& v)
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
        struct adapter;

        template <typename Type, typename Value>
        struct adapter<std::vector<Type, std::allocator<Type>> const, Value>
            : adapter_vector<std::vector<Type, std::allocator<Type>> const, Value>
        {
            typedef std::vector<Type, std::allocator<Type>> vector_type;
            adapter(adapt_type<vector_type const> vc_ref)
                : adapter_vector(vc_ref)
            {}

            adapter() : adapter_vector()
            {}

            adapter(adapter const& v)
                : adapter_vector(static_cast<adapter_vector const&>(v))
            { }

            adapter(size_type size)
                : adapter_vector(size)
            {}
        };

        template <typename Type, typename Value>
        struct adapter<std::vector<Type, std::allocator<Type>>, Value >
            : adapter_vector<std::vector<Type, std::allocator<Type>>, Value >
        {
            typedef std::vector<Type, std::allocator<Type>> vector_type;
            adapter(adapt_type<vector_type> vc_ref)
                : adapter_vector(vc_ref)
            {}

            adapter() : adapter_vector()
            {}

            adapter(adapter const& v)
                : adapter_vector(static_cast<adapter_vector const&>(v))
            {}

            adapter(size_type size)
                : adapter_vector(size)
            {}
        };

        template <typename Type>
        inline adapter<Type >
        adapt(Type& v) {
            return adapter<Type>(adapt_ptr<Type>(&v));
        }

        template <typename Type>
        inline adapter<Type const>
        adapt(Type const& v) {
            return adapter<Type const>(adapt_ptr<Type const>(&v));
        }

        template <typename Conv, typename Type>
        inline adapter<Type, Conv>
        adapt_typed(Type& v) {
            return adapter<Type, Conv>(adapt_ptr<Type>(&v));
        }

        template <typename Conv, typename Type>
        inline adapter<Type const, Conv const>
        adapt_typed(Type const& v) {
            return adapter<Type const, Conv const>(adapt_ptr<Type const>(&v));
        }
    }

    /// <summary>Currently we use this approach for adaptation of
    /// the extern type vectors to inner tape_inner_type.</summary>
    typedef std::vector<cl::tape_double> tape_double_vector;

    /// <summary>Tape function is a compatible external functional implementation
    /// this should be suitable inside external framework.</summary>
    template <typename Base>
    class tape_function
        : public tape_function_base<Base>
        , tapescript::serialize_accessor<Base>
    {
    public:

        typedef tapescript::serialize_accessor<Base> serializability;
        typedef tape_function_base<Base> base;

        tape_function()
            : tape_function_base<Base>()
            , serializability()
        { }

        template <typename Serializer>
        tape_function(Serializer& serializer)
            : tape_function_base<Base>()
            , serializability()
        {
            serializer & *this;
        }

        template <typename Inner, typename Serializer>
        tape_function(std::vector<cl::tape_wrapper<Inner>> const& x
                , std::vector<cl::tape_wrapper<Inner>> const& y
                , Serializer& serializer)
                        : tape_function_base<Base>(tapescript::adapt(x), tapescript::adapt(y))
                        , serializability(tapescript::adapt(x))
        {
            serializer & *this;
        }

        tape_function(tapescript::tape_ref_vector const& x, tapescript::tape_ref_vector const& y)
            : tape_function_base<Base>(x.vec_, y.vec_)
            , serializability(x.vec_)
        { }

        template <typename Inner>
        tape_function(std::vector<cl::tape_wrapper<Inner>> const& x, std::vector<cl::tape_wrapper<Inner>> const& y)
            : tape_function_base<Base>(tapescript::adapt(x), tapescript::adapt(y))
            , serializability(tapescript::adapt(x))
        { }

        template<typename Vector, typename Serializer>
        inline Vector
        reverse(size_t q, Vector const& v, Serializer& s)
        {
            return this->Reverse(q, std::make_pair(v, &s)).first;
        }

        template<typename Vector>
        inline Vector
        reverse(size_t q, Vector const& v)
        {
            return this->Reverse(q, v);
        }

        /// assign a new operation sequence
        template <typename ADvector>
        void dependent(const ADvector &x, const ADvector &y)
        {
            this->Dependent(x,y);
        }

        /// assign a new operation sequence
        template <typename ADvector>
        void tape_read(const ADvector &x, const ADvector &y)
        {
            this->Dependent(x, y);
        }


        /// forward mode user API, one order multiple directions.
        template <typename VectorBase>
        inline VectorBase forward(size_t q, size_t r, const VectorBase& x)
        {
            return this->Forward(q,r,x);
        }

        /// forward mode user API, multiple directions one order.
        template <typename VectorBase>
        inline VectorBase forward(size_t q,
            const VectorBase& x, std::ostream& s = std::cout)
        {
            return this->Forward(q,x,s);
        }

        /// Dependent function forward to the adjoint library
        template <typename Inner>
        void Dependent(std::vector<cl::tape_wrapper<Inner>> const& x, std::vector<cl::tape_wrapper<Inner>> const& y)
        {
            tape_function_base<Base>::Dependent(tapescript::adapt(x), tapescript::adapt(y));
        }
    };

    template <typename Inner>
    class tape_function<std::complex<cl::tape_wrapper<Inner> > >
        : public cl::tape_function_base<std::complex<Inner> >
    {
    public:
        typedef cl::tape_function_base<std::complex<Inner> > fun_base;
        template <typename VectorType>
        tape_function(VectorType& x, VectorType& y)
            : fun_base(
                cl::tapescript::adapt_typed<cl::tape_inner_type<std::complex<Inner> > >(x)
                , cl::tapescript::adapt_typed<cl::tape_inner_type<std::complex<Inner> > >(y))
        {   }
    };

    template <class Inner>
    inline void
    Independent(std::vector<cl::tape_wrapper<Inner>>& v_tape, std::size_t abort_index)
    {
        ext::Independent(tapescript::adapt(v_tape), abort_index);
    }

    template <class Inner>
    inline void
    Independent(std::vector<cl::tape_wrapper<Inner>>& v_tape)
    {
        ext::Independent(tapescript::adapt(v_tape));
    }

    inline void
    Independent(std::vector<std::complex<cl::tape_double>> &x, std::size_t abort_index)
    {
#if defined CL_TAPE_COMPLEX_ENABLED
        ext::Independent(cl::tapescript::adapt_typed<cl::tape_inner_type<std::complex<double> > >(x), abort_index);
#endif
    }


    template <class Inner>
    inline void
    tape_start(std::vector<cl::tape_wrapper<Inner>>& v_tape, std::size_t abort_index)
    {
        ext::Independent(tapescript::adapt(v_tape), abort_index);
    }

    template <class Inner>
    inline void
    tape_start(std::vector<cl::tape_wrapper<Inner>>& v_tape)
    {
        ext::Independent(tapescript::adapt(v_tape));
    }

    template <typename Type>
    inline void print_type()
    {
#pragma message (__FUNCSIG__)
    }

    inline void
    Independent(std::vector<std::complex<cl::tape_double>> &x)
    {
#if defined CL_COMPILE_TIME_DEBUG
        print_type<decltype(cl::tapescript::adapt_typed<tape_inner_type<std::complex<double> > >(x)[0])>();
#endif
#if defined CL_TAPE_COMPLEX_ENABLED
        ext::Independent(cl::tapescript::adapt_typed<cl::tape_inner_type<std::complex<double> > >(x));
#endif
    }

}

namespace cl_ext
{
    template <int const_, typename Then, typename Else>
    using if_c = cl::tapescript::if_c<const_, Then, Else>;

    struct tape_ref_operators;

    template <typename Left, typename Right>
    struct custom_operator;

    // This can be also declared in external code
    // TODO check possibility to change extern namespace to another one
    // as a variant we should use clear top namespace
    // Changing namespace may cause conflict
    template <typename Base, typename Right>
    struct custom_operator<cl::tapescript::tape_ref<Base>, Right>
    {
        typedef tape_ref_operators type;
    };

    template <typename Left, typename Base>
    struct custom_operator<Left, cl::tapescript::tape_ref<Base> >
    {
        typedef tape_ref_operators type;
    };

    template <typename Base>
    struct custom_operator<cl::tapescript::tape_ref<Base>, cl::tapescript::tape_ref<Base> >
    {
        typedef tape_ref_operators type;
    };

    template <typename Base, typename Right>
    struct custom_operator<std::vector<Base, std::allocator<Base> >, Right >
    {
        typedef tape_ref_operators type;
    };

    template <typename Return, typename Oper, typename Constr>
    struct operator_ref_ {};

    ADJOINT_REF_OPERATOR_IMPL(oper_minus, -);
    ADJOINT_REF_OPERATOR_IMPL(oper_plus, +);
    ADJOINT_REF_OPERATOR_IMPL(oper_mult, *);
    ADJOINT_REF_OPERATOR_IMPL(oper_div, / );
    ADJOINT_REF_OPERATOR_IMPL(oper_plus_eq, += );
    ADJOINT_REF_OPERATOR_IMPL(oper_minus_eq, -= );

    /*OPERATOR_TRAITS_ADJOINTREF_DECL(oper_minus);
    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_plus);
    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_mult);
    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_div);
    OPERATOR_TRAITS_ADJOINTREF_DECL(oper_plus_eq);*/

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
    inline std::ostream& operator << (std::ostream &o, cl::tapescript::tape_ref<Base> const& adj)
    {
        return (o << *adj.ptr_);
    }
}

#endif // cl_tape_impl_adjointref_hpp