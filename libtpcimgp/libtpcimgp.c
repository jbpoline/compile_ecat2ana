/**
  Copyright (c) 2004-2007,2012 by Turku PET Centre
   
  File: libtpcimgp.c
   
  Purpose: methods for testing functions in the library libtpcimgp,
  and for printing out libtpcimgp information, such as Readme,
  History and build information. 
   
  Version:
  2005-02-22 1.0.0 Jarkko Johansson
  2005-04-26 1.1.0 Jarkko Johansson
  2005-06-09 1.2.0 Kaisa Sederholm
  2006-06-19 1.2.1 KS
  2006-08-08 1.3.0 KS
  2006-10-30 1.3.1 VO
  2007-02-02 1.3.2 VO
  2012-10-16 VO
  2012-11-07 VO
   
*/
/*****************************************************************************/

/** Library name. */
#define LIB_NAME "libtpcimgp"

/** Serial numbers for function names. */
#define ALLTESTS 0
#define IMGARITHM 1
#define IMGTRANSFORM 2
#define IMGFILTER 3

/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
/*****************************************************************************/
#include "img.h"
#include "imgfile.h"
/*****************************************************************************/
#include "include/imgproc.h"
/*****************************************************************************/
#define MAXVAL 100000;
/*****************************************************************************/
#define ARITHMERROR 100
#define TRANSFORMERROR 200
/*****************************************************************************/
// Test function declarations:
/**
   Function for testing library functions related to image arithmetics.
*/
int test_imgArithm(IMG *img1, IMG *img2);

/**
   Function for testing library functions related to image transforms.
*/
int test_imgTransform(IMG *img1, IMG *img2, int zoom);

/**
 *  Function for testing library functions related to image transforms.
 *  imgGaussKernel,  imgFreeKernel, imgConvolute
 *
 * @returns 0 if testing was successfull
 */
/*int test_imgFilter() {
	//test image structure
	IMG img;
	IMG smoothed_img;
	//test reference image structure
	IMG ref_img;
	//image filename
	char imgfile[FILENAME_MAX] = "test/static.v";
	//command line
	char cmd[128] = {0};
	//return value
	int ret = 0;
	//gauss kernel in matrix
        float **gauss;
	int x = 0;
	int y = 0;
	int z = 0;
	int t = 0;
	float min;
	float max;
	float mean;
	float var;
	float diff;
	float size=5.0;

        //init images
  	imgInit(&img);
  	imgInit(&smoothed_img);
  	imgInit(&ref_img);
  	fprintf(stdout, "reading image %s\n", imgfile);
  	ret = imgRead(imgfile, &img);
  	if(ret) {
		fprintf(stderr,"failed to read %s\n", imgfile);
		return ret;
	}
	imgAllocate(&smoothed_img,img.dimz,img.dimy,img.dimx,img.dimt);

	//test smoothing
	for(int i = 0; i < 10; i++) {
		//smooth using library
    		gauss = imgGaussKernel(size);
    		if(!gauss) {
        		perror("imgGaussKernel failed");
        		return 1;
    		}
    		for(int x = 0; x < img.dimx; x++) {
    		for(int y = 0; y < img.dimy; y++) {
    		for(int z = 0; z < img.dimz; z++) {
    		for(int t = 0; t < img.dimt; t++) {
    			smoothed_img.m[z][y][x][t] = img.m[z][y][x][t];
    		}
		}
		}
		}
	    	if(imgConvolute(&smoothed_img, 0, 0, gauss, size)) {
	        	perror("imgConvolute failed");
	        	return 1;
	    	}
	    	imgFreeKernel(gauss,size);

		//smooth using gsmooth program
		sprintf(cmd,"test/gsmooth static.v %d",i);
		system(cmd);
		sprintf("test/static_%dmm.v",i);
  		ret = imgRead(imgfile, &ref_img);
  		if(ret) {
			fprintf(stderr,"failed to read %s\n", imgfile);
			return ret;
		}
		//compare results
		mean = 0;
		min = MAX_FLOAT;
		max = MIN_FLOAT;
    		for(int x = 0; x < img.dimx; x++) {
    		for(int y = 0; y < img.dimy; y++) {
    		for(int z = 0; z < img.dimz; z++) {
    		for(int t = 0; t < img.dimt; t++) {
    			diff = smoothed_img->m[z][y][x][t] - ref_img->m[z][y][x][t];
			mean += diff;
			if(diff < min) min = diff;
			if(diff > max) max = diff;
    		}
		}
		}
		}
    		mean /= img.dimx*img.dimy*img.dimz*img.dimt;
    		var = 0;
    		for(int x = 0; x < img.dimx; x++) {
    		for(int y = 0; y < img.dimy; y++) {
    		for(int z = 0; z < img.dimz; z++) {
    		for(int t = 0; t < img.dimt; t++) {
    			diff = smoothed_img->m[z][y][x][t] - ref_img->m[z][y][x][t];
			var += powf(diff-mean,2.0f);
    		}
		}
		}
		}
    		var /= img.dimx*img.dimy*img.dimz*img.dimt;
    		printf("difference: mean:%f var:%f min:%f max:%f\n",mean,var,min,max);
	}
	//free memory
        imgEmpty(&img);
        imgEmpty(&smoothed_img);
        imgEmpty(&ref_img);
}*/

