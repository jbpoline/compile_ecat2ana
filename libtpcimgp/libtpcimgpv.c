/*******************************************************************************

  Copyright (c) 2005-2012 by Turku PET Centre

  File: libtpcimgpv.c  

  Purpose: functions for printing the information on the library. 
  
  Library version: See the history printing function below.

*/
/******************************************************************************/

/** Library name. */
#define LIB_NAME "libtpcimgp"

/** Library version. */
#define LIB_VERSION "2.0.6"

/** Copyright. */
#define COPYRIGHT "(c) 2005-2012 by Turku PET Centre, University of Turku"

/******************************************************************************/

#include <stdio.h>
#include "libtpcimgpv.h"

/******************************************************************************/

/** 
    Print the library readme to specified FILE pointer, use stdout for screen.
*/
void libtpcimgp_print_readme(FILE *fp){

  fputs(
  "\n"
  LIB_NAME" "LIB_VERSION" "COPYRIGHT"\n"
  "\n"
  "Build "__DATE__" "__TIME__"\n"
  "\n"
  "\n"
  "PET Image Processing library   \n"
  "-------------------------------------------------------------------------------\n"
  "\n"
  "Libtpcimgp is distributed under the GNU Lesser General Public Licence\n"
  "http://www.turkupetcentre.net/software/lgpl-license.html\n"
  "More information on open source licences: http://www.opensource.org\n"
  "\n"
  "Description\n"
  "-------------------------------------------------------------------------------\n"
  "\n"
  "The libtpcimgp library is a collection of commonly used C files\n"
  "at Turku PET Centre for image processing.\n"
  "\n"
  "List of files\n"
  "-------------------------------------------------------------------------------\n"
  "\n"
  "Doxyfile        - Doxygen configuration file.\n"
  "include/        - Includes all the header files for the .c files\n"
  "Makefile        - The makefile\n"
  "\n"
  "** .c files **\n"
  "imgarithm.c     - Functions for image arithmetics.\n"
  "imgfilter.c     - Functions for doing Gaussian filtering.\n"
  "imgflip.c       - Functions for turning image volume.\n"
  "imgframe.c      - Functions for setting IMG frame times.\n"
  "imgmatch.c      - Functions for comparing IMG contents.\n"
  "imgscanner.c    - Functions for setting scanner specific parameters.\n"
  "imgsegm.c       - Functions for segmenting images.\n"
  "imgthrs.c       - Functions for thresholding and outlier filtering.\n"
  "imgtiff.c       - Functions for writing TIFF images.\n"
  "imgtransform.c  - Functions for image transformations.\n"
  "libtpcimgp.c    - Functions for running tests on libtpcimgp.\n"
  "libtpcimgpv.c   - Functions for printing information on this library.\n"
  "point.c         - Functions for processing with points.\n"
  "\n"
  "Using the library\n"
  "-------------------------------------------------------------------------------\n"
  "\n"
  "The program that uses the library should include the .h file it needs.\n"
  "Example 1:\n"
  "    \\#include <imgproc.h>  // includes all library functions\n"
  "Example 2:\n"
  "    \\#include <imgtransform.h>   // includes functions for transforms \n"
  "    \\#include <point.h>  // includes the 3D point structure\n"
  "    \\#include <libtpcimgpv.h>  // includes the information\n"
  "    \\#include <imgfilter.h>  // includes functions for Gaussian filtering\n"
  "\n"
  "libtpcimgp depends on libtpcimgio and libtpcmisc.\n"
  "Example:\n"
  "  gcc myprog.c -ltpcimgp -ltpcimgio -ltpcmisc \n"
  "\n",
  fp);
  fflush(fp);
}

