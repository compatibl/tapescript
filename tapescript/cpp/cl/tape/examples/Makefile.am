
AM_CPPFLAGS = -I${top_srcdir} -I${top_builddir}

noinst_PROGRAMS = tape_example

AM_CPPFLAGS += -std=c++11
AM_CPPFLAGS += -DCL_TAPE_INNER_ARRAY_ENABLED -DCL_TAPE_CPPAD -DCL_TAPE -DCL_TAPE_CAN_GET_VALUE

tape_exampledir=${includedir}
tape_example_HEADERS = \
    impl/adapter_performance_test.hpp\
    impl/array_examples.hpp\
    impl/basic_examples.hpp\
    impl/double_copy.hpp\
    impl/double_examples.hpp\
    impl/linear_regression.hpp\
    impl/performance_tests.hpp\
    impl/performance_utils.hpp\
    impl/quadratic_regression_examples.hpp\
    impl/regression_examples_impl.hpp\
    impl/regression_examples.hpp\
    impl/utils.hpp
    
tape_example_SOURCES = \
    tape_example.cpp

EXTRA_DIST = \
	autogen.sh

dist-hook:
	mkdir -p $(distdir)/build