// Functions for initializing image data:

/**
   Function for drawing a cros on the center of the image region,
   of width 2 pixels and value 1.
 */
void drawCross(IMG *img);

/**
   Function for drawing an unsymmetric cross for testing image transformation
   functions.
 */
void drawUnSymmetricCross(IMG *img);


/** 
    Function for printing the usage information of the libtpcimgp test program.
*/
void print_usage();

// Verbose mode switch:
int VERBOSE = 0;

int main (int argc, char *argv[ ])
{
    int c;
    int errflg=0, functionflag=-1, exit_code=0, plane;;
    int  planes1, rows1, columns1, frames1, planes2, rows2, columns2, frames2, zoom=2;
    IMG img1, img2;
    //    char filename1[100], filename2[100];
    extern char *optarg;
    extern int optind, optopt;
    
    while ((c = getopt(argc, argv, "vVbBrRhHf:F:z:Z:")) != -1) {
      switch (c) {

      case 'V':
      case 'v':
	// Set verbose flag on:
	VERBOSE = 1;
	break;

      case 'B':
      case 'b':
	// Print out the build information:
	libtpcimgp_print_build(stdout);
	return(0);

      case 'r':
	// Print out the Readme message:
	libtpcimgp_print_readme(stdout);
	return(0);
      case 'R':
	// Print out the Readme message with Doxygen tags:
	libtpcimgp_print_dreadme(stdout);
	return(0);

      case 'h':
	// Print out the History message:
	libtpcimgp_print_history(stdout);
	return(0);
      case 'H':
	// Print out the History message with Doxygen tags:
	libtpcimgp_print_dhistory(stdout);
	return(0);

      case 'f':
      case 'F':
	// Set function serial number to be tested:
	functionflag = atoi(optarg);
	break;

      case 'z':
      case 'Z':
	// Set zoom factor:
	zoom = atoi(optarg);
	break;

      case ':': // -f or -z without argument
	fprintf(stderr,"Option -%c requires an argument\n", optopt);
	errflg++;
	break;
      case '?':
	fprintf(stderr,"Unrecognised option: -%c\n", optopt);
	errflg++;
      }
    }// End parsing command line options...

    if(errflg) {
      print_usage();
      return(2);
    }

    if(functionflag<0){
      print_usage();
      return(0);
    }

    libtpcimgp_print_build(stdout);

    //// Initialize some images to test with:
    imgInit(&img1); imgInit(&img2);
    // Set dimensions for test images:
    planes1 = 1;
    rows1 = 32;
    columns1 = 32;
    frames1 = 1;
    planes2 = planes1;
    rows2 = rows1*zoom;
    columns2 = columns1*zoom;
    frames2 = frames1;

    if(imgAllocate(&img1,planes1,rows1,columns1,frames1)){
      printf("    \nError in image allocation: imgAllocate() @ img.c\n");
      return(1);
    }
    if(imgAllocate(&img2,planes2,rows2,columns2,frames2)){
      printf("\n    Error in image allocation: imgAllocate() @ img.c\n");
      return(1);
    }
    
    for(plane = 0; plane < img1.dimz; plane++)
      img1.planeNumber[plane] = img2.planeNumber[plane] = plane;
    
    img1.sizex = img1.sizey = img1.sizez = img2.sizex = img2.sizey = img2.sizez = 1;
    img1.type = img2.type = IMG_TYPE_IMAGE;


    // Choose function(s) to test:
    switch(functionflag) {
    case ALLTESTS:
      exit_code = test_imgArithm(&img1,&img1);
      if(exit_code) exit(exit_code);
      exit_code = test_imgTransform(&img1,&img2,zoom);
      if(exit_code) exit(exit_code);
    case IMGARITHM:
      exit_code = test_imgArithm(&img1,&img1);
      break;
    case IMGTRANSFORM:
      exit_code = test_imgTransform(&img1,&img2,zoom);
      break;
    /*case IMGFILTER:
      exit_code = test_imgFilter();
      break;*/
    default:
      break;
    }
    
    //imgWrite(filename1,&img1);
    //imgWrite(filename2,&img2);
    imgEmpty(&img1); imgEmpty(&img2);
    
    exit(exit_code);
}

