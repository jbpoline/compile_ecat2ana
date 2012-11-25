/**
   Copyright (c) 2004-2008 by Turku PET Centre
   
   File: libtpcimgio.c
   
   Purpose: methods for testing functions in the library libtpcimgio,
   and for printing out libtpcimgio information, such as Readme,
   History and build information. 

   NOTICE! This program should always have the same version with the
   library.
   
   Version:
   
   2005-02-22 1.0.0 Jarkko Johansson
   2005-04-26 1.1.0 Jarkko Johansson
   2005-08-01 1.1.1 Jarkko Johansson
   2005-11-21 1.1.3 Vesa Oikonen
   2005-12-12 1.1.4 Vesa Oikonen
   2006-10-31 1.1.5 Vesa Oikonen
   2006-12-28 1.1.6 Vesa Oikonen
   2007-02-02 1.2.0 Vesa Oikonen
     Added basic test functions for image i/o.
   2007-02-11 1.2.1 VO
   2007-02-27 1.2.2 VO
   2007-03-13 1.2.3 VO
   2007-03-14 1.2.4 VO
   2007-03-25 1.2.5 VO
   2007-04-03 1.3.0 VO
     Added test function for polar map img i/o.
  2007-17-07 Harri Merisaari
    Modified for optional ANSi compatibility
   2007-09-10 1.3.2 VO
   2007-09-10 1.3.3 VO
   2007-09-11 1.3.4 VO
   2007-09-12 1.3.5 VO
   2008-07-07 1.3.6 VO
   2008-07-11 1.3.7 VO
   Later: see libtpcimgiov.c.

*/
/*****************************************************************************/

/** Library name. */
#define LIB_NAME "libtpcimgio"

/** Serial numbers for function names. */
#define IMG_IO 1
#define POLARMAP_IO 2
#define MODULE 3

/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
/*****************************************************************************/
#include "libtpcmisc.h"
#include "include/imgio.h"
/*****************************************************************************/

#define MAXVAL 100000;
/*****************************************************************************/

/* Test function declarations: */
int test_create_img(IMG *img, int dim_x, int dim_y, int dim_z, int dim_t);
int test_create_polarmap(IMG *img, int num_rings, int *sectors_per_ring, float *ring_position, short int *ring_angle, short int start_angle);
int test_img_io(IMG *img);
int test_polarmap_io(IMG *img);
int test_modules(IMG *img);
void print_usage();

/* Verbose mode switch: */
int VERBOSE = 0;

/*!
 * Main function. For retrieval of library information and for testing.
 *
 * @param argc number of arguments
 * @param argv array of pointers to string arguments
 * @returns 0 in case of successfull execution
 */
int main (int argc, char *argv[ ]) {
    int c;
    int errflg=0, functionflag=0, exit_code=0;
    extern char *optarg;
    extern int optind, optopt;
    IMG img, polarmap;

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
	libtpcimgio_print_build(stdout);
	return(0);

      case 'r':
	/* Print out the Readme message: */
	libtpcimgio_print_readme(stdout);
	return(0);
      case 'R':
	/* Print out the Readme message with Doxygen tags: */
	libtpcimgio_print_dreadme(stdout);
	return(0);

      case 'h':
	/* Print out the History message: */
	libtpcimgio_print_history(stdout);
	return(0);
      case 'H':
	/* Print out the History message with Doxygen tags: */
	libtpcimgio_print_dhistory(stdout);
	return(0);

      case 'f':
      case 'F':
	/* Set function serial number to be tested: */
	functionflag = atoi(optarg);
	break;

      case ':': /* -f or -z without argument */
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

    libtpcimgio_print_build(stdout);

    /* Make a test IMG */
    imgInit(&img);
    if(test_create_img(&img, 3, 4, 2, 5)) {
      printf("cannot create test data.\n");
      return(1);
    }


    /* Choose function(s) to test: */
    switch(functionflag){
    case IMG_IO:
      exit_code = test_img_io(&img);
      break;
    case POLARMAP_IO:
      {
      int num_rings=4;
      int sectors_per_ring[]={6,6,4,1};
      float ring_position[]={0,0.2,0.4,0.6};
      short int ring_angle[]={90,90,45,0};
      short int start_angle=0;
      imgInit(&polarmap);
      if(test_create_polarmap(&polarmap, num_rings, sectors_per_ring, ring_position, ring_angle, start_angle)) {
        printf("cannot create test data.\n");
        return(1);
      }
      exit_code = test_polarmap_io(&polarmap);
      }
      imgEmpty(&polarmap);
      break;
    case MODULE:
      exit_code = test_modules(&img);
      break;
    default:
      fprintf(stdout,"\n    NO TEST FUNCTION IMPLEMENTATIONS!\n");
      break;
    }
    imgEmpty(&img);
    printf("\n exit_code := %d\n", exit_code);
    exit(exit_code);
}
/******************************************************************************/

