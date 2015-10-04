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

#ifndef cl_tape_impl_doubleconverter_hpp
#define cl_tape_impl_doubleconverter_hpp

#include <cl/tape/impl/doubleoperatorcheck.hpp>

namespace cl
{
    namespace tapescript
    {
        // Returns constant ref value from TapeType
        template <typename TapeType>
        inline typename TapeType::value_type const& cvalue(TapeType const& tv);

        // Returns ref value from TapeType
        template <typename TapeType>
        inline typename TapeType::value_type& value(TapeType& tv);
    }

    template <typename... Comparison>
    struct Conversion;

    /// <summary>This template and its specializations provide conversion to tape_double value_type.</summary>
    template <typename Type
            , typename ValueType
            , typename NativeType = typename std::remove_volatile<typename std::remove_const<Type>::type >::type
            , typename IsSameType = typename std::is_same<NativeType, tape_double>::type
            , typename IsArithmetic = typename std::is_arithmetic<NativeType>::type
            , typename IsConvertible = typename std::is_convertible<Type, ValueType>::type
            , typename IsHasOperator = typename cl::tapescript::is_has_operator_real<NativeType>::type
            , typename IsEnum = typename std::is_enum<NativeType>::type  >
    struct tape_double_convert
    {
        typedef ValueType type;
    };

    /// <summary>This template and its specializations provide conversion to tape_double value_type.</summary>
    template <typename Type, typename ValueType, typename NativeType
            , typename IsSame //[ std::true_type] typename IsArithm, [std::true_type] typename IsConv
            , typename IsHasOper //, [std::false_type] typename IsEnum
        >
    struct tape_double_convert<Type, ValueType, NativeType
        , IsSame, std::true_type, std::true_type, IsHasOper, std::false_type>
    {
        typedef ValueType type;

        template <typename ThisType>
        static void convert(ThisType& this_value, Type const& other_value)
        {
            this_value.value_
                    = static_cast<typename cl::remove_ad<ValueType>::type >(other_value);
        }
    };

    /// <summary>This template and its specializations provide conversion to tape_double value_type.</summary>
    template <typename Type, typename ValueType, typename NativeType
        , typename IsSame // [std::false_type] typename IsArithm, [std::true_type] typename IsConv
        , typename IsHasOper // [std::false_type] typename IsEnum
    >
    struct tape_double_convert<Type, ValueType, NativeType
        , IsSame, std::false_type, std::true_type, IsHasOper, std::false_type>
    {
        typedef ValueType type;

        template <typename ThisType>
        static void convert(ThisType& this_value, Type const& other_value)
        {
            typedef typename cl::remove_ad<ValueType>::type value_type;
            this_value.value_ = other_value;
        }
    };

    /// <summary>This template and its specializations provide conversion to tape_double value_type.</summary>
    template <typename Type, typename ValueType, typename NativeType
                , typename IsArithm, typename IsConv, typename IsHasOper, typename IsEnum>
    struct tape_double_convert<Type, ValueType, NativeType
        , std::true_type, IsArithm, IsConv, IsHasOper, IsEnum>
    {
        typedef ValueType type;

        template <typename ThisType>
        static void convert(ThisType& this_value, Type const& other_value)
        {
            this_value.value_ = other_value.value_;
        }
    };

    /// <summary>This template and its specializations provide conversion to tape_double value_type.</summary>
    template <typename Type, typename ValueType, typename NativeType
        , typename IsSame, /*typename IsConv,*/ typename IsHasOper, typename IsEnum>
    struct tape_double_convert<Type, ValueType, NativeType
        , IsSame, std::true_type, std::false_type, IsHasOper, IsEnum>
    {
        typedef ValueType type;

        // If ValueType is a class, constructor should be called
        template <typename ThisType>
        static void convert(ThisType& this_value, Type const& other_value)
        {
            this_value.value_ = static_cast<typename cl::remove_ad<ValueType>::type >(other_value);
        }
    };

    /// <summary>This template and its specializations provide conversion to tape_double value_type.</summary>
    template <typename Type, typename ValueType, typename NativeType
        , typename IsSame, typename IsConv, typename IsArithm, typename IsEnum>
    struct tape_double_convert<Type, ValueType, NativeType
        , IsSame, IsArithm, IsConv, std::true_type, IsEnum>
    {
        typedef ValueType type;

        // If ValueType is a class, constructor should be called
        template <typename ThisType>
        static void convert(ThisType& this_value, Type const& other_value)
        {
            // If we have const reference, non-constant operator cannot be used
            // and we should use the addition instance instead
            typedef typename
                tapescript::if_c<
                    cl::tapescript::is_has_operator_real<NativeType>::is_const
                    , Type const&
                    , Type >::type type;
            type v = other_value;
            this_value = v.operator tape_double();
        }

        // If ValueType is a class, constructor should be called
        template <typename ThisType>
        static void convert(ThisType& this_value, Type& other_value)
        {
            this_value = other_value.operator tape_double();
        }
    };

    /// <summary>This template and its specializations provide conversion to tape_double value_type.</summary>
    template <typename Type, typename ValueType, typename NativeType
        , typename IsSame, typename IsConv, typename IsArithm, typename IsHasOper>
    struct tape_double_convert<Type, ValueType, NativeType
        , IsSame, IsArithm, IsConv, IsHasOper, std::true_type>
    {
        typedef ValueType type;

        // If ValueType is a class, constructor should be called
        template <typename ThisType>
        static void convert(ThisType& t, Type const& other_value)
        {
            t.value_ = static_cast<ValueType>(other_value);
        }
    };

    /// <summary>This template and its specializations provide conversion to tape_double value_type.</summary>
    template <typename Converter, typename ThisType, typename From>
    inline typename void
    convert(ThisType& this_value, From const& other_value)
    {
        Converter::convert(this_value, other_value);
    };
}

#endif // cl_tape_impl_doubleconverter_hpp