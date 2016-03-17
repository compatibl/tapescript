/*
Copyright (C) 2015-present CompatibL

Performance test results and finance-specific examples are available at:

http://www.tapescript.org

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

#ifndef cl_tape_impl_double_hpp
#define cl_tape_impl_double_hpp

# include <cl/tape/declare.hpp>
# include <cl/tape/impl/detail/oper_def.hpp>
# include <cl/tape/impl/detail/conversion/converter.hpp>
# include <cl/tape/impl/std_fwd.hpp>
# include <cl/tape/impl/tape_fwd.hpp>

# include <cl/tape/impl/detail/double_type_value.hpp>
# include <cl/tape/impl/detail/traits.hpp>
# include <cl/tape/impl/detail/double/differentiable.hpp>

namespace cl
{
    /// <summary>Immutable double type with AD support designed
    /// to serve as a drop-in replacement to native double.</summary>
    template <typename Inner>
    struct tape_wrapper
                : std::conditional<cl::is_implemented<struct compatibl_ad_enabled>::value
                    , differentiable<tape_wrapper<Inner>>
                    , not_differentiable<tape_wrapper<Inner>> >::type
    {
        typedef Inner base_type;
        typedef base_type_traits<base_type> traits;
        typedef tape_wrapper<Inner> tape_type;
        typedef CppAD::AD<Inner> base;

        typedef typename
            std::conditional<
                cl::is_implemented<struct compatibl_ad_enabled>::value
                , differentiable<tape_wrapper<Inner>>
                , not_differentiable<tape_wrapper<Inner>>
            >::type differentiable_type;

        /*template <class >
        friend struct cl::class_members;

        template <class >
        friend struct cl::class_fields;*/

        /// <summary>AD implementation type, typedef according to this library's naming conventions.</summary>
        typedef typename
            base_type_traits<Inner>::value_type Impltype;

        /// <summary>AD implementation type, alternative typedef required by STL.</summary>
        typedef typename
            base_type_traits<Inner>::value_type value_type;

        typedef value_type& value_ref;

        // double_value type is calculated if we can use union for Inner
        typedef typename
            base_type_traits<base_type>
        ::double_value_type<Inner, value_type*>::type double_value_type;

        // Ad type
        typedef typename
            base_type_traits<base_type>
        ::double_value_type<Inner, value_type*>::tdouble_type tdouble_type;

        // If we can't use in union double is equals tdouble_type
        typedef typename
            base_type_traits<base_type>
        ::double_value_type<Inner, value_type*>::double_type double_type;

    private:

        /// <summary>Friend class to conversions.</summary>
        template <typename, typename, typename, typename, typename, typename, typename, typename >
        friend struct cl::tape_double_convert;

        // Friend functions
        // in which we should get double directly
        template <typename Inner_>
        friend cl::tape_wrapper<Inner_> std::ceil(cl::tape_wrapper<Inner_> const& x, void*);

        template <typename Inner_>
        friend cl::tape_wrapper<Inner_> std::floor(cl::tape_wrapper<Inner_> const& x, void*);

    public:

        /// <summary>Copy ctor.</summary>
        tape_wrapper(tape_wrapper<Inner> const& v);

        /// <summary>Move constructor.</summary>
        tape_wrapper(tape_wrapper<Inner>&& ov);

        /// <summary>Assignment from other type.</summary>
        tape_wrapper<Inner>& operator = (tape_wrapper<Inner>&& ov);

        /// <summary>Explicit conversion operator from an arbitrary type.</summary>
        template <typename Type>
        explicit tape_wrapper(Type const& value);

        /// <summary>Assignment from other type.</summary>
        template <typename Type>
        inline tape_type& operator=(Type const& rhs);

        /// <summary>Assignment from other type.</summary>
        inline tape_wrapper<Inner>& operator=(tape_wrapper<Inner> const& rhs);

        /// <summary>Assign through reference wrapper.</summary>
        template <typename Type>
        explicit tape_wrapper(ref_wrap<Type> const& v);

    private: // PRIVATE

        /// <summary>Return cast value if it's arithmetic.</summary>
        template <typename Type>
        inline Type get__(std::true_type, std::false_type) const;

        template <typename Type>
        inline Type get__(std::true_type, std::true_type) const;

        /// <summary>Return cast value. Try to create instance type.</summary>
        template <typename Type>
        inline Type get__(std::false_type, std::false_type) const;

        template <typename Type>
        inline Type get__(std::false_type, std::true_type) const;

    public:

        //!! Should we select one option and stay with it?
