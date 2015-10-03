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

# if !defined cl_tape_impl_ad_ad_fields_hpp
# define cl_tape_impl_ad_ad_fields_hpp

# include <cl/tape/impl/reflection/mem_access.hpp>
# include <cl/tape/impl/tape_fwd.hpp>

namespace cl
{
    template <template <class > class Ho_, class Base>
    struct class_fields<Ho_<CppAD::player<Base>>>
    {
        typedef fields <
            decl_mem(CppAD::player<Base>::num_var_rec_)
            , decl_mem(CppAD::player<Base>::num_load_op_rec_)
            , decl_mem(CppAD::player<Base>::num_vecad_vec_rec_)
            , decl_mem(CppAD::player<Base>::op_rec_)
            , decl_mem(CppAD::player<Base>::vecad_ind_rec_)
            , decl_mem(CppAD::player<Base>::op_arg_rec_)
            , decl_mem(CppAD::player<Base>::par_rec_)
            , decl_mem(CppAD::player<Base>::text_rec_)
            , decl_mem(CppAD::player<Base>::op_)
            , decl_mem(CppAD::player<Base>::op_index_)
#           if CL_USE_OP_ARG
                // Excludefrom serialization
                // this field initialize when call forward-start
                , decl_mem(CppAD::player<Base>::op_arg_)
#           endif
            , decl_mem(CppAD::player<Base>::var_index_)
        > type;
    };

    template <template <class > class Ho_, class Base>
    struct class_fields<Ho_<CppAD::ADFun<Base>>>
    {
        typedef fields <
            decl_mem(CppAD::ADFun<Base>::has_been_optimized_)
            , decl_mem(CppAD::ADFun<Base>::check_for_nan_)
            , decl_mem(CppAD::ADFun<Base>::compare_change_count_)
            , decl_mem(CppAD::ADFun<Base>::compare_change_number_)
            , decl_mem(CppAD::ADFun<Base>::compare_change_op_index_)
            , decl_mem(CppAD::ADFun<Base>::num_order_taylor_)
            , decl_mem(CppAD::ADFun<Base>::cap_order_taylor_)
            , decl_mem(CppAD::ADFun<Base>::num_direction_taylor_)
            , decl_mem(CppAD::ADFun<Base>::num_var_tape_)
            , decl_mem(CppAD::ADFun<Base>::ind_taddr_)
            , decl_mem(CppAD::ADFun<Base>::dep_taddr_)
            , decl_mem(CppAD::ADFun<Base>::dep_parameter_)
            , decl_mem(CppAD::ADFun<Base>::taylor_)
            , decl_mem(CppAD::ADFun<Base>::cskip_op_)
            , decl_mem(CppAD::ADFun<Base>::load_op_)
            , decl_mem(CppAD::ADFun<Base>::play_)
            // Currenlty exclude from serialization
            // after call dependent this fields is empty, perhaps
            // we can work without it
#           if defined CL_USE_SPARSE_PACK_            
                , decl_mem(CppAD::ADFun<Base>::for_jac_sparse_pack_)
                , decl_mem(CppAD::ADFun<Base>::for_jac_sparse_set_)
#           endif
        > type;
    };

    template <template <class > class Ho_, class Base>
    struct class_fields<Ho_<cl::tape_function<Base>>> 
        : class_fields<Ho_<CppAD::ADFun<Base>>>
    {};

#   if defined CL_USE_TAPE_SERIALIZATION

    template <template <class > class Ho_, class Base>
    struct class_fields<Ho_<CppAD::ADTape<Base>>>
    {
        typedef fields <
              decl_mem(CppAD::ADTape<Base>::id_)
            , decl_mem(CppAD::ADTape<Base>::size_independent_)
            , decl_mem(CppAD::ADTape<Base>::Rec_)
        > type;
    };

    template <template <class > class Ho_, class Base>
    struct class_fields<Ho_<CppAD::recorder<Base>>>
    {
        typedef fields <
            decl_mem(CppAD::recorder<Base>::abort_op_index_)
            , decl_mem(CppAD::recorder<Base>::thread_offset_)
            , decl_mem(CppAD::recorder<Base>::num_var_rec_)
            , decl_mem(CppAD::recorder<Base>::num_load_op_rec_)
            , decl_mem(CppAD::recorder<Base>::op_rec_)
            , decl_mem(CppAD::recorder<Base>::vecad_ind_rec_)
            , decl_mem(CppAD::recorder<Base>::op_arg_rec_)
            , decl_mem(CppAD::recorder<Base>::par_rec_)
            , decl_mem(CppAD::recorder<Base>::text_rec_)
        > type;
    };

#   endif

}

#endif