/******************************************************************************/
/*!
 * Create IMG for testing purposis.
 *
 * @param img	Pointer to an initiated but not allocated IMG
 * @param dim_x	X dimension (nr of columns)
 * @param dim_y	Y dimension (nr of rows)
 * @param dim_z	Z dimension (nr of planes)
 * @param dim_t time dimension (nr of time frames, or gates)
 * @returns 0 if IMG could be created, or nonzero in case of error.
 */
int test_create_img(IMG *img, int dim_x, int dim_y, int dim_z, int dim_t) {
  int zi, xi, yi, fi;
  float f=-100.0;

  if(img==NULL || img->status!=IMG_STATUS_INITIALIZED) return(1);
  if(imgAllocate(img, dim_z, dim_y, dim_x, dim_t)){
    printf("    \nError in image allocation: imgAllocate() @ img.c\n");
    return(1);
  }
  img->dimt=dim_t; img->dimx=dim_x; img->dimy=dim_y; img->dimz=dim_z;
  for(zi=0; zi<img->dimz; zi++) img->planeNumber[zi]=zi+1;
  img->type = IMG_TYPE_IMAGE;
  for(fi=0; fi<dim_t; fi++) {
    if(fi==0) img->start[fi]=0.0; else img->start[fi]=img->end[fi-1];
    img->end[fi]=(fi+1)*60.0;
    img->mid[fi]=0.5*(img->start[fi]+img->end[fi]);
    img->decayCorrFactor[fi]=1.0;
  }
  for(zi=0; zi<dim_z; zi++) {
    f+=(float)(1000*zi);
    for(yi=0; yi<dim_y; yi++) {
      f+=(float)(10*yi);
      for(xi=0; xi<dim_x; xi++) {
        f+=(float)(xi);
        for(fi=0; fi<dim_t; fi++) {
          img->m[zi][yi][xi][fi]=f*(float)(1+fi);
        }
      }
    }
  }
  img->unit=IMGUNIT_KBQ_PER_ML;

  return(0);
}
/******************************************************************************/
/*!
 * Create POLARMAP IMG for testing purposis.
 *
 * @param img 			Pointer to an initiated but not allocated IMG
 * @param num_rings		Number of polar map rings
 * @param sectors_per_ring      Sectors per ring
 * @param ring_position		Ring positions
 * @param ring_angle		Ring angles
 * @param start_angle		Polar map start angle for rings, as defined in ECAT 7 header
 * @returns 0 if IMG could be created, or nonzero in case of error.
 */
int test_create_polarmap(IMG *img, int num_rings, int *sectors_per_ring, float *ring_position,
			 short int *ring_angle, short int start_angle) {
  int si, fi, xi, dim_x, dim_t=1;
  float f=1.0;

  if(img==NULL || img->status!=IMG_STATUS_INITIALIZED) return(1);
  for(si=dim_x=0; si<num_rings; si++) dim_x+=sectors_per_ring[si];
  if(imgAllocate(img, 1, 1, dim_x, dim_t)){
    printf("    \nError in image allocation: imgAllocate() @ img.c\n");
    return(1);
  }
  img->dimt=dim_t; img->dimx=dim_x; img->dimy=1; img->dimz=1;
  img->type = IMG_TYPE_POLARMAP;
  for(fi=0; fi<dim_t; fi++) {
    if(fi==0) img->start[fi]=0.0; else img->start[fi]=img->end[fi-1];
    img->end[fi]=(fi+1)*60.0;
    img->mid[fi]=0.5*(img->start[fi]+img->end[fi]);
  }
  for(xi=0; xi<dim_x; xi++) {
    f+=(float)(xi);
    for(fi=0; fi<dim_t; fi++) {
      img->m[0][0][xi][fi]=f*(float)(1+fi);
    }
  }
  img->unit=IMGUNIT_KBQ_PER_ML;
  img->polarmap_num_rings=num_rings;
  for(si=0; si<num_rings; si++) {
    img->polarmap_sectors_per_ring[si]=sectors_per_ring[si];
    img->polarmap_ring_position[si]=ring_position[si];
    img->polarmap_ring_angle[si]=ring_angle[si];
  }
  img->polarmap_start_angle=start_angle;

  return(0);
}
/******************************************************************************/

/*!
 * Test bench for IO functions. Tests include: <BR>
 * ECAT7 image write and read<BR>
 * ECAT7 image values write and read<BR>
 * ECAT6 image volume write and read<BR>
 * Analyze image volume write and read<BR>
 *
 * @param img related image stucture
 * @return 0 in case of success<BR>
 * 11 = write error<BR>
 * 110 = img max is not correct<BR>
 * 111 = img abs max is not correct<BR>
 * 112 = img min is not correct<BR>
 * 113 = img frame min or max is not correct<BR>
 * 12 = read error<BR>
 * 13 = write error<BR>
 * 14 = read error <BR>
 * 21 = write error<BR>
 * 22 = read error<BR>
 * 31 = write error<BR>
 * 32 = read error<BR>
 * 33 = write error<BR>
 * 34 = read error<BR>
 */