int test_imgArithm(IMG *img1, IMG *img2){
 
  int error_code = 0, plane, row, column, frame;
  float operand;
  char operation = 0;
  float ****cross_mask;

  printf("\n    Testing image arithmetics functions...\n");

  // Initialize the input image with an unsymmetric cross:
    drawUnSymmetricCross(img1);
    if(VERBOSE) printf("\n    Initial cross drawn. \n");
    
  // Create a mask for checking the results:
    cross_mask = malloc(img1->dimz * sizeof(float***));
    for(plane = 0; plane < img1->dimz; plane++){
      cross_mask[plane] = malloc(img1->dimy * sizeof(float**));
      for(row = 0; row < img1->dimy; row++){
	cross_mask[plane][row] = malloc(img1->dimx * sizeof(float*));
	for(column = 0; column < img1->dimx; column++)
	  cross_mask[plane][row][column] = malloc(img1->dimt*sizeof(float));
      }
    }
    // Copy the image buffer to mask buffer:
    for(plane = 0; plane < img1->dimz; plane++)
      for(row = 0; row < img1->dimy; row++)
	for(column = 0; column < img1->dimx; column++) 
	  for(frame = 0; frame < img1->dimt; frame++)
	    cross_mask[plane][row][column][frame] = img1->m[plane][row][column][frame];
	  
    if(VERBOSE) printf("\n    Mask created succesfully.\n");
    

  // Sum and check (expected result = 2*mask):
  operation = '+';
  error_code = imgArithm(img1,img2,operation,10000000,0);
  if(!error_code){
    for(plane = 0; plane < img1->dimz; plane++)
      for(row = 0; row < img1->dimy; row++)
	for(column = 0; column < img1->dimx; column++)
	  for(frame = 0; frame < img1->dimt; frame++){
	    if((img1->m[plane][row][column][frame] !=
                2*cross_mask[plane][row][column][frame]))
              {
		error_code = ARITHMERROR + 1;
		if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                      plane,row,column,frame,img1->m[plane][row][column][frame]);
	      }
	    }
  }
  if(error_code){
    if(VERBOSE) 
      printf("\n    Test FAILED: imgArithm() sum operation failed with error code: %i\n",
          error_code);
    return(error_code);
  }
  else{
    if(VERBOSE) 
      printf("\n    Test SUCCESFULL: imgArithm() sum operation exited with: %i\n",error_code);
  }

