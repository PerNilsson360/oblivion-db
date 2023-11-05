check_PROGRAMS = test
test_SOURCES = test.cc
test_LDADD = $(top_srcdir)/src/liboblivion.a
TESTS = $(check_PROGRAMS) test-object-atomic-values.sh test-string-array.sh

AM_CPPFLAGS = -g -I$(top_srcdir)/include -I$(top_srcdir)/src

clean-local:
	-rm *~