int test_img_io(IMG *img) {
  char *fname="test_img_io", temp[FILENAME_MAX];
  int ret;
  IMG img2;
  float minvalue = 0.0f;
  float maxvalue = 0.0f;
  float maxvalue_old = 0.0f;
  float maxvalue_abs_old = 0.0f;
  float minvalue_old = 0.0f;
  float minvalue_frame_old = 0.0f;
  float maxvalue_frame_old = 0.0f;

  printf("\n Testing IMG I/O functions\n");
  img->type=IMG_TYPE_IMAGE;
  /*IMG_TEST=2;*/
  imgInit(&img2);
  printf("\tfname := %s\n", fname);

  if(imgMax(img, &maxvalue_old)) return(110);
  if(imgAbsMax(img, &maxvalue_abs_old)) return(111);
  if(imgMinMax(img, &minvalue_old, &maxvalue)) return(112);
  if(imgFrameMinMax(img, 1, &minvalue_frame_old, &maxvalue_frame_old)) return(113);

  /* ECAT7 image write and read */
  img->_fileFormat=IMG_E7;
  ret=imgWrite(fname, img); if(ret) return(11);
  ret=imgRead(fname, &img2);
  if(ret) return(12);
  remove(fname); imgEmpty(&img2);
  img->_fileFormat=IMG_E7_2D;
  ret=imgWrite(fname, img); if(ret) return(13);
  ret=imgRead(fname, &img2); if(ret) return(14);
  remove(fname); imgEmpty(&img2);

  /* ECAT7 image values write and read testing */
  if(imgMax(img, &maxvalue)) return(110);
  if(maxvalue != maxvalue_old) return(110);
  if(imgAbsMax(img, &maxvalue)) return(111);
  if(maxvalue != maxvalue_abs_old) return(111);
  if(imgMinMax(img, &minvalue, &maxvalue)) return(112);
  if(minvalue != minvalue_old) return(112);
  if(imgFrameMinMax(img, 1, &minvalue, &maxvalue)) return(113);
  if(minvalue != minvalue_frame_old) return(113);
  if(maxvalue != maxvalue_frame_old) return(113);

  /* ECAT6 image volume write and read */
  img->_fileFormat=IMG_E63;
  ret=imgWrite(fname, img); if(ret) return(21);
  ret=imgRead(fname, &img2); if(ret) return(22);
  remove(fname); imgEmpty(&img2);

  /* Analyze image volume write and read */
  img->_fileFormat=IMG_ANA; /*ANALYZE_TEST=IMG_TEST=4;*/
  ret=imgWrite(fname, img); if(ret) return(31);
  ret=imgRead(fname, &img2); if(ret) return(32);
  sprintf(temp, "%s.hdr", fname); remove(temp);
  sprintf(temp, "%s.img", fname); remove(temp);
  imgEmpty(&img2);
  img->_fileFormat=IMG_ANA_L;
  ret=imgWrite(fname, img); if(ret) return(33);
  ret=imgRead(fname, &img2); if(ret) return(34);
  sprintf(temp, "%s.hdr", fname); remove(temp);
  sprintf(temp, "%s.img", fname); remove(temp);
  imgEmpty(&img2);
  return(0);
}
/******************************************************************************/

/*!
 * Testbench for polar map images
 *
 * @param img img structure for testing
 * @return 0 if ok
 */
int test_polarmap_io(IMG *img) {
  char *fname="test_polarmap_io";
  int ret;
  IMG img2;

  printf("\n Testing POLARMAP IMG I/O functions\n");
  img->type=IMG_TYPE_POLARMAP;
  /*IMG_TEST=2;*/
  imgInit(&img2);
  printf("\tfname := %s\n", fname); 

  /* ECAT7 image write and read */
  img->_fileFormat=IMG_POLARMAP;
  ret=imgWrite(fname, img); if(ret) return(11);
  ret=imgRead(fname, &img2); if(ret) return(12);
  remove(fname); imgEmpty(&img2);

  return(0);
}

/******************************************************************************/

/*!
 * Testbench for module tests.
 *
 * @param img img structure for testing
 * @return 0 if ok
 */
