###############################################################################
#
# Copyright (c) 2005-2012 by Turku PET Centre
#
# Purpose: Makefile for PET Image I/O C library (libtpcimgio).
#
# Environment variables:
#	OS = 'operating system'
#	INCLUDE = 'include path' (optional)
#	LDFLAGS = 'library path' (optional)
#	PET_BIN = 'where to install binaries'
#	PET_LIB = 'where to install libraries'
#	PET_ARCHIVE = 'where to archive sources'
#
# Required files:
#	Doxyfile
#
# Main targets:
#	compile: compile the library and the test program
#	test: run tests on library
#	install: install the library and the test program
#	distribution: archiving sources and html docs for backup and distribution
#	clean & clobber: clean the directory
#
# Compile with: make -e
# Compile with: make -e "ANSI = -ansi -pedantic" for ANSI compatibility
#
# Version:
#  2005-22-02 Jarkko Johansson
#  2005-03-03 JJ
#	Set environment variables INCLUDE and LDFLAGS to location of header
#	files and libraries to be linked (linking for the test program).
# 2005-04-19 JJ
#	Targets for installation and htlm creation added.
#	Htlm creation with doxygen requires the Doxyfile.
# 2005-05-04 JJ
#	Target for archiving added (works only in UNIX/Linux).
# 2005-11-21 VO
#	Information is extracted always from local directory (added ./).
#       Progvers and adddate are called without version numbers.
# 2006-07-30 VO
#       Changed according to example library project dated 2006-06-20.
#       Compiler flag -std=gnu99 (default is gnu89).
# 2006-10-30 VO
#       Added imgunit.c.
# 2007-02-02 VO
#       Removed halflife.c and halflife.h (moved to tpclibmisc).
#       Added imgdecay.c, imgana.c, and img_e63.c.
# 2007-02-27 VO
#       Added img_e7.c.
# 2007-04-03 VO
#       Calls test function 2.
# 2007-07-17 Harri Merisaari
#	Added ANSI flag for C-compiler
# 2008-07-02 VO
#       Added -copy to libinstall
#       CFLAGS -O changed to -O2
# 2009-02-16 VO
#       Added micropet.c
# 2011-09-11 VO
#       Changed to work with 64-bit Windos 7. 
# 2012-01-10 VO
#       Added target finalinstall.
# 2012-02-04 VO
#       Added img_upet.c.
#
#
###############################################################################
# ------------- General settings:----------------------------------------------#
LIBRARY		= libtpcimgio
SOURCES 	= ecat63r.c ecat63w.c ecat63p.c ecat63ml.c \
		  ecat7r.c ecat7w.c ecat7p.c ecat7ml.c ecat7h.c \
		  analyze.c interfile.c micropet.c \
		  imgfile.c img_ana.c img_upet.c img_e63.c img_e7.c \
                  imgunit.c imgmax.c imgdecay.c vol.c img.c \
		  sifio.c sif.c weight.c libtpcimgiov.c
OBJECTS 	= ${SOURCES:.c=.o}
LIBARCHIVE	= ${LIBRARY}.a
DOXYGEN		= doxygen
# Add -m64 or -m32 below when necessary
CC		= gcc
CFLAGS 		= -std=gnu99 -s -Wall -O3 ${INCLUDE} ${ANSI}
LIBS 		= ${LDFLAGS} -ltpcimgio -ltpcmisc -lm
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
	${CC} ${LIBRARY}.c -o ${LIBEXEC} ${CFLAGS} ${LIBS}
ifneq ($(OS),Windows_NT)
	 chmod a+x ${LIBEXEC}
endif

# ------------- Testing: ------------------------------------------------------#
test: ${LIBEXEC}
	${SUF}${LIBRARY} -f 1
	${SUF}${LIBRARY} -f 2

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

# ------------- Distribution: ----------------------------------------------------------#
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



