
#pragma once

# ifndef CL_BASE_REQUIRE_INCLUDED
# define CL_BASE_REQUIRE_INCLUDED

// Bases definitions
#include <cl/tape/impl/inner_double.hpp>

// definitions that must come before base implementations
# include <cppad/error_handler.hpp>
# include <cppad/local/define.hpp>
# include <cppad/local/cppad_assert.hpp>
# include <cppad/local/declare_ad.hpp>

// grouping documentation by feature
# include <cppad/local/base_cond_exp.hpp>
# include <cppad/local/base_std_math.hpp>

// must define template class numeric_limits before the base cases
# include <cppad/local/limits.hpp>
# include <cppad/local/epsilon.hpp> // deprecated

// base cases
#include <cl/tape/impl/base_inner_double.hpp>

# endif
