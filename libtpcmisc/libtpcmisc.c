/**
  Copyright (c) 2004-2008,2012 by Turku PET Centre

  File: libtpcmisc.c

  Purpose: methods for testing functions in the library libtpcmisc,
  and for printing out libtpcmisc information, such as Readme,
  History and build information.

  NOTICE! This program should always have the same version with the
  library.
   
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details:
  http://www.gnu.org/copyleft/lesser.html

  You should have received a copy of the GNU Lesser General Public License
  along with this library/program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 

  Turku PET Centre, Turku, Finland, http://www.turkupetcentre.fi/

  Modification history:
  2005-02-22 1.0.0 Jarkko Johansson
  2005-04-26 1.1.0 JJ
  2006-03-26 1.1.1 Vesa Oikonen
  2006-06-03 1.1.2 VO
  2006-07-21 1.1.3 VO
  2006-07-25 1.1.4 VO
  2006-10-02 1.1.5 VO
     Bug correction version.
     Compiles independently without other TPC libraries.
  2006-10-12 1.1.6 VO
     Bug correction version.
  2006-10-23 1.1.7 VO
     Added files quots.c and quots.h.
     See iftread.c.
  2007-02-02 1.2.0 VO
  2007-03-30 1.2.1 Kaisa Liukko
    Added memory_handler.c which is used by lse and lsi (libtpcmodel).
    Added isotope I-124 into halflife.c and halflife.h.
  2008-07-14 1.3.2 VO
  2012-01-07 VO
    Tiny changes in comments to prevent warnings from Doxygen.


*/
/*****************************************************************************/
/** Library name. */
#define LIB_NAME "libtpcmisc"

/** Serial numbers for function names. */
#define SWAP 1
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
/*****************************************************************************/
#include "include/libtpcmisc.h"
/*****************************************************************************/
#define MAXVAL 100000;
/*****************************************************************************/

/**
    Function for printing the usage information of the libtpcmisc test program.
*/
void print_usage();

/* Verbose mode switch: */
int VERBOSE = 0;

/**
 * Prints library information and runs some tests
 */
int main(
  /** Nr of arguments */
  int argc,
  /** Pointer to arrays of argument string */
  char *argv[ ]
) {
    int c;
    int errflg=0, functionflag=0, exit_code=0;
    extern char *optarg;
    extern int optind, optopt;

    while ((c = getopt(argc, argv, "vVbBrRhHf:F:")) != -1) {
      switch (c) {

      case 'V':
      case 'v':
	/* Set verbose flag on: */
	VERBOSE = 1;
	break;

      case 'B':
      case 'b':
	/* Print out the build information: */
	libtpcmisc_print_build(stdout);
	return(0);

      case 'r':
	/* Print out the Readme message: */
	libtpcmisc_print_readme(stdout);
	return(0);
      case 'R':
	/* Print out the Readme message with Doxygen tags: */
	libtpcmisc_print_dreadme(stdout);
	return(0);

      case 'h':
	/* Print out the History message: */
	libtpcmisc_print_history(stdout);
	return(0);
      case 'H':
	/* Print out the History message with Doxygen tags: */
	libtpcmisc_print_dhistory(stdout);
	return(0);

      case 'f':
      case 'F':
	/* Set function serial number to be tested: */
	functionflag = atoi(optarg);
	break;

      case ':': /* -f without argument */
	fprintf(stderr,"Option -%c requires an argument\n", optopt);
	errflg++;
	break;
      case '?':
	fprintf(stderr,"Unrecognised option: -%c\n", optopt);
	errflg++;
      }
    }/* End parsing command line options... */

    if (errflg) {
      print_usage();
      return(2);
    }

    if(!functionflag){
      print_usage();
      return(0);
    }

    libtpcmisc_print_build(stdout);

    fprintf(stdout,"\n    NO TEST FUNCTION IMPLEMENTATIONS!\n");

    exit(exit_code);
}

/*!
 * Prints usage information
 */
void print_usage(){

  libtpcmisc_print_build(stdout);

  puts("\n"
       "   Methods for testing functions in the library "LIB_NAME"\n"
       "   and for printing out associated information, such as Readme,\n"
       "   History and build information. \n"
       "\n"
       "   Usage: "LIB_NAME" [-Options]\n"
       "\n");
  puts("   Options:\n"
       "   -h | H \n"
       "       Print out the "LIB_NAME" History message. Include Doxygen style tags\n"
       "       by using option '-H'.\n"
       "   -r | R \n"
       "       Print out the "LIB_NAME" Readme message. Include Doxygen style tags\n"
       "       by using option '-R'.\n"
       "   -b\n");
  puts("       Print out the "LIB_NAME" build information.\n"
       "   -f <function serial number>\n"
       "       Runs test procedures for the functions corresponding to given\n"
       "       'function serial number'. Serial numbers are defined in file\n"
       "       libtpcmisc.c.\n"
       "   -v\n"
       "       Run in verbose mode.\n"
       "\n"
       "   E.g.: "LIB_NAME" -h\n"
       "         "LIB_NAME" -v -f 1 \n"
       "\n");

  fflush(stdout);
}
