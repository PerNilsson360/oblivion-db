BUILT_SOURCES = JsonParser.hh JsonParser.cc JsonScanner.cc location.hh position.hh stack.hh
CLEANFILES = JsonParser.hh JsonParser.cc JsonScanner.cc location.hh position.hh stack.hh JsonParser.output
AM_CPPFLAGS = --std=c++17 -g -I$(top_srcdir)/include
AM_YFLAGS = -t -v -d -Wno-yacc
AM_LFLAGS = -olex.yy.c
lib_LIBRARIES = liboblivion.a
liboblivion_a_SOURCES = JsonParser.yy JsonScanner.ll JsonDriver.cc Json.cc Schema.cc
bin_PROGRAMS = jsch
jsch_SOURCES = jsch.cc $(liboblivion_a_SOURCES)


clean-local:
	-rm *~