/** 
    Print the library History to specified FILE pointer, 
    use stdout for screen.
*/
void libtpcimgp_print_history(FILE *fp)
{
  fputs(
  " \n"
  "2005-01-04 version 1.0.0 Vesa Oikonen, Calle Laakkonen, Riku Klén, Jarkko Johansson\n"
  "  Former libpet, the common PET C library, has been divided up in smaller\n"
  "  sublibraries that each handle a specific task. This library includes functions\n"
  "  utilised in image processing.\n"
  "2005-04-21 1.0.1 JJ\n"
  "  Changes in files libtpcimgpv.c and .h:\n"
  "  Functions libtpcimgp_print_readme(), libtpcimgp_print_history(),\n"
  "  libtpcimgp_print_dreadme() and libtpcimgp_print_dhistory()\n"
  "  are used for getting the Readme and History messages for the libtpcimgp,\n"
  "  with and without Doxygen style tags. Remember to update the\n"
  "  functions to get correct Readme and History messages!\n"
  "  Function libtpcimgp_print_build(), is used for getting the build\n"
  "  information.\n"
  "2005-04-26 1.1.0 JJ\n"
  "  Compiled with MinGW.\n"
  "  Changes made in class point:\n"
  "  round() -> pRound() (conflict with math.h round())\n"
  "  PI = value -> \\#ifndef PI = value \\#endif\n"
  "2005-06-02 1.1.1 Kaisa Sederholm\n"
  "  mestim.c added \n"
  "2005-06-09 1.2.0 KS\n"
  "  files median.c, lms.c, lts.c ans mestim.c removed and added to\n"
  "  libtpcmodel library\n"
  "2006-06-19 1.2.1 KS\n"
  "  Functions imgMatchMatrix and imgMatchHeader were added to imgproc.c.\n"
  "2006-08-08 1.3.0 KS\n"
  "  imgfilter.c containing functions for Gaussian filtering was added.\n"
  "2006-10-30 1.3.1 VO\n"
  "  imgthrs.c was added.\n"
  "2007-02-02 1.3.2 VO\n"
  "  Max and min search functions moved into libtpcimgio.\n"
  "  Added more functions for comparing IMG contents.\n"
  , fp);
  fputs(
  "2008-07-14 1.3.3 VO\n"
  "  imgDeleteFrameOverlap() rewritten, previous version saved as *_old().\n"
  "  Added function imgCutoff()\n"
  "2009-02-03 1.3.4 VO\n"
  "  Added functions imgExistentTimes() and imgThresholdingLowHigh().\n"
  "  Function imgFrameIntegral() sums data if frame times are all zeroes;\n"
  "  therefore imgThresholding() works also in those cases.\n"
  "  Function imgThresholdTemplate() also works with preset template."
  "2009-02-04 1.4.0 VO\n"
  "  imgsegm.c was added, including functions for flexible segmentation.\n"
  "2009-04-30 1.4.1 VO\n"
  "  Bug corrected in imgExistentTimes().\n"
  "2009-08-21 1.4.2 VO\n"
  "  Added functions imgThresholdTemplateCount() and imgConvertUnit().\n"
  "2009-12-29 1.4.3 VO\n"
  "  Additions in imgfilter.c and imgfilter.h.\n"
  "  Addition in imgmatch.c and imgmatch.h.\n"
  "2010-11-30 1.4.4 VO\n"
  "  Added function img2cube() in imgtransform.c and .h.\n"
  "2011-09-09 2.0.0 VO\n"
  "  Changes to compile, install and distribute in 64-bit systems.\n"
  "2012-02-17 2.0.1 VO\n"
  "  Tiny changes to compile with new libtpcimgio version.\n"
  "2012-04-22 2.0.2 VO\n"
  "  Bug fix in imgGaussianFilter().\n"
  "2012-09-21 2.0.2 VO\n"
  "  Fixed typos in function descriptions; version number not changed.\n"
  "2012-09-21 2.0.3 VO\n"
  "  Applied defines IMG_DC_UNKNOWN, IMG_DC_CORRECTED and IMG_DC_NONCORRECTED.\n"
  "2012-10-26 2.0.4 VO\n"
  "  Added imgtiff.c and imgtiff.h.\n"
  "2012-11-07 2.0.5 VO\n"
  "  Updated imgFrameIntegral().\n"
  "  Function interface changed in imgarithm.c and imgarithm.h.\n"
  "2012-11-13 2.0.6 VO\n"
  "  Small changes in imgGaussianFilter(), not affecting results.\n"
  "\n", fp);

  fflush(fp);
}

/** 
    Print the library Readme to specified FILE pointer with Doxygen tags, 
    use stdout for screen.
*/
void libtpcimgp_print_dreadme(FILE *fp)
{
  fprintf(fp,"/**\n\\mainpage %s %s %s\n\\section readme Readme\n<pre>\n",
    LIB_NAME,LIB_VERSION,COPYRIGHT);
  libtpcimgp_print_readme(fp);
  fprintf(fp,"\n</pre> \n */\n");
}

/** 
    Print the library History to specified FILE pointer with Doxygen tags, 
    use stdout for screen.
*/
void libtpcimgp_print_dhistory(FILE *fp)
{
  fprintf(fp,"/**\n\\page History\n\\section history History\n<pre>\n");
  libtpcimgp_print_history(fp);
  fprintf(fp,"\n</pre> \n */\n");
}

/** 
    Print the build info to specified FILE pointer, use stdout for screen.
*/
void libtpcimgp_print_build(FILE *fp)
{
  fprintf(fp,"\n %s %s %s\n", LIB_NAME, LIB_VERSION, COPYRIGHT);
  fprintf(fp,"\n Build %s %s\n", __DATE__, __TIME__);
#ifdef __STRICT_ANSI__
  fprintf(fp," (ANSI compatible build)\n");
#endif
#if defined(__FILE__) && defined(__TIMESTAMP__)
  printf(" File %s last edited on %s\n", __FILE__, __TIMESTAMP__);
#endif
#if defined(__STDC_VERSION__)
  printf(" Version of C: %ld\n", __STDC_VERSION__);
#endif
#if defined(__STDC_ISO_10646__)
  printf(" Compiler ISO/IEC 10646 specification: %ld\n", __STDC_ISO_10646__);
#endif
#if defined(__GNUC__) && defined(__VERSION__)
  printf(" GNU C version: %s\n", __VERSION__);
#endif
#if defined(__x86_64__) || defined(__LP64__) || defined(__ppc64__)
  printf(" Architecture: 64-bit\n");
#else
  printf(" Architecture: 32-bit\n");
#endif
}
/******************************************************************************/

/******************************************************************************/