// Substract and check (expected result = 0):
  operation = '-';
  error_code = imgArithm(img1,img2,operation,10000000,0);
  if(!error_code){
  for(plane = 0; plane < img1->dimz; plane++)
    for(row = 0; row < img1->dimy; row++)
      for(column = 0; column < img1->dimx; column++)
	for(frame = 0; frame < img1->dimt; frame++){
	  if(img1->m[plane][row][column][frame] != 0){
	    error_code = ARITHMERROR + 2;
	    if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                    plane,row,column,frame,img1->m[plane][row][column][frame]);
	  }
	}
  }
  if(error_code){
    if(VERBOSE) 
      printf("\n    Test FAILED: imgArithm() substract operation failed with error code: %i\n",
        error_code);
    return(error_code);
  }
  else{
    if(VERBOSE) 
      printf("\n    Test SUCCESFULL: imgArithm() substract operation exited with: %i\n",
         error_code);
  }

  // Initialize the input image with an unsymmetric cross:
    drawUnSymmetricCross(img1);

  // Multiply and check (expected result = mask*mask):
  operation = 'x';
  error_code = imgArithm(img1,img1,operation,10000000,0);
  if(!error_code){
    for(plane = 0; plane < img1->dimz; plane++)
      for(row = 0; row < img1->dimy; row++)
	for(column = 0; column < img1->dimx; column++)
	  for(frame = 0; frame < img1->dimt; frame++){
	      if(img1->m[plane][row][column][frame] !=
                 cross_mask[plane][row][column][frame] * cross_mask[plane][row][column][frame])
              {
		error_code = ARITHMERROR + 3;
		if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                     plane,row,column,frame,img1->m[plane][row][column][frame]);
	      }
	  }
  }
  if(error_code){
    if(VERBOSE) 
      printf("\n    Test FAILED: imgArithm() multiplication operation failed with error code: %i\n",
         error_code);
    return(error_code);
  }
  else{
    if(VERBOSE) 
      printf("\n    Test SUCCESFULL: imgArithm() multiplication operation exited with: %i\n",
         error_code);
  }
  
  // Devide and check (expected result = 0 or 1):
  if(!error_code){
    operation = '/';
    error_code = imgArithm(img1,img1,operation,10000000,0);
    for(plane = 0; plane < img1->dimz; plane++)
      for(row = 0; row < img1->dimy; row++)
	for(column = 0; column < img1->dimx; column++)
	  for(frame = 0; frame < img1->dimt; frame++){
	    if(cross_mask[plane][row][column][frame] != 0){
	      if(img1->m[plane][row][column][frame] != 1){
		error_code = ARITHMERROR + 4;
		if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                     plane,row,column,frame,img1->m[plane][row][column][frame]);
	      }
	    }
	    else{
	      if(img1->m[plane][row][column][frame] != 0){
		error_code = ARITHMERROR + 5; // Error in deviding with zero.
		if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                     plane,row,column,frame,img1->m[plane][row][column][frame]);
	      }
	    }
	  }
  }
  if(error_code){
    if(VERBOSE) 
      printf("\n    Test FAILED: imgArithm() division operation failed with error code: %i\n",
        error_code);
    return(error_code);
  }
  else{
    if(VERBOSE) 
      printf("\n    Test SUCCESFULL: imgArithm() division operation exited with: %i\n",
          error_code);
  }
  
  //// Check constant operations:

  // Initialize the input image with an unsymmetric cross:
    drawUnSymmetricCross(img1);

  // Add one and check (expected result = mask + 1):
  operation = '+';
  operand = 1.;
  error_code = imgArithmConst(img1,operand,operation,10000000,0);
  if(!error_code){
    for(plane = 0; plane < img1->dimz; plane++)
      for(row = 0; row < img1->dimy; row++)
	for(column = 0; column < img1->dimx; column++)
	  for(frame = 0; frame < img1->dimt; frame++){
	      if((img1->m[plane][row][column][frame] !=
                  cross_mask[plane][row][column][frame] + 1))
              {
		error_code = ARITHMERROR + 6;
		if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                    plane,row,column,frame,img1->m[plane][row][column][frame]);
	      }
	  }
  }
  if(error_code){
    if(VERBOSE) 
      printf("\n    Test FAILED: imgArithmConst() sum operation failed with error code: %i\n",
           error_code);
    return(error_code);
  }
  else{
    if(VERBOSE) 
      printf("\n    Test SUCCESFULL: imgArithmConst() sum operation exited with: %i\n",
           error_code);
  }

  // Substract one from previous image buffer and check (expected result = mask):
  operation = '-';
  operand = 1.;
  error_code = imgArithmConst(img1,operand,operation,10000000,0);
  if(!error_code){
    for(plane = 0; plane < img1->dimz; plane++)
      for(row = 0; row < img1->dimy; row++)
	for(column = 0; column < img1->dimx; column++)
	  for(frame = 0; frame < img1->dimt; frame++){
	      if(img1->m[plane][row][column][frame] != cross_mask[plane][row][column][frame]){
		error_code = ARITHMERROR + 7;
		if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                     plane,row,column,frame,img1->m[plane][row][column][frame]);
	      }
	  }
  }
  if(error_code){
    if(VERBOSE) 
      printf("\n    Test FAILED: imgArithmConst() substract operation failed with error code: %i\n",
           error_code);
    return(error_code);
  }
  else{
    if(VERBOSE) 
      printf("\n    Test SUCCESFULL: imgArithmConst() substract operation exited with: %i\n",
            error_code); 
  }

  // Divide by 2 and check (expected result = mask / 2):
  operation = '/';
  operand = 2.;
  error_code = imgArithmConst(img1,operand,operation,10000000,0);
  if(error_code == 0){
    for(plane = 0; plane < img1->dimz; plane++)
      for(row = 0; row < img1->dimy; row++)
	for(column = 0; column < img1->dimx; column++)
	  for(frame = 0; frame < img1->dimt; frame++){
	    if(img1->m[plane][row][column][frame] != (cross_mask[plane][row][column][frame] / 2)){
		error_code = ARITHMERROR + 8;
		if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                     plane,row,column,frame,img1->m[plane][row][column][frame]);
	    }
	  }
  }
  if(error_code){
    if(VERBOSE) 
      printf("\n    Test FAILED: imgArithmConst() division operation failed with error code: %i\n",
           error_code);
    return(error_code);
  }
  else{
    if(VERBOSE) 
      printf("\n    Test SUCCESFULL: imgArithmConst() division operation exited with: %i\n",
            error_code); 
  }

  // Multiply by 2 and check (expected result = mask):
  operation = 'x';
  operand = 2.;
  error_code = imgArithmConst(img1,operand,operation,10000000,0);
  if(!error_code){
    for(plane = 0; plane < img1->dimz; plane++)
      for(row = 0; row < img1->dimy; row++)
	for(column = 0; column < img1->dimx; column++)
	  for(frame = 0; frame < img1->dimt; frame++){
	    if(img1->m[plane][row][column][frame] != cross_mask[plane][row][column][frame]){
	      error_code = ARITHMERROR + 9;
	      if(VERBOSE) printf("    m[%i][%i][%i][%i] = %f\n",
                     plane,row,column,frame,img1->m[plane][row][column][frame]);
	    }
	  }
  }
  if(error_code){
    if(VERBOSE) 
      printf("\n    Test FAILED: imgArithmConst() multiplication operation failed with error code: %i\n",
           error_code);
    return(error_code);
  }
  else{
    if(VERBOSE) 
      printf("\n    Test SUCCESFULL: imgArithmConst() multplication operation exited with: %i\n",
           error_code); 
  }

  printf("\n    Test SUCCESFULL: imgArithm and imgArithmConst() operations exited with: %i\n",
            error_code); 

  exit(error_code);
}