int test_modules(IMG *img) {
  	char test_str[256] = {0};
	int i = 0;
#ifndef __STRICT_ANSI__
        ECAT7_mainheader h;
  	struct tm *st;
	time_t lt;
#endif

	/* testing because of change into ANSI */
	#define TEST_WITH_FILENAME(filename,desired_length,error_value) \
	sprintf(test_str,filename); \
	anaRemoveFNameExtension(test_str); \
	i = 0; \
	while(test_str[i] != 0) i++; \
	if(i != desired_length) return(error_value); \
	memset(test_str,0,256); \
	\

        TEST_WITH_FILENAME("filename.img",8,1)
	TEST_WITH_FILENAME("filename.hdr",8,2)
	TEST_WITH_FILENAME("filename.sif",8,3)
	TEST_WITH_FILENAME("filename.img.sif",12,4)
	TEST_WITH_FILENAME("filename.img.hdr",12,5)
	TEST_WITH_FILENAME("filename.dat",8,6)
	TEST_WITH_FILENAME("filename",8,7)
        TEST_WITH_FILENAME("filename.IMG",8,8)
	TEST_WITH_FILENAME("filename.HDR",8,9)
	TEST_WITH_FILENAME("filename.SIF",8,10)
	TEST_WITH_FILENAME("filename.IMG.SIF",12,11)
	TEST_WITH_FILENAME("filename.IMG.HDR",12,12)
	TEST_WITH_FILENAME("filename.DAT",8,13)
	TEST_WITH_FILENAME("filename",8,14)
	TEST_WITH_FILENAME("filename.",8,15)

	#define TEST_WITH_FORMAT(filename,desired_format,error_value) \
	img->_fileFormat = IMG_UNKNOWN; \
        sprintf(test_str,filename); \
	imgFormatFromFName(img, test_str); \
	if(img->_fileFormat != desired_format) return(error_value); \
	memset(test_str,0,256); \
	\

	TEST_WITH_FORMAT("filename.img",IMG_E63,16)
	TEST_WITH_FORMAT("filename.scn",IMG_E63,17)
	TEST_WITH_FORMAT("filename.nrm",IMG_E63,18)
	TEST_WITH_FORMAT("filename.atn",IMG_E63,19)
	TEST_WITH_FORMAT("filename.hdr",IMG_ANA,20)
	TEST_WITH_FORMAT("filename.img.hdr",IMG_ANA,21)
	TEST_WITH_FORMAT("filename.polmap",IMG_POLARMAP,22)
	TEST_WITH_FORMAT("filename.img.polmap",IMG_POLARMAP,23)
	TEST_WITH_FORMAT("filename",IMG_ANA,24)
	TEST_WITH_FORMAT("filename.v",IMG_E7,25)
	TEST_WITH_FORMAT("filename.dat",IMG_E7,26)
	img->_fileFormat = -1;
	sprintf(test_str,"filename.dat"); \
        imgFormatFromFName(img, test_str);
        if(img->_fileFormat != IMG_E7) return(27);

#ifndef __STRICT_ANSI__
	/* test that time converts properly should be 1.1.1970 00:00:00
	 *
	 * This test should be run with different timezone settings at
	 * operating system
	 */
	h.scan_start_time = 0;
	imgGetEcat7MHeader(img, &h);
        lt = (time_t)img->scanStart;
        st = localtime(&lt);
	if(st->tm_hour != 0) {
		printf("hour:%d\n",st->tm_hour);
		return(28);
	}
	/* test that value is 1.2.1999 12:23:35 stays as it was */
        st->tm_year = 99;
	st->tm_mon = 2;
	st->tm_mday = 1;
	st->tm_hour = 12;
	st->tm_min = 23;
	st->tm_sec = 35;
	lt = mktime(st);
	h.scan_start_time = lt;
        imgGetEcat7MHeader(img, &h);
        lt = (time_t)img->scanStart;
        st = localtime(&lt);
	if(st->tm_year != 99 ||	st->tm_mon  !=  2 ||
	   st->tm_mday !=  1 ||	st->tm_hour != 12 ||
	   st->tm_min  != 23 || st->tm_sec  != 35) {
		return(29);
	}
#endif
  	return 0;
}

/******************************************************************************/
/*!
 * Function for printing the usage information of the libtpcimgio test program.
 */
void print_usage(){

  libtpcimgio_print_build(stdout);

  puts("\n"
       "   Methods for testing functions in the library "LIB_NAME"\n"
       "   and for printing out associated information, such as Readme,\n"
       "   History and build information. \n"
       "\n"
       "   Usage: "LIB_NAME" [-Options]\n"
       "\n"
       "   Options:\n"
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
       "       libtpcimgio.c.\n"
       "   -v\n"
       "       Run in verbose mode.\n"
       "\n"
       "   E.g.: "LIB_NAME" -h\n"
       "         "LIB_NAME" -v -f 1 \n"
       "         "LIB_NAME" -v -f ${IMGTRANSFORM} -z 4 \n"
       "\n");

  fflush(stdout);
}