#   if defined CL_EXPLICIT_NATIVE_CONVERSION

        // This should skip message
        template <typename Type>
        inline void to(std::false_type) const;

        // This should show message
        template <typename Type>
        inline void to(std::true_type) const;

#   if defined CL_TAPE_CPPAD
        inline operator value_type() const;
#   endif

        // Try to convert explicitly if this is arithmetic
        template <typename Type>
        inline explicit operator Type() const;

#   else
        inline explicit operator int() const;
        inline explicit operator value_type() const;
        inline explicit operator unsigned int() const;
        inline explicit operator unsigned long() const;
        inline explicit operator long() const;
#   endif

    public:
        // CONSTRUCTORS
        inline tape_wrapper();

        /// <summary>Implicit constructor from double.</summary>
        inline tape_wrapper(value_type const& value);

        //!!! Should this include other CL_TAPE_* options?
#   if defined CL_TAPE_CPPAD
        inline tape_wrapper(Inner const& value);
#   endif

    public: // OPERATORS

        /// <summary>Negation operator.</summary>
        inline bool operator!() const;

        /// <summary>Assignment of native double.</summary>
        inline tape_type& operator=(double rhs);

        /// <summary>Returns a copy if self.</summary>
        inline tape_type operator+() const;

        /// <summary>Returns the negative of self.</summary>
        inline tape_type operator-() const;

        /// <summary>Prefix incrementation.</summary>
        inline tape_type& operator++();

        /// <summary>Postfix incrementation.</summary>
        inline tape_type operator++(int);

        /// <summary>Prefix decrementation.</summary>
        inline tape_type& operator--();

        /// <summary>Postfix decrementation.</summary>
        inline tape_type operator--(int);

    private:

        template <typename Type>
        static value_type
        Value__(Type const& obj, std::true_type);

        template <typename Type>
        static value_type
        Value__(Type const& obj, std::false_type);

    public:

        typedef double_value_traits<tape_wrapper<Inner>
            , cl::is_implemented<compatibl_ad_enabled>::value > double_traits_type;

        template <class Type>
        inline void alloc_(Type const& value, std::false_type);

        template <class Type>
        inline void alloc_(Type const& value, std::true_type);

        template <class Type>
        inline void alloc_(Type const& value);

        inline bool is_ptr() const;

        inline bool is_pointer() const;

        inline tdouble_type&
        tdouble_(std::true_type);

        inline tdouble_type&
        tdouble_(std::false_type);

        inline tdouble_type&
        tdouble_();

        inline tdouble_type const&
        tdouble_(std::true_type) const;

        inline tdouble_type const&
        tdouble_(std::false_type) const;

        inline tdouble_type const&
        tdouble_() const;

        inline double_type& double_(std::false_type);

        inline double_type& double_(std::true_type);

        inline double_type& double_();

        inline double_type const&
        double_() const;

    private:

        inline static bool
        check__(bool enabled_diff
            , bool set_differ_on = false, bool value = true);

    public:

        template <typename Type>
        inline static bool
        differentiation_check();

#       ifdef CL_TAPE_CAN_GET_VALUE
        //!!  This can break AD if it is misused; or stop recording if it is invoked
        inline tdouble_type const&
        value() const;

        template <typename TapeType>
        friend inline typename TapeType::tdouble_type const&
        cl::tapescript::cvalue(TapeType const& tapetype);

        template <typename TapeType>
        friend inline typename TapeType::tdouble_type&
        cl::tapescript::value(TapeType& tapetype);