int test_imgTransform(IMG *img1, IMG *img2, int zoom){

  int error_code = 0;
  int plane, row, column, frame, r_limit, c_limit, cross_width;

  printf("\n    Testing image transformation functions...\n");

  // Initialize the input image with an unsymmetric cross:
  drawUnSymmetricCross(img1);

  // Scale the first given image and write to second:
  imgScale(img1,img2,zoom,0);

  // Check the scaled cross:
  cross_width = 2*zoom;
  r_limit = (img2->dimy - cross_width)/2;
  c_limit = (img2->dimx - cross_width)/2;
  for(plane = 0; plane < img2->dimz; plane++)
    for(row = 0; row < img2->dimy; row++)
      for(column = 0; column < img2->dimx; column++)
	for(frame = 0; frame < img2->dimt; frame++){
	  if((row >= r_limit) && (row <= r_limit + (cross_width - 1))){
	    if(column < c_limit) // left 
	      if(img2->m[plane][row][column][frame] != 1)
		error_code = TRANSFORMERROR + 1;
	    if(column > c_limit + (cross_width - 1)) // right
	      if(img2->m[plane][row][column][frame] != 3)
		error_code = TRANSFORMERROR + 2;
	    if((column >= c_limit) && (column <= c_limit + (cross_width - 1))) //center
	      if(img2->m[plane][row][column][frame] != 5)
		error_code = TRANSFORMERROR + 3;
	  }
	  else{
	    if((column >= c_limit) && (column <= c_limit + (cross_width - 1))){	   
	      if(row < r_limit) // top
		if(img2->m[plane][row][column][frame] != 4)
		  error_code = TRANSFORMERROR + 4;
	      if(row > r_limit) // bottom
		if(img2->m[plane][row][column][frame] != 2)
		  error_code = TRANSFORMERROR + 5;	      
	    }
	    else{
	      if(img2->m[plane][row][column][frame] != 0)
		error_code = TRANSFORMERROR + 6;	  
	    }
	  }
	}
  
  if(error_code)
    printf("\n    Test FAILED: imgScale() by factor %i failed with error code: %i\n",
         zoom,error_code);
  else
    printf("\n    Test SUCCESFULL: imgScale() by factor %i exited with: %i\n",
         zoom,error_code); 

  return(error_code);
}

