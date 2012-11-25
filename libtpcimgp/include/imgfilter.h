/*****************************************************************************
 gfilt.h (c) 2006,2009 by Turku PET Centre

  Header for Gaussian filter.

  Version:
  2006-08-08 Kaisa Sederholm
  2009-12-16 VO
    Added define for mallocMatrix().
  2009-12-29 VO
    Added functions imgFillGaussKernel(), imgConvolute2D(), and
    imgGaussianFilter().


******************************************************************************/

#ifndef _GFILT_H
#define _GFILT_H
/*****************************************************************************/
#include <img.h>
/*****************************************************************************/
extern int imgFillGaussKernel(
  float **kernel, float stdev, int size
);
extern int imgConvolute2D(
  float ***data, float **buffer, int frame, int width, int height,
  float **kernel, int size, int border, int verbose, char *errmsg
); 
extern int imgGaussianFilter(
  IMG *img, float gauss_sd, int size, int border, int verbose, char *errmsg 
);
/*****************************************************************************/
extern float **mallocMatrix(float w, float h);
extern float **imgGaussKernel(int size);
extern void imgFreeKernel(float **kernel, int size);
extern void imgConvoluteData(float ***data, float **buffer, int frame, 
  int width, int height, float **kernel, int size);
extern int imgConvolute(IMG *img, int frame, int plane, float **kernel, int size);
/*****************************************************************************/
#endif /* _GFILT_H */

