################################################################################
#
#  Makefile for ecat2ana
#
#  Environment variables:
#	OS = 'operating system'
#	INCLUDE = 'include path'
#	LDFLAGS = 'library path'
#       PET_ROOT = 'root directory of petsoftware'
#	PET_BIN = 'where to install binaries'
#	PET_ARCHIVE = 'where to archive sources'
#
#  Main targets:
#	compile: compile the program
#	install: install the program
#       progversion: create version information file
#       distribution: create xml documentation, archive sources and exes
#	clean & clobber: clean the directory
#
#  Date:
#  2007-03-25 Vesa Oikonen
#  2007-09-12 VO
#  2009-10-29 VO
#  2012-09-25 VO
#
#
################################################################################
# ------------- General settings:----------------------------------------------#
PROGRAM		= ecat2ana
SOURCES 	= $(PROGRAM).c 
OBJECTS 	= ${SOURCES:.c=.o}
CC		= gcc
CFLAGS 		= -std=gnu99 -s -Wall -O3 ${INCLUDE}
LIBS 		= -ltpcimgp -ltpcimgio -ltpcmisc -lm
ZIP		= zip
PRVERS		= progvers
ADDDATE		= adddate
PDOC2HTML	= pdoc2htm -L

# ------------- OS/terminal dependent settings:--------------------------------#
ifeq ($(OS),Windows_NT)
EXT		= .exe
else
SUF		= ./
endif

# ------------- Compilation: --------------------------------------------------#
compile: ${PROGRAM}

${PROGRAM}: ${OBJECTS}
	 ${LINK.c} -o ${PROGRAM} ${CFLAGS} ${LDFLAGS} ${OBJECTS} ${LIBS}
ifneq ($(OS),Windows_NT)
	 chmod a+x ${PROGRAM}
endif

# ------------- Version information: ------------------------------------------#
progversion: ${PROGRAM}.version

${PROGRAM}.version: ${PROGRAM}
	${PRVERS} ${SUF}${PROGRAM}${EXT} ${PROGRAM}.version

# ------------- Installation: -------------------------------------------------#
install: progversion
	cp -f ${PROGRAM}${EXT} ${PET_BIN}/${PROGRAM}${EXT}
	${ADDDATE} -i=${PROGRAM}.version -o=${PET_BIN} -nodate -copy ${PROGRAM}${EXT}

# ------------- Distribution: -------------------------------------------------#
distribution: html progversion xml execarchive srcarchive

html:	${PROGRAM}
	./${PROGRAM}${EXT} -H > tmp.txt
	${PDOC2HTML} tmp.txt ${PET_ARCHIVE}/doc/${PROGRAM}.html
	rm -f tmp.txt

xml: ${PROGRAM}
	make_xmlhelp.sh ${PROGRAM}

execarchive: ${PROGRAM}
	make_execarchive.sh ${PROGRAM} ${EXT}

srcarchive: clobber
	make_srcarchive.sh ${PROGRAM}

# ------------- Cleanup: ------------------------------------------------------#
clean:
	rm -f ${PROGRAM}${EXT} ${PROGRAM} ${OBJECTS}

clobber: clean
