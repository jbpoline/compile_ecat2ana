################################################################################
#
#  Makefile for Turku PET Centre libraries.
#
#  Environment variables:
#	OS = 'operating system'
#	INCLUDE = 'include path'
#	LDFLAGS = 'library path'
#	PET_BIN = 'where to install binaries'
#	PET_LIB = 'where to install libraries'
#	PET_ARCHIVE = 'where to archive sources'
#
#  Main targets:
#	compile: compile library and executable
#	install: install library and executable
#	distribution: archiving sources and html docs for backup and distribution
#	clean & clobber: clean the directory
#
#  Compile with: make -e
#       For ANSI compatible version compile with: make "ANSI = -ansi -pedantic"
#
# Date:
# 2007-03-30 Kaisa Liukko
#       Copied from example_library_project folder
# 2007-07-17 Harri Merisaari
#       Added flag for ANSI (-ansi -pedantic) compiling
# 2008-07-02 Vesa Oikonen
#       Added -copy to libinstall
# 2008-07-14 VO
#       Added backup.c
# 2010-04-12 VO
#       petunits.c and .h moved here from libtpccurveio
# 2010-05-18 VO
#       Added datetime.c and datetime.h
# 2011-09-11 VO
#       Changed to work with 64-bit Windos 7.
# 2012-01-10 VO
#       Added target finalinstall.
# 2012-02-23 VO
#       Added branch.c.
# 2012-09-28 VO
#       Added rname.c.
#
#
################################################################################
# ------------- General settings:----------------------------------------------#
LIBRARY		= libtpcmisc
SOURCES 	= substitutions.c backup.c branch.c idcrypt.c intex.c petc99.c \
		  studynr.c swap.c halflife.c ift.c iftfile.c iftsrch.c \
                  decpoint.c quots.c petunits.c rname.c datetime.c \
                  memory_handler.c libtpcmiscv.c
OBJECTS 	= ${SOURCES:.c=.o}
LIBARCHIVE	= ${LIBRARY}.a
DOXYGEN		= doxygen
CC		= gcc
# Add -m64 or -m32 below when necessary
CFLAGS 		= -std=gnu99 -s -Wall -O3 ${INCLUDE} ${ANSI}
LIBS 		= ${LDFLAGS} -ltpcmisc -L.
ZIP		= zip
PRVERS		= progvers -opt=-b
ADDDATE		= adddate
PDOC2HTML	= pdoc2htm -L

# ------------- OS/terminal dependent settings:--------------------------------#
ifeq ($(OS),Windows_NT)
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
	rm -f ${LIBEXEC} ${OBJECTS} ${LIBRARY}.a

clobber: clean

