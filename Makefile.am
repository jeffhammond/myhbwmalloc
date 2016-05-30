ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/src

EXTRA_DIST = VERSION

lib_LTLIBRARIES = libhbwmalloc.la

libhbwmalloc_la_SOURCES = src/hbwmalloc.c        \
                          src/dlmalloc.c         \
			  src/fortraninterface.f90

src/mspace.h

libhbwmalloc_la_LDFLAGS = -version-info $(libhbwmalloc_abi_version)

include_HEADERS = include/hbwmalloc.h src/mspace.h

bin_PROGRAMS =
check_PROGRAMS =
TESTS =
XFAIL_TESTS =

MPIEXEC = mpiexec -n 2
LOG_COMPILER=$(MPIEXEC)

# BEGIN TESTING

check_PROGRAMS += test/test_features 		\
		  test/test_libnuma		\
		  test/test_performance

TESTS          += test/test_features 		\
                  test/test_libnuma		\
                  test/test_performance

test_test_features_LDADD    = libhbwmalloc.la
test_test_libnuma_LDADD     = libhbwmalloc.la
test_test_performance_LDADD = libhbwmalloc.la

# END TESTING

.PHONY: checkprogs
checkprogs: $(check_PROGRAMS)
