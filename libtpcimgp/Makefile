################################################################################
#
#  Copyright (c) 2005-2012 by Turku PET Centre
#
#  Purpose: Makefile for PET Image processing C library (libtpcimgp).
#
#  Environment variables:
#	OS = 'operating system'
#	INCLUDE = 'include path'
#	LDFLAGS = 'library path'
#	PET_BIN = 'where to install binaries'
#	PET_LIB = 'where to install libraries'
#	PET_ARCHIVE = 'where to archive sources'
#
#  Required files:
#	Doxyfile
#
#  Main targets:
#	compile: compile library and executable
#	test: run tests on library
#	install: install library and executable
#	distribution: archiving sources and html docs for backup and distribution
#	clean & clobber: clean the directory
#
#  Compile with: make -e
#  This library is dependent on TPC libraries libtpcimgio and libtpcmisc.
#
#  Date:
#  2005-05-19 Jarkko Johansson
#  2005-11-07 Calle Laakkonen
#  2005-11-29 Jarkko Johansson
#  2006-02-09 Seppo Tyrvainen
#  2006-06-20 Jarkko Johansson
#  2006-07-13 Vesa Oikonen
#    Added the option to line "PRVERS = progvers -opt=-b".
#    Replaced the rule html by distribution in the list above.
#  2006-10-02 VO
#    Added -std=gnu99 to CFLAGS.
#  2006-10-30 VO
#    Added new C files.
#  2007-02-02 VO
#    Added new C files.
#    imgproc.c removed (contents splitted).
#    Added target 'test'.
#  2008-07-02 VO
#    Added -copy to libinstall
#    CFLAGS -O changed to -O2
#  2009-02-04 VO
#    Added imgsegm.c.
# 2011-09-13 VO
#    Changed to work with 64-bit Windos 7. 
# 2012-01-10 VO
#    Added target finalinstall.
# 2012-02-17 VO
#    Includes libtpcmisc.
# 2012-10-26 VO
#    Added imgtiff.c
#
################################################################################
# ------------- General settings:----------------------------------------------#
LIBRARY		= libtpcimgp
SOURCES 	= imgarithm.c imgmatch.c imgscanner.c imgsegm.c imgflip.c \
                  imgtransform.c point.c imgfilter.c \
                  imgthrs.c imgframe.c imgtiff.c libtpcimgp.c libtpcimgpv.c
OBJECTS 	= ${SOURCES:.c=.o}
LIBARCHIVE	= ${LIBRARY}.a
DOXYGEN		= doxygen
CC		= gcc
# Add -m64 or -m32 below when necessary
CFLAGS 		= -std=gnu99 -s -Wall -O3 ${INCLUDE}
LIBS 		= ${LDFLAGS} -ltpcimgio -ltpcmisc -ltpcimgp -lm
ZIP		= zip
PRVERS		= progvers -opt=-b
ADDDATE		= adddate
PDOC2HTML	= pdoc2htm -L

# ------------- OS/terminal dependent settings:--------------------------------#
ifeq ($(OS),Windows_NT)
SUF		= 
EXT		= .exe
endif

LIBEXEC 	= ${LIBRARY}${EXT}

# ------------- Compilation: --------------------------------------------------#
compile: ${LIBARCHIVE} ${LIBEXEC}

# Compile library
${LIBARCHIVE}: ${OBJECTS}
	${AR} -rvs ${LIBARCHIVE} ${OBJECTS}

# Compile executable
${LIBEXEC}: ${LIBARCHIVE} ${LIBRARY}.c
	${CC} ${LIBRARY}.c -o ${LIBEXEC}  ${CFLAGS} ${LIBS}

ifneq ($(OS),Windows_NT)
	 chmod a+x ${LIBEXEC}
endif

# ------------- Testing: ------------------------------------------------------#
test: ${LIBEXEC}
	${SUF}${LIBRARY} -v -f 0

# ------------- Version information: ------------------------------------------#
libversion: ${LIBRARY}.version

${LIBRARY}.version: ${LIBEXEC}
	${PRVERS} ${LIBEXEC} ${LIBRARY}.version

# ------------- Installation: -------------------------------------------------#
install: libversion libinstall execinstall

libinstall: ${LIBARCHIVE}
	${ADDDATE} -i=${LIBRARY}.version -o=${PET_LIB} -nodate -copy ${LIBARCHIVE}

execinstall: ${LIBEXEC}
	${ADDDATE} -i=${LIBRARY}.version -o=${PET_BIN} -nodate -copy ${LIBEXEC}

finalinstall: install
	cp -f ${LIBARCHIVE} ${PET_LIB}/${LIBARCHIVE}
	cp -f ${LIBEXEC} ${PET_BIN}/${LIBEXEC}

# ------------- Distribution: -------------------------------------------------#
distribution: libversion doxyapi libbinarchive libsrcarchive

doxyapi: ${LIBARCHIVE} ${LIBEXEC}
	./${LIBEXEC} -R > Readme
	./${LIBEXEC} -H > History
	make_doxyapi.sh ${LIBRARY}

libbinarchive: ${LIBARCHIVE} ${LIBEXEC}
	make_libbinarchive.sh ${LIBRARY} ${LIBEXEC}

libsrcarchive: clobber
	make_libsrcarchive.sh ${LIBRARY}

# ------------- Cleanup: ------------------------------------------------------#
clean:
	rm -f ${LIBRARY}${EXT} ${OBJECTS} ${LIBRARY}.a

clobber: clean
