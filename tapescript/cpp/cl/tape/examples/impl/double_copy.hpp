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

#ifndef cl_tape_impl_double_copy_hpp
#define cl_tape_impl_double_copy_hpp

// This file is a copy of double.hpp with value_type double

#include <cl/tape/declare.hpp>
#include <cl/tape/impl/tape_fwd.hpp>
#include <cl/tape/impl/doubleconverter.hpp>

namespace cl
{
    /// <summary>Immutable double type with AD support designed
    /// to serve as a drop-in replacement to native double.</summary>
    template <typename Inner>
    class tape_wrapper
    {
    public:
        typedef Inner base_type;
        typedef tape_wrapper<Inner> tape_type;

        typedef double ImplType;

        /// <summary>AD implementation type, alternative typedef required by STL.</summary>
        typedef ImplType value_type;

    private:

        /// <summary>The friend class to conversations.</summary>
        template <typename, typename, typename, typename, typename, typename, typename, typename >
        friend struct cl::tape_double_convert;

        //!! This can break AD if it is misused; or stop recording if it is invoked
#ifdef CL_TAPE_CAN_GET_VALUE
        template <typename TapeType>
        friend inline typename TapeType::value_type const&  cl::tapescript::cvalue(TapeType const& tapetype);

        template <typename TapeType>
        friend inline typename TapeType::value_type& cl::tapescript::value(TapeType& tapetype);
#endif
        
    public: // TYPEDEFS

        /// <summary>Explicit conversion operator from an arbitrary type.</summary>
        template <typename Type>
        explicit tape_type(Type const& rhs)
            : value_()
        {
            cl::tape_double_convert<Type, value_type>::convert(*this, rhs);
        }

        /// <summary>Assignment from other type.</summary>
        template <typename Type>
        inline tape_type& operator=(Type const& rhs)
        {
            cl::tape_double_convert<Type, value_type>::convert(*this, rhs);
            return *this;
        }

        //!!  This can break AD if it is misused; or stop recording if it is invoked
#ifdef CL_TAPE_CAN_GET_VALUE
        inline value_type value() const
        {
            return value_;
        }
#endif

    private: // PRIVATE

        /// <summary>Return cast value if it's arithmetic.</summary>
        template <typename Type>
        inline Type get__(std::true_type, std::false_type) const
        {
#if defined CL_COMPILE_TIME_DEBUG
#   pragma message ("get__ std::true_type, std::false_type " __FUNCSIG__)
#endif
            return static_cast<Type>(ext::Value(this->value_));
        }

        template <typename Type>
        inline Type get__(std::true_type, std::true_type) const
        {
#if defined CL_COMPILE_TIME_DEBUG
#   pragma message ("get__ std::true_type, std::true_type " __FUNCSIG__)
#endif
            return static_cast<Type>(this->value_);
        }

        /// <summary>Return cast value. Try to create instance type.</summary>
        template <typename Type>
        inline Type get__(std::false_type, std::false_type) const
        {
#if defined CL_COMPILE_TIME_DEBUG
#   pragma message ("get__ std::false_type, std::false_type " __FUNCSIG__)
#endif
            // Would cause a recursive call
            static_assert(false, "Conversion to this type isn't implemented");
            return Type();
        }

        template <typename Type>
        inline Type get__(std::false_type, std::true_type) const
        {
#if defined CL_COMPILE_TIME_DEBUG
#   pragma message ("get__ std::false_type, std::true_type " __FUNCSIG__)
#endif
            return Type(value_);
        }
    public:

        //!! Should we select one option and stay with it?
#ifndef CL_EXPLICIT_NATIVE_CONVERSION

        // This should skip message
        template <typename Type>
        inline void to(std::false_type) const {   }

        // This should show message
        template <typename Type>
        inline void to(std::true_type) const
        {
            //#pragma message ("Derivatives may be incorrect if this method is invoked: " __FUNCSIG__)
        }

#if defined CL_TAPE_CPPAD
        inline operator value_type() const
        {
            return value_;
        }
#endif

        // Try to convert explicitly if this is arithmetic
        template <typename Type>
        inline explicit operator Type() const
        {
#if defined CL_COMPILE_TIME_DEBUG
#   pragma message ("convert compilation for: " __FUNCSIG__)
#endif
            typedef typename std::remove_volatile<typename std::remove_const<Type>::type >::type native_type;

            enum {
                is_sm = std::is_same<native_type, value_type >::value
                , is_constructible_from_value = std::is_constructible <native_type, value_type>::value
                , is_arithmetic_or_contructible =
                std::is_arithmetic<Type>::value
                || std::is_constructible <native_type, typename cl::remove_ad<value_type>::type >::value
            };

            typedef std::integral_constant<bool, is_sm || is_constructible_from_value > is_value_type;
            typedef std::integral_constant<bool, is_arithmetic_or_contructible > is_arithm_or_contrictible;

            // We should show message if we can create
            // class from double type but not from QlDouble
            to<Type>(std::integral_constant<bool, is_arithmetic_or_contructible
                && !std::is_arithmetic<Type>::value >());

            return this->get__<Type>(is_arithm_or_contrictible(), is_value_type());
        }
#else
        inline explicit operator int() const { return (int)value_; }
        inline explicit operator value_type() const { return value_; }
        inline explicit operator unsigned int() const { return (unsigned int)value_; }
        inline explicit operator unsigned long() const { return (unsigned long)value_; }
        inline explicit operator long() const { return (long)value_; }
#endif

