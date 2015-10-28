# Process this file with autoconf to produce a configure script.
AC_INIT([TapeScript], [2.0])

AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign])


AC_COPYRIGHT([
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
limitations under the License.])

AC_PROG_AWK
AC_PROG_INSTALL

# Help the compiler find external software

[project_path="`pwd`"]
[dependencies_include_path="/../../../../../dependencies/cpp"]
[tapescript_include_path="/../../../"]	
AC_SUBST([CPPFLAGS],["${CPPFLAGS} -I${project_path}${tapescript_include_path}"])
AC_SUBST([CPPFLAGS],["${CPPFLAGS} -I${project_path}${dependencies_include_path}"])
#AC_SUBST([CPPFLAGS],["${CPPFLAGS} -I${dependencies_include_path}"])

AC_SUBST([CPPFLAGS],["${CPPFLAGS} -DNDEBUG"])

# Continue setup

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_LIBTOOL

AX_CHECK_COMPILE_FLAG([-std=c++11], [
  CFLAGS+=" -std=c++11"
], [
  echo "C compiler cannot compile C11 code"
  exit -1
])


AM_CONDITIONAL(ENABLE_STATIC, test "${enable_static}" == "yes")

AX_CXX_COMPILE_STDCXX_11()

AC_ARG_WITH([cpp11],
            [CFLAGS+=" -std=c++11"], [echo "with cpp11"])

AC_LANG([C++])

AC_ARG_ENABLE([openmp],
              AC_HELP_STRING([--enable-openmp],
                             [If enabled, configure will try to detect
                              and enable OpenMP support.]),
              [ql_openmp=$enableval],
              [ql_openmp=no])
if test "$ql_openmp" = "yes" ; then
   AC_OPENMP
   AC_SUBST([CXXFLAGS],["${CXXFLAGS} ${OPENMP_CXXFLAGS}"])
fi

# Check for Emacs
AM_PATH_LISPDIR
AM_CONDITIONAL(EMACS_FOUND, test "${EMACS}" != "no")

# done, output the configured files
AC_CONFIG_FILES([
    Makefile
    ])
AC_OUTPUT