#       endif

        // Value field
        double_value_type value_;
    };

    template <typename Base>
    inline void
    tape_wrapper_clear(cl::tape_wrapper<Base>* this_ptr, std::false_type)
    {}

    template <typename Base>
    inline void
    tape_wrapper_clear(cl::tape_wrapper<Base>* this_ptr, std::true_type)
    {
        typedef typename
            cl::tape_wrapper<Base>::double_traits_type double_traits_type;

        if (this_ptr->is_ptr()
            && double_traits_type::is_union_able)
        {
            double_traits_type
                ::free(this_ptr->value_);

            // Here for case when ~ called twice
            this_ptr->value_ = cl::tape_wrapper<Base>::double_value_type();
        }
    }

    template <typename tape_adapter>
    differentiable<tape_adapter>::~differentiable()
    {
        tape_wrapper_clear(static_cast<tape_adapter*>(this)
            , cl::is_implemented<cl::compatibl_ad_enabled>());
    }

    template <typename Inner>
    inline bool
    tape_wrapper<Inner>::check__(bool enabled_diff
        , bool set_differ_on = false, bool value = true)
    {
        static bool is_differ_on = false;

        if (set_differ_on)
            is_differ_on = value;

        if (is_differ_on)
        {
            CL_ASSERT(enabled_diff
                , "Should be identical types.");

            if (!enabled_diff)
                cl::throw_("Incompatible AD using.");
        }

        return !is_differ_on || enabled_diff;
    }

    template <typename Inner>
    template <typename Type>
    inline bool
    tape_wrapper<Inner>::differentiation_check()
    {
        return check__(
            differentiable_type::enabled);
    }


    OPBIN(cl::tape_wrapper<Base>, +);
    OPBIN(cl::tape_wrapper<Base>, -);
    OPBIN(cl::tape_wrapper<Base>, *);

    //OPBIN(-);
    OPBIN(bool, ==);
    OPBIN(bool, !=);
    OPBIN(bool, >=);
    OPBIN(bool, <=);
    OPBIN(bool, >);
    OPBIN(bool, <);

    ASSIGMENT_OPER(+= );
    ASSIGMENT_OPER(-= );
    ASSIGMENT_OPER(*= );
    ASSIGMENT_OPER(/= );
    ASSIGMENT_OPER(%= );

    template <typename D, typename Base>
    inline D
    alloc_value_(tape_wrapper<Base> const& v, std::true_type)
    {
        if (cl::is_implemented<cl::compatibl_ad_enabled>::value
                && v.is_ptr())
        {
            return D::alloc_(v.tdouble_());
        }

        return D(v.double_());
    }

    template <typename D, typename Base>
    inline D
    alloc_value_(tape_wrapper<Base> const& v, std::false_type)
    {
        return D(v.double_());
    }

    template <typename D
        , typename Base, template <typename > class Back>
    inline D
    alloc_value_(typename Back<Base> const& v, std::true_type)
    {
        return D::alloc_(v);
    }

    template <typename D
        , typename Base, template <typename > class Back>
    inline D
    alloc_value_(typename Back<Base> const& v, std::false_type)
    {
        return D(v);
    }

    template <typename Inner>
    tape_wrapper<Inner>::tape_wrapper()
        : value_(0.0)
    {
        CL_CHECK_ENABLED_DIFF(Inner);
    }

    template <typename Inner>
    tape_wrapper<Inner>::tape_wrapper(tape_wrapper<Inner> const& v)
        : value_(alloc_value_<
                        tape_wrapper<Inner>::double_value_type
                    >(v, std::integral_constant<bool
                    , tape_wrapper<Inner>::double_traits_type::is_union_able
                      && cl::is_implemented<cl::compatibl_ad_enabled>::value>())
               )
    {
        CL_CHECK_ENABLED_DIFF(Inner);
    }

    /// <summary>Implicit constructor from double.</summary>
    template <typename Inner>
    tape_wrapper<Inner>::tape_wrapper(value_type const& v)
        : value_(alloc_value_<
                        tape_wrapper<Inner>::double_value_type
                    >(v, std::integral_constant<bool
                    , tape_wrapper<Inner>::double_traits_type::is_union_able
                        && cl::is_implemented<cl::compatibl_ad_enabled>::value>()))
    {
        CL_CHECK_ENABLED_DIFF(Inner);
    }

    //!!! Should this include other CL_TAPE_* options?
#   if defined CL_TAPE_CPPAD
    template <typename Inner>
    inline tape_wrapper<Inner>::tape_wrapper(Inner const& value) : value_(value)
    {
        CL_CHECK_ENABLED_DIFF(Inner);
    }