void drawCross(IMG *img){

  int plane, row, column, frame, r_limit, c_limit;

  // Draw a symmetric cross on all planes and frames in the image:
  r_limit = (img->dimy - 2)/2;
  c_limit = (img->dimx - 2)/2;
  for(plane = 0; plane < img->dimz; plane++)
    for(row = 0; row < img->dimy; row++)
      for(column = 0; column < img->dimx; column++)
	for(frame = 0; frame < img->dimt; frame++){
	  if((row >= r_limit) && (row <= r_limit + 1))
	    img->m[plane][row][column][frame] = 1;
	  if((column >= c_limit) && (column <= c_limit + 1)\
	     && !((row >= r_limit) && (row <= r_limit + 1)))
	    img->m[plane][row][column][frame] = 1;
	}

}

void drawUnSymmetricCross(IMG *img){


  int plane, row, column, frame, r_limit, c_limit;

  // Draw an unsymmetric cross on all planes and frames in the image:
  r_limit = (img->dimy - 2)/2;
  c_limit = (img->dimx - 2)/2;
  for(plane = 0; plane < img->dimz; plane++)
    for(row = 0; row < img->dimy; row++)
      for(column = 0; column < img->dimx; column++)
	for(frame = 0; frame < img->dimt; frame++){
	  if((row >= r_limit) && (row <= r_limit + 1)){
	    if(column < c_limit) // left 
	      img->m[plane][row][column][frame] = 1;
	    if(column > c_limit + 1) // right
	      img->m[plane][row][column][frame] = 3;
	    if((column >= c_limit) && (column <= c_limit + 1)) //center
	      img->m[plane][row][column][frame] = 5;
	  }
	  else{
	    if((column >= c_limit) && (column <= c_limit + 1)){	     
	      if(row < r_limit) // top
		img->m[plane][row][column][frame] = 4;
	      if(row > r_limit) // bottom
		img->m[plane][row][column][frame] = 2;

	    }
	    else{
	      img->m[plane][row][column][frame] = 0;
	    }
	  }
	}

}
 


void print_usage(){

  libtpcimgp_print_build(stdout);

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
  "   -b\n"
  "       Print out the "LIB_NAME" build information.\n"
  "   -f <function serial number>\n"
  "       Runs test procedures for the functions corresponding to given\n"
  "       'function serial number'. Serial numbers are defined in file\n"
  "       "LIB_NAME".c; enter 0 to execute them all.\n"
  "   -z<zoom factor>\n"
  "       Zoom factor utilised in image scaling test functions.\n"
  "   -v\n"
  "       Run in verbose mode.\n"
  "\n"
  "   E.g.: "LIB_NAME" -h\n"
  "         "LIB_NAME" -v -f 1 \n"
  "         "LIB_NAME" -v -f ${IMGTRANSFORM} -z 4 \n"
  "\n"
  );

  fflush(stdout);
}
/******************************************************************************/
