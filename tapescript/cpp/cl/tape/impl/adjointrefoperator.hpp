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

#ifndef cl_tape_impl_adjointrefoperator_hpp
#define cl_tape_impl_adjointrefoperator_hpp

#define OPERATOR_TRAITS_ADJOINTREF_DECL(oper)                                                                                           \
    template <typename Base, typename FrameworkType, typename LeftConst, typename RightConst>                                           \
    struct operator_traits<cl::tapescript::tape_ref<Base>, FrameworkType, struct oper                                                    \
                , LeftConst, RightConst, tape_ref_operators>                                                                              \
                : operator_ref_<typename cl::tapescript::tape_ref<Base>::inner_type, oper, std::false_type >                             \
    {};                                                                                                                                 \
                                                                                                                                        \
    template <typename FrameworkType, typename Base, typename LeftConst, typename RightConst>                                           \
    struct operator_traits<FrameworkType, cl::tapescript::tape_ref<Base>, oper, LeftConst, RightConst, tape_ref_operators>                 \
        : operator_ref_<typename cl::tapescript::tape_ref<Base>::inner_type, oper, std::false_type >                                     \
    {};                                                                                                                                 \
                                                                                                                                        \
    template <typename Base, typename LeftConst, typename RightConst>                                                                   \
    struct operator_traits<cl::tapescript::tape_ref<Base>, cl::tapescript::tape_ref<Base>, oper, LeftConst, RightConst, tape_ref_operators> \
        : operator_ref_<typename cl::tapescript::tape_ref<Base>::inner_type, oper, std::false_type >                                     \
    {};                                                                                                                                 \

#define ADJOINT_REF_OPERATOR_IMPL(oper, oper_i)                                                                                         \
    template <typename Return>                                                                                                          \
    struct operator_ref_<Return, struct oper, std::false_type>                                                                          \
    {                                                                                                                                   \
        typedef Return type;                                                                                                            \
    public:                                                                                                                             \
        template <typename left_impl, typename right_impl>                                                                              \
        inline type operator()(left_impl& left, right_impl const& right)                                                                \
        {                                                                                                                               \
            return deref(std::ref(left)) oper_i deref(std::ref(right));                                                                 \
        }                                                                                                                               \
    };                                                                                                                                  \

#endif // cl_tape_impl_adjointrefoperator_hpp