#   endif

    template <typename Inner>
    tape_wrapper<Inner>::tape_wrapper(tape_wrapper&& ov)
        : value_(ov.value_)
    {
        CL_CHECK_ENABLED_DIFF(Inner);

        ov.value_ = tape_wrapper<Inner>::double_value_type();
    }

    template <typename Inner>
    inline tape_wrapper<Inner>&
    tape_wrapper<Inner>::operator =(cl::tape_wrapper<Inner>&& ov)
    {
        CL_CHECK_ENABLED_DIFF(Inner);

        value_ = ov.value_;
        ov.value_ = tape_wrapper<Inner>::double_value_type();
        return *this;
    }

    template <typename Inner>
    template <typename Type>
    tape_wrapper<Inner>::tape_wrapper(Type const& value)
        : value_()
    {
        CL_CHECK_ENABLED_DIFF(Inner);

        cl::tape_double_convert<Type, value_type>::convert(*this, value);
    }

    template <typename Inner>
    template <typename Type>
    inline void tape_wrapper<Inner>::alloc_(Type const& value, std::false_type)
    {
        value_ = value;
    }

    template <typename Inner>
    template <typename Type>
    inline void tape_wrapper<Inner>::alloc_(Type const& value, std::true_type)
    {
        value_.bits_ = double_value_type::alloc_(value);
    }

    template <typename Inner>
    template <typename Type>
    inline void tape_wrapper<Inner>::alloc_(Type const& value)
    {
        typedef std::integral_constant<bool
            , double_traits_type::is_union_able> value_using;

        this->alloc_(value, value_using());
    }

    template <typename Inner>
    inline bool tape_wrapper<Inner>::is_ptr() const
    {
        return double_traits_type::is_ptr(*this);
    }

    template <typename Inner>
    inline bool tape_wrapper<Inner>::is_pointer() const
    {
        return is_ptr();
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::tdouble_type&
    tape_wrapper<Inner>::tdouble_(std::true_type)
    {
        CL_ASSERT(this->is_ptr(), "The tape should be initialized.");

        return *reinterpret_cast<value_type *>(value_.bits_ & ~double_value_type::ptr_mask64);
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::tdouble_type&
    tape_wrapper<Inner>::tdouble_(std::false_type)
    {
        return value_;
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::tdouble_type&
    tape_wrapper<Inner>::tdouble_()
    {
        typedef std::integral_constant<bool
            , double_traits_type::is_union_able> value_using;

        return double_traits_type::tdouble_<tdouble_type>(value_);
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::tdouble_type const&
    tape_wrapper<Inner>::tdouble_(std::true_type) const
    {
        CL_ASSERT(this->is_ptr(), "The tape should be initialized.");

        return *reinterpret_cast<value_type *>(value_.bits_ & ~double_value_type::ptr_mask64);
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::tdouble_type const&
    tape_wrapper<Inner>::tdouble_(std::false_type) const
    {
        return value_;
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::tdouble_type const&
    tape_wrapper<Inner>::tdouble_() const
    {
        typedef std::integral_constant<bool
            , double_traits_type::is_union_able> value_using;

        return double_traits_type::tdouble_<tdouble_type const>(value_);
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::double_type&
    tape_wrapper<Inner>::double_(std::false_type)
    {
        return value_;
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::double_type&
    tape_wrapper<Inner>::double_(std::true_type)
    {
        CL_ASSERT(!this->is_ptr() || !std::is_arithmetic<base_type>::value
            , "The tape should be initialized.");

        return value_.dbl_;
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::double_type&
    tape_wrapper<Inner>::double_()
    {
        typedef std::integral_constant<bool
            , double_traits_type::is_union_able> value_using;

        return double_traits_type::double_<double_type>(value_);
    }

    template <typename Inner>
    inline typename tape_wrapper<Inner>::double_type const&
    tape_wrapper<Inner>::double_() const
    {
        CL_ASSERT(!this->is_ptr() || !std::is_arithmetic<base_type>::value
            , "The tape should be initialized.");

        return double_traits_type::double_<double_type const>(value_);
    }

    template <typename Inner>
    inline tape_wrapper<Inner>& tape_wrapper<Inner>::operator=
        (tape_wrapper<Inner> const& v)
    {
        CL_CHECK_ENABLED_DIFF(Inner);

        char case_ = (char)this->is_ptr() << 1 | (char)v.is_ptr();
        switch (case_)
        {
            case 0:
                this->value_ = v.value_;
                break;
            case 1:
                this->value_ =
                    alloc_value_<
                        tape_wrapper<Inner>::double_value_type
                    >(v, std::integral_constant<bool
                 , tape_wrapper<Inner>::double_traits_type::is_union_able>());
                break;
            case 2:
                this->tdouble_() = v.double_();
                break;
            case 3:
                this->tdouble_() = v.tdouble_();
                break;
            default:
                break;
        }

        return *this;
    }

    /// <summary>Assignment from other type.</summary>
    template <typename Inner>
    template <typename Type>
    inline typename tape_wrapper<Inner>::tape_type&
    tape_wrapper<Inner>::operator=(Type const& rhs)
    {
        CL_CHECK_ENABLED_DIFF(Inner);

        *this = tape_wrapper<Inner>(rhs);
        return *this;
    }

    /// <summary>Assign through reference wrapper.</summary>
    template <typename Inner>
    template <typename Type>
    tape_wrapper<Inner>::tape_wrapper(ref_wrap<Type> const& v)
        : value_(traits<>::get())
    {
        CL_CHECK_ENABLED_DIFF(Inner);
    }

//!!  This can break AD if it is misused; or stop recording if it is invoked
#   ifdef CL_TAPE_CAN_GET_VALUE
    template <typename Inner>
    inline typename tape_wrapper<Inner>::tdouble_type const&
    tape_wrapper<Inner>::value() const
    {
        return this->tdouble_();
    }
#   endif

    /// <summary>Return cast value if it's arithmetic.</summary>
    template <typename Inner>
    template <typename Type>
    inline Type tape_wrapper<Inner>::get__(std::true_type, std::false_type) const
    {
#       if defined CL_COMPILE_TIME_DEBUG
#          pragma message ("get__ std::true_type, std::false_type " __FUNCSIG__)
#       endif

        if (!this->is_ptr())
            return static_cast<Type>(ext::Value(this->double_()));

        return static_cast<Type>(ext::Value(this->tdouble_()));
    }

    template <typename Inner>
    template <typename Type>
    inline Type tape_wrapper<Inner>::get__(std::true_type, std::true_type) const
    {
#       if defined CL_COMPILE_TIME_DEBUG
#          pragma message ("get__ std::true_type, std::true_type " __FUNCSIG__)
#       endif

        return static_cast<Type>(this->value());
    }

    /// <summary>Return cast value. Try to create instance type.</summary>
    template <typename Inner>
    template <typename Type>
    inline Type tape_wrapper<Inner>::get__(std::false_type, std::false_type) const
    {
#       if defined CL_COMPILE_TIME_DEBUG
#          pragma message ("get__ std::false_type, std::false_type " __FUNCSIG__)
#       endif
                // Would cause a recursive call
#       if defined _MSC_VER
                static_assert(false, "Conversion to this type isn't implemented");
#       elif __GNUC__
                cl::throw_("Conversion to this type isn't implemented");
#       endif

        return Type();
    }

    template <typename Inner>
    template <typename Type>
    inline Type tape_wrapper<Inner>::get__(std::false_type, std::true_type) const
    {
#       if defined CL_COMPILE_TIME_DEBUG
#          pragma message ("get__ std::false_type, std::true_type " __FUNCSIG__)
#       endif

        return Type(*value_.tdbl_);
    }

#   if defined CL_EXPLICIT_NATIVE_CONVERSION

    // This should skip message
    template <typename Inner>
    template <typename Type>
    inline void tape_wrapper<Inner>::to(std::false_type) const {   }

    // This should show message
    template <typename Inner>
    template <typename Type>
    inline void tape_wrapper<Inner>::to(std::true_type) const
    {
#       pragma message ("Derivatives may be incorrect if this method is invoked: " __FUNCSIG__)
    }

#   if defined CL_TAPE_CPPAD
    template <typename Inner>
    inline tape_wrapper<Inner>::operator value_type() const
    {
        return *value_.tdbl_;
    }
#   endif

    // Try to convert explicitly if this is arithmetic
    template <typename Inner>
    template <typename Type>
    inline tape_wrapper<Inner>::operator Type() const
    {
#       if defined CL_COMPILE_TIME_DEBUG
#          pragma message ("convert compilation for: " __FUNCSIG__)
#       endif

        typedef typename std::remove_volatile<typename std::remove_const<Type>::type >::type native_type;

        enum {
            is_sm = std::is_same<native_type, value_type >::value
                , is_constructible_from_value = std::is_constructible <native_type, value_type>::value
                , is_arithmetic_or_contructible =
                    std::is_arithmetic<Type>::value
                    || std::is_constructible <native_type, typename cl::remove_ad<value_type>::type >::value
                , is_remove_ad = std::is_same<native_type, typename cl::remove_ad<value_type>::type>::value
        };

        typedef std::integral_constant<bool, is_sm || is_constructible_from_value > is_value_type;
        typedef std::integral_constant<bool, is_arithmetic_or_contructible > is_arithm_or_contrictible;

        // We should show message if we can create
        // class from double type but not from QlDouble
        to<Type>(std::integral_constant<bool, is_arithmetic_or_contructible
            && !std::is_arithmetic<Type>::value
            && !is_remove_ad>());

        return this->get__<Type>(is_arithm_or_contrictible(), is_value_type());
    }
#   else
    //  We use ocnversion not only for arithmetic types,
    // if type can be contruct from double we should use direct conversion
    template <typename Inner>
    inline tape_wrapper<Inner>::operator int() const { return (int)value_; }
    template <typename Inner>
    inline tape_wrapper<Inner>::operator value_type() const { return value_; }
    template <typename Inner>
    inline tape_wrapper<Inner>::operator unsigned int() const { return (unsigned int)value_; }
    template <typename Inner>
    inline tape_wrapper<Inner>::operator unsigned long() const { return (unsigned long)value_; }
    template <typename Inner>
    inline tape_wrapper<Inner>::operator long() const { return (long)value_; }
#   endif


    /// <summary>Returns a copy if self.</summary>
    template <typename Inner>
    inline typename tape_wrapper<Inner>::tape_type
    tape_wrapper<Inner>::operator+() const { return tape_type(*this); }

    /// <summary>Returns the negative of self.</summary>
    template <typename Inner>
    inline typename tape_wrapper<Inner>::tape_type
    tape_wrapper<Inner>::operator-() const
    {
        return this->is_ptr() ? tape_type(-this->tdouble_()) : tape_type(-this->double_());
    }

    /// <summary>Prefix incrementation.</summary>
    template <typename Inner>
    inline typename tape_wrapper<Inner>::tape_type&
    tape_wrapper<Inner>::operator++() { this->value_.dbl_ += 1.0; return *this; }

    /// <summary>Postfix incrementation.</summary>
    template <typename Inner>
    inline typename tape_wrapper<Inner>::tape_type
    tape_wrapper<Inner>::operator++(int)
    {
        tape_type result(*this);
        ++(*this);

        return result;
    }

    /// <summary>Prefix decrementation.</summary>
    template <typename Inner>
    inline typename tape_wrapper<Inner>::tape_type&
    tape_wrapper<Inner>::operator--()
    {
        *this -= 1.0;
        return *this;
    }

    /// <summary>Postfix decrementation.</summary>
    template <typename Inner>
    inline typename tape_wrapper<Inner>::tape_type tape_wrapper<Inner>::operator--(int)
    {
        tape_type result(*this);
        --(*this);

        return result;
    }

    template <typename Inner>
    template <typename Type>
    typename tape_wrapper<Inner>::value_type
    tape_wrapper<Inner>::Value__(Type const& obj, std::true_type) { return obj; }

    template <typename Inner>
    template <typename Type>
    typename tape_wrapper<Inner>::value_type
    tape_wrapper<Inner>::Value__(Type const& obj, std::false_type) { return static_cast<value_type>(obj); }

    /// <summary>Negation operator.</summary>
    template <typename Inner>
    inline bool
    tape_wrapper<Inner>::operator!() const
    { return *this == 0.0; } //!! Provide special treatment for AD

    /// <summary>Assignment of native double.</summary>
    template <typename Inner>
    inline typename tape_wrapper<Inner>::tape_type&
    tape_wrapper<Inner>::operator=(double value)
    {
        if (this->is_ptr())
        {
            this->tdouble_() = value;
        }
        else
            this->double_() = value;

        return *this;
    }

}

#endif  // cl_tape_impl_double_hpp

