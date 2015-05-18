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

#ifndef __cl_tape_impl_double_hpp__
#define __cl_tape_impl_double_hpp__

#include <cl/tape/declare.hpp>
#include <cl/tape/impl/doubleconverter.hpp>

namespace cl
{
    /// <summary>Immutable double type with AD support designed
    /// to serve as a drop-in replacement to native double.</summary>
    class TapeDouble
    {
    public:

        /// <summary>AD implementation type, typedef according to this library's naming conventions.</summary>
#ifdef CL_TAPE_CPPAD
        typedef CppAD::AD<double> ImplType;
#elif defined CL_TAPE_ADOLC
        typedef adouble ImplType;
#else
        typedef double ImplType;
#endif

        /// <summary>AD implementation type, alternative typedef required by STL.</summary>
        typedef ImplType value_type;

    private:

        /// <summary> The friend class to conversations </summary>
        template <typename, typename, typename, typename, typename, typename, typename, typename >
        friend struct cl::TapeDoubleConvert;

//!! Should we have this, can break AD when misused; or it could stop recording when invoked
#ifdef CL_TAPE_CAN_GET_VALUE
        template <typename TapeType>
        friend inline typename TapeType::value_type const&  cl::detail::cvalue(TapeType const& tapetype);

        template <typename TapeType>
        friend inline typename TapeType::value_type& cl::detail::value(TapeType& tapetype);
#endif

    public: // TYPEDEFS

        /// <summary>Explicit conversion operator from an arbitrary type.</summary>
        template <typename Type>
        explicit TapeDouble(Type const& rhs)
            : value_()
        {
            cl::TapeDoubleConvert<Type, value_type>::convert(*this, rhs);
        }

        /// <summary>Assignment from other type.</summary>
        template <typename Type>
        inline TapeDouble& operator=(Type const& rhs)
        {
            cl::TapeDoubleConvert<Type, value_type>::convert(*this, rhs);
            return *this;
        }

//!! Should we have this, can break AD when misused; or it could stop recording when invoked
#ifdef CL_TAPE_CAN_GET_VALUE
        inline value_type value() const
        {
            return value_;
        }
#endif

    private: // PRIVATE

        /// <summary>Return cast value is it's arithmetic.</summary>
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

        /// <summary>Return cast value. Try to create instance Typed.</summary>
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
#pragma message ("Derivatives may be incorrect if this method is invoked: " __FUNCSIG__)
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

            //  We should show message currenlty in cases when we can create
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

        inline TapeDouble() : value_() {}

        /// <summary>Implicit constructor from double.</summary>
        inline TapeDouble(value_type rhs) : value_(rhs) {}

//!!! Should this include other CL_TAPE_* options?
#if defined CL_TAPE_CPPAD
        inline TapeDouble(double rhs) : value_(rhs) {}
#endif

    public: // OPERATORS

        /// <summary>Negation operator.</summary>
        inline bool operator!() const { return value_ == 0.0; } //!! Provide special treatment for AD

        /// <summary>Assignment of native double.</summary>
        inline TapeDouble& operator=(double rhs) { value_ = rhs; return *this; }

        /// <summary>Adds rhs to self.</summary>
        inline TapeDouble& operator+=(const TapeDouble& rhs) { value_ += rhs.value_; return *this; }

        /// <summary>Adds rhs to self.</summary>
        inline TapeDouble& operator+=(double rhs) { value_ += rhs; return *this; }

        /// <summary>Subtracts rhs from self.</summary>
        inline TapeDouble& operator-=(const TapeDouble& rhs) { value_ -= rhs.value_; return *this; }

        /// <summary>Subtracts rhs from self.</summary>
        inline TapeDouble& operator-=(double rhs) { value_ -= rhs; return *this; }

        /// <summary>Multiplies self by rhs.</summary>
        inline TapeDouble& operator*=(const TapeDouble& rhs) { value_ *= rhs.value_; return *this; }

        /// <summary>Multiplies self by rhs.</summary>
        inline TapeDouble& operator*=(double rhs) { value_ *= rhs; return *this; }

        /// <summary>Divides self by rhs.</summary>
        inline TapeDouble& operator/=(const TapeDouble& rhs) { value_ /= rhs.value_; return *this; }

        /// <summary>Divides self by rhs.</summary>
        inline TapeDouble& operator/=(double rhs) { value_ /= rhs; return *this; }

        /// <summary>Returns a copy if self.</summary>
        inline TapeDouble operator+() const { return TapeDouble(value_); }

        /// <summary>Returns the negative of self.</summary>
        inline TapeDouble operator-() const { return TapeDouble(-value_); }

        /// <summary>Returns true if self is equal to rhs.</summary>
        inline bool operator==(const TapeDouble& rhs) const { return value_ == rhs.value_; }

        /// <summary>Returns true if self is equal to rhs.</summary>
        inline bool operator==(double rhs) const { return value_ == rhs; }

        /// <summary>Returns true if self is not equal to rhs.</summary>
        inline bool operator!=(const TapeDouble& rhs) const { return value_ != rhs.value_; }

        /// <summary>Returns true if self is not equal to rhs.</summary>
        inline bool operator!=(double rhs) const { return value_ != rhs; }

        /// <summary>Returns true if self is less than rhs.</summary>
        inline bool operator<(const TapeDouble& rhs) const { return value_ < rhs.value_; }

        /// <summary>Returns true if self is less than rhs.</summary>
        inline bool operator<(double rhs) const { return value_ < rhs; }

        /// <summary>Returns true if self is less than or equal to rhs.</summary>
        inline bool operator<=(const TapeDouble& rhs) const { return value_ <= rhs.value_; }

        /// <summary>Returns true if self is less than or equal to rhs.</summary>
        inline bool operator<=(double rhs) const { return value_ <= rhs; }

        /// <summary>Returns true if self is more than rhs.</summary>
        inline bool operator>(const TapeDouble& rhs) const { return value_ > rhs.value_; }

        /// <summary>Returns true if self is more than rhs.</summary>
        inline bool operator>(double rhs) const { return value_ > rhs; }

        /// <summary>Returns true if self is more than or equal to rhs.</summary>
        inline bool operator>=(const TapeDouble& rhs) const { return value_ >= rhs.value_; }

        /// <summary>Returns true if self is more than or equal to rhs.</summary>
        inline bool operator>=(double rhs) const { return value_ >= rhs; }

        /// <summary>Prefix incrementation.</summary>
        inline TapeDouble& operator++() {   value_ += 1.0; return *this; }

        /// <summary>Postfix incrementation.</summary>
        inline TapeDouble operator++(int) { TapeDouble result(value_); ++(*this);  return result; }

        /// <summary>Prefix decrementation.</summary>
        inline TapeDouble& operator--() { value_ -= 1.0; return *this; }

        /// <summary>Postfix decrementation.</summary>
        inline TapeDouble operator--(int) { TapeDouble result(value_); --(*this);  return result; }

    private:

        template <typename Type>
        inline static ImplType
        Value(Type const& obj, std::true_type) { return obj.value_; }

        template <typename Type>
        inline static ImplType
        Value(Type const& obj, std::false_type) { return static_cast<ImplType>(obj); }
    };
}

#endif  // __cl_tape_impl_double_hpp__