    private: // FIELDS

        value_type value_;

    public: // CONSTRUCTORS

        inline tape_type() : value_() {}

        /// <summary>Implicit constructor from double.</summary>
        inline tape_type(value_type rhs) : value_(rhs) {}

        //!!! Should this include other CL_TAPE_* options?
#if defined CL_TAPE_CPPAD
        //inline tape_type(const Inner& rhs) : value_(rhs) {}
#endif

    public: // OPERATORS
        
        /// <summary>Negation operator.</summary>
        inline bool operator!() const { return value_ == 0.0; } //!! Provide special treatment for AD

        /// <summary>Assignment of native double.</summary>
        inline tape_type& operator=(double rhs) { value_ = rhs; return *this; }

        /// <summary>Adds rhs to self.</summary>
        inline tape_type& operator+=(const tape_type& rhs) { value_ += rhs.value_; return *this; }

        /// <summary>Adds rhs to self.</summary>
        inline tape_type& operator+=(double rhs) { value_ += rhs; return *this; }

        /// <summary>Subtracts rhs from self.</summary>
        inline tape_type& operator-=(const tape_type& rhs) { value_ -= rhs.value_; return *this; }

        /// <summary>Subtracts rhs from self.</summary>
        inline tape_type& operator-=(double rhs) { value_ -= rhs; return *this; }

        /// <summary>Multiplies self by rhs.</summary>
        inline tape_type& operator*=(const tape_type& rhs) { value_ *= rhs.value_; return *this; }

        /// <summary>Multiplies self by rhs.</summary>
        inline tape_type& operator*=(double rhs) { value_ *= rhs; return *this; }

        /// <summary>Divides self by rhs.</summary>
        inline tape_type& operator/=(const tape_type& rhs) { value_ /= rhs.value_; return *this; }

        /// <summary>Divides self by rhs.</summary>
        inline tape_type& operator/=(double rhs) { value_ /= rhs; return *this; }

        /// <summary>Returns a copy if self.</summary>
        inline tape_type operator+() const { return tape_type(value_); }

        /// <summary>Returns the negative of self.</summary>
        inline tape_type operator-() const { return tape_type(-value_); }

        /// <summary>Returns true if self is not equal to rhs.</summary>
        inline bool operator!=(const tape_type& rhs) const { return value_ != rhs.value_; }

        /// <summary>Returns true if self is not equal to rhs.</summary>
        inline bool operator!=(double rhs) const { return value_ != rhs; }

        /// <summary>Returns true if self is less than or equal to rhs.</summary>
        inline bool operator<=(const tape_type& rhs) const { return value_ <= rhs.value_; }

        /// <summary>Returns true if self is less than or equal to rhs.</summary>
        inline bool operator<=(double rhs) const { return value_ <= rhs; }

        /// <summary>Returns true if self is more than rhs.</summary>
        inline bool operator>(const tape_type& rhs) const { return value_ > rhs.value_; }

        /// <summary>Returns true if self is more than rhs.</summary>
        inline bool operator>(double rhs) const { return value_ > rhs; }

        /// <summary>Returns true if self is more than or equal to rhs.</summary>
        inline bool operator>=(const tape_type& rhs) const { return value_ >= rhs.value_; }

        /// <summary>Returns true if self is more than or equal to rhs.</summary>
        inline bool operator>=(double rhs) const { return value_ >= rhs; }

        /// <summary>Prefix incrementation.</summary>
        inline tape_type& operator++() { value_ += 1.0; return *this; }

        /// <summary>Postfix incrementation.</summary>
        inline tape_type operator++(int) { tape_type result(value_); ++(*this);  return result; }

        /// <summary>Prefix decrementation.</summary>
        inline tape_type& operator--() { value_ -= 1.0; return *this; }

        /// <summary>Postfix decrementation.</summary>
        inline tape_type operator--(int) { tape_type result(value_); --(*this);  return result; }
        
    private:

        template <typename Type>
        inline static ImplType
            Value(Type const& obj, std::true_type) { return obj.value_; }

        template <typename Type>
        inline static ImplType
            Value(Type const& obj, std::false_type) { return static_cast<ImplType>(obj); }
    };
}

#endif  // cl_tape_impl_double_copy_hpp
