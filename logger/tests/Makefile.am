# This file is part of SWIFT.
# Copyright (c) 2019 loic.hausammann@epfl.ch.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Add the source directory and the non-standard paths to the included library headers to CFLAGS
AM_CFLAGS = $(PYTHON_INCS) -I$(top_srcdir)/src -I$(top_srcdir)/logger $(HDF5_CPPFLAGS) $(GSL_INCS) $(FFTW_INCS)

AM_LDFLAGS = ../../src/.libs/libswiftsim.a ../.libs/liblogger.a $(HDF5_LDFLAGS) $(HDF5_LIBS) $(FFTW_LIBS) $(TCMALLOC_LIBS) $(JEMALLOC_LIBS) $(TBBMALLOC_LIBS) $(GRACKLE_LIBS) $(GSL_LIBS) $(PROFILER_LIBS) $(PYTHON_LIBS) -lswiftsim

# List of programs and scripts to run in the test suite
TESTS = testLogfileHeader testLogfileReader testTimeArray testQuickSort testVR

# List of test programs to compile
check_PROGRAMS = testLogfileHeader testLogfileReader testTimeArray testQuickSort testVR

# Rebuild tests when SWIFT is updated.
$(check_PROGRAMS): ../../src/.libs/libswiftsim.a ../.libs/liblogger.a

# Sources for the individual programs
testLogfileHeader_SOURCES = testLogfileHeader.c
testLogfileReader_SOURCES = testLogfileReader.c
testTimeArray_SOURCES = testTimeArray.c
testQuickSort_SOURCES = testQuickSort.c
testVR_SOURCES = testVR.c

# Files necessary for distribution
EXTRA_DIST = testLogfileHeader.yml testLogfileReader.yml
