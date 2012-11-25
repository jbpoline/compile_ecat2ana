/******************************************************************************
  Copyright (c) 2005,2006,2009,2012 by Turku PET Centre

  File: imgfilter.c
  Purpose: Gaussian IMG filter.

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
  2005-08-08 Calle Laakkonen
      First created.
  2006-08-08 Kaisa Sederholm
  2009-12-29 Vesa Oikonen
      Added functions imgFillGaussKernel(), imgConvolute2D(), and
      imgGaussianFilter(), which are all used in Gaussian filtering program
      imgfiltg and are tested there against imsmooth() in Octavia.
      Previous functions are not needed, and could be removed.
  2012-04-22 VO
      Fixed memory allocation bug in imgGaussianFilter() which appeared
      on a 64-bit Windows system.
  2012-11-08 VO
      Code cleaning, not affecting compiled code.
  2012-11-13 VO
      imgGaussianFilter() accepts 0 as kernel size, using default in that case.


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
/*****************************************************************************/
#include "img.h"
#include "include/imgfilter.h"
/*****************************************************************************/

/*****************************************************************************/
/** Make a gaussian convolution kernel.
\return Returns 0 when successful, otherwise <>0.
*/
int imgFillGaussKernel(
  /** Gaussian convolution kernel matrix[size][size], filled here */
  float **kernel,
  /** Gaussian S.D. in pixels (decimals are ok) */
  float stdev,
  /** Kernel dimension */
  int size
) {
  int x, y;
  float mx, my, v, ksum=0.0;

  if(kernel==NULL || size<3) return 1;
  if(stdev<0.0) return 2;
  if((size%2)==0) return 3; // size must be odd number

  v=stdev*stdev;
  for(x=0; x<size; x++) {
    mx = x - size/2; // note: ints on purpose
    for(y=0; y<size; y++) {
      my = y - size/2; // note: ints on purpose
      if(stdev>0) {
#if(0)  // Gaussian in the middle of pixel is calculated
        kernel[y][x] =
          (1.0/(2.0*M_PI*v)) * powf(M_E,-((mx*mx)+(my*my))/(2.0*v));
#else  // Pixel area is considered
        kernel[y][x] = 0.25
              *(erff((mx-0.5)/(stdev*M_SQRT2))-erff((mx+0.5)/(stdev*M_SQRT2)))
              *(erff((my-0.5)/(stdev*M_SQRT2))-erff((my+0.5)/(stdev*M_SQRT2)));
#endif
      } else {
        if(x==size/2 && y==size/2) kernel[y][x]=1.0; else kernel[y][x]=0.0;
      }
      ksum+=kernel[y][x];
    }
  }

  /* Ensure quantitativity with normalization;
     divide each kernel value by their sum */
  v=1.0/ksum;
  for(x=0; x<size; x++) for(y=0; y<size; y++) kernel[y][x]*=v;

  return 0;
}
/*****************************************************************************/

/*****************************************************************************/
/** Perform a convolution operation on one image matrix.
 *  Note that this may be a very slow process if image and kernel size is
 *  large: width*height*size*size multiplications will be done.
\return Returns 0 when successful, otherwise <>0.
 */
int imgConvolute2D(
  /** Image matrix data[height][width][frame] */
  float ***data,
  /** Temporary preallocated memory buffer[size][width+size-1] */
  float **buffer,
  /** frame [0..dimt] of data matrix which is processed */
  int frame,
  /** Width of image matrix (dimx) */
  int width,
  /** Height of image matrix (dimy) */
  int height,
  /** Convolution kernel[size][size] matrix */
  float **kernel,
  /** Convolution kernel size; must be an odd number and >=3 */
  int size,
  /** Fill borders with zero (0) or with closest image pixel (<>0) */
  int border,
  /** Verbose level; if zero, then only warnings are printed into stderr */
  int verbose,
  /** Pointer to error message, at least 128 characters; NULL, if not needed */
  char *errmsg 
) {
  const int size2=size/2, bufw=width+2*size2;
  int x, y, i;
  float *tmp;
  float filtered;
  int kx, ky;

  if(verbose>0) 
    printf("imgConvolute2D(***data, **buf, %d, %d, %d, **ker, %d, %d, %d)\n", 
           frame, width, height, size, border, verbose);
  /* Check input */
  if(errmsg!=NULL) strcpy(errmsg, "invalid function input");
  if(data==NULL || buffer==NULL || frame<0 || width<3 || height<3||kernel==NULL)
    return 1;
  if((size%2)==0) {
    if(errmsg!=NULL) strcpy(errmsg, "kernel size is even number");
    return 1;
  }
  if(size<3) {
    if(errmsg!=NULL) strcpy(errmsg, "kernel size must be >=3");
    return 1;
  }
  if(verbose>1) {
    printf("  size2 := %d\n", size2);
    printf("  bufw := %d\n", bufw);
  }

  /* Fill the initial buffer */
  if(verbose>2) printf("filling initial data buffer\n"); 
  if(border==0) {
    for(y=0; y<size2; y++) {
      for(x=0; x<bufw; x++) buffer[y][x]=0;
    }
    for(y=size2; y<size; y++) {
      for(x=0; x<size2; x++) buffer[y][x]=0;
      for(x=size2; x<bufw-size2; x++) buffer[y][x]=data[y-size2][x-size2][frame];
      for(x=bufw-size2; x<bufw; x++) buffer[y][x]=0;
    }
  } else {
    for(y=0; y<size2; y++) {
      for(x=0; x<size2; x++) buffer[y][x]=data[0][0][frame];
      for(x=size2; x<bufw-size2; x++) buffer[y][x]=data[0][x-size2][frame];
      for(x=bufw-size2; x<bufw; x++) buffer[y][x]=data[0][width-1][frame];
    }
    for(y=size2; y<size; y++) {
      for(x=0; x<size2; x++) buffer[y][x]=data[0][y-size2][frame];
      for(x=size2; x<bufw-size2; x++) buffer[y][x]=data[y-size2][x-size2][frame];
      for(x=bufw-size2; x<bufw; x++) buffer[y][x]=data[y-size2][width-1][frame];
    }
  }
  if(verbose>8) {
    int xi, yi;
    printf("Initial buffer for row-index 0:\n");
    for(yi=0; yi<size; yi++) {
      for(xi=0; xi<bufw; xi++) printf(" %4.0f", buffer[yi][xi]);
      printf("\n");
    }
  }

  /* Filter */
  for(y=0; y<height; y++) {
    if(verbose>2) printf("filtering image row %d\n", y+1); 

    /* Filter a row */
    for(x=0; x<width; x++) {
      filtered = 0.0;
      for(ky=0; ky<size; ky++) {
        for(kx=0; kx<size; kx++) {
          filtered += buffer[ky][x+kx] * kernel[ky][kx];
        }
      }
      data[y][x][frame] = filtered;
    }

    if(y==height-1) break; // no need for further buffer filling

    if(verbose>2) printf("filling data buffer for next image row\n"); 
    /* Shift rows in data buffer */
    tmp = buffer[0];
    for(i=1; i<size; i++) buffer[i-1]=buffer[i];
    buffer[size-1]=tmp;

    /* Get next row from image or 'guess' it */
    if(y+size2+1<height) { // still inside original image 
      if(border==0) {
        for(x=0; x<size2; x++)
          buffer[size-1][x]=0;
        for(x=size2; x<bufw-size2; x++)
          buffer[size-1][x]=data[y+size2+1][x-size2][frame];
        for(x=bufw-size2; x<bufw; x++)
          buffer[size-1][x]=0;
      } else {
        for(x=0; x<size2; x++)
          buffer[size-1][x]=data[y+size2+1][0][frame];
        for(x=size2; x<bufw-size2; x++)
          buffer[size-1][x]=data[y+size2+1][x-size2][frame];
        for(x=bufw-size2; x<bufw; x++)
          buffer[size-1][x]=data[y+size2+1][width-1][frame];
      }
    } else { // below original image
      if(border==0) {
        for(x=0; x<bufw; x++) buffer[size-1][x] = 0;
      } else {
        for(x=0; x<size2; x++)
          buffer[size-1][x]=data[height-1][0][frame];
        for(x=size2; x<bufw-size2; x++)
          buffer[size-1][x]=data[height-1][x-size2][frame];
        for(x=bufw-size2; x<bufw; x++)
          buffer[size-1][x]=data[height-1][width-1][frame];
      }
    }
    if(verbose>9) {
      int xi, yi;
      printf("Buffer for row-index %d:\n", y+1);
      for(yi=0; yi<size; yi++) {
        for(xi=0; xi<bufw; xi++) printf(" %4.0f", buffer[yi][xi]);
        printf("\n");
      }
    }
  }
  return 0;
}
/*****************************************************************************/

/*****************************************************************************/
/** Apply Gaussian filter to whole dynamic image in IMG struct.
\return If an error is encountered, function returns a non-zero value. 
        Otherwise 0 is returned.
 */
int imgGaussianFilter(
  /** Image data to be processed; data is overwritten with filtered image */
  IMG *img,
  /** Gaussian S.D. in pixels (decimals are ok) */
  float gauss_sd,
  /** Gaussian kernel size in pixels (must be odd number);
   *  typically 6*gauss_sd is sufficient; enter zero to set it automatically
   *  to 6*gauss_sd (plus 1 if necessary). */
  int size,
  /** In filtering, fill borders with zero (0) or with closest
   *  image pixel (<>0) */
  int border,
  /** Verbose level; if zero, then only warnings are printed into stderr */
  int verbose,
  /** Pointer to error message, at least 128 characters; NULL, if not needed */
  char *errmsg 
) {
  int i, j, zi, fi, ret;
  float **buffer, *bdata;
  float **gauss, *gdata;

  if(verbose>0)
    printf("imgGaussianFilter(*img, %g, %d, %d)\n", gauss_sd, size, verbose); 

  /* Check input */
  if(errmsg!=NULL) strcpy(errmsg, "invalid function input");
  if(img==NULL || img->dimz<1 || img->dimx<3 || img->dimy<3) return 1;
  if(gauss_sd<0.0) return 2; // SD=0 is accepted for basic testing
  if(size!=0 && (size<3 || (size%2)==0) ) return 2;
  if(size==0) {
    size=6*roundf(gauss_sd); if(size<3) size=3; if((size%2)==0) size++;
    if(verbose>0) printf("size := %d\n", size);
  }

  /* Allocate memory for the kernel */
  if(errmsg!=NULL) strcpy(errmsg, "cannot allocate memory for kernel");
  if(verbose>2) printf("allocating memory for the kernel\n");
  gdata=(float*)malloc( size*size*sizeof(float) );
  if(gdata==NULL) {return 4;}
  gauss=(float**)malloc(size*sizeof(float*));
  if(gauss==NULL) {free(gdata); return 4;}
  for(i=0; i<size; i++) gauss[i]=gdata+(i*size);

  /* Allocate memory for temporary buffer */
  if(errmsg!=NULL) strcpy(errmsg, "cannot allocate memory for data buffer");
  if(verbose>2) printf("allocating memory for the raw buffer\n");
  bdata=(float*)malloc( (img->dimx+size-1)*size*sizeof(float) );
  if(bdata==NULL) {free(gauss); free(gdata); return 4;}
  if(verbose>2) printf("allocating memory for the buffer\n");
  buffer=(float**)malloc(size*sizeof(float*));
  if(buffer==NULL) {free(gauss); free(gdata); free(bdata); return 4;}
  if(verbose>2) printf("preparing buffer\n");
  for(i=0; i<size; i++) buffer[i]=bdata+(i*(img->dimx+size-1));

  /* Make a gaussian convolution kernel */
  if(verbose>1) printf("calculating Gaussian convolution kernel\n");
  if(imgFillGaussKernel(gauss, gauss_sd, size) != 0) {
    if(errmsg!=NULL) strcpy(errmsg, "cannot compute Gaussian kernel");
    free(gauss); free(gdata); free(buffer); free(bdata);
    return 5;
  }
  if(verbose>4) {
    printf("Gaussian convolution kernel:\n");
    for(i=0; i<size; i++) {
      printf(" ");
      for(j=0; j<size; j++) printf(" %8.6f", gauss[i][j]);
      printf("\n");
    }
  }

  /* Convolute each image matrix */
  if(verbose>0) printf("convolution of the image data\n");
  for(zi=0; zi<img->dimz; zi++) {
    if(verbose>1) printf("  plane %d\n", zi+1);
    for(fi=0; fi<img->dimt; fi++) {
      if(verbose>1) printf("    frame %d\n", fi+1);
      ret=imgConvolute2D(img->m[zi], buffer, fi, img->dimx, img->dimy,
                         gauss, size, border, verbose, errmsg);
      if(ret!=0) {
        free(gauss); free(gdata); free(buffer); free(bdata);
        return(10+ret);
      }
    }
  }

  /* Free memory */
  free(gauss); free(gdata); free(buffer); free(bdata);

  if(verbose>1) printf("  convolution done.\n");
  return 0;
}
/*****************************************************************************/

/*****************************************************************************/
/** \internal
  \brief Allocate memory for a float matrix
  
  The matrix will be in form matrix[h][w]

  \param w width of matrix
  \param h height of matrix
  \return the allocated memory. NULL on error
  */
float **mallocMatrix(float w,float h)
{
  int y;
  float **matrix = malloc(h * sizeof(float*));
  if(!matrix) return NULL;
  for(y=0;y<h;y++) {
    /* Allocate a row */
    matrix[y] = malloc(w*sizeof(float));

    if(!matrix[y]) {
      while(y>0) free(matrix[--y]);
      free(matrix);
      return NULL;
    }
  }
  return matrix;
}
/*****************************************************************************/

/*****************************************************************************/
/** \brief Make a gaussian convolution kernel
  \param size kernel size
  \return the convolution kernel. NULL on error
*/
float **imgGaussKernel(
  int size
) {
  float **kernel, stdev, mx, my;
  int x,y;

  kernel = mallocMatrix(size,size);
  if(!kernel) return NULL;

  stdev = size/7.0;
  stdev = stdev*stdev;

  for(x=0;x<size;x++) {
    mx = x - size/2;
    for(y=0;y<size;y++) {
      my = y - size/2;
      kernel[x][y] =
       (1.0/(2*M_PI*stdev))*pow(M_E,-((mx*mx)+(my*my))/(2.0*stdev));
    }
  }
  return kernel;
}
/*****************************************************************************/

/*****************************************************************************/
/** \brief Free a convolution kernel
  \param kernel the convolution kernel
  \param size size of the kernel
  */
void imgFreeKernel(
  float **kernel,
  int size
) {
  int r;
  for(r=0;r<size;r++) free(kernel[r]);
  free(kernel);
}
/*****************************************************************************/

/*****************************************************************************/
/** \brief Perform a convolution operation on float data

\param data data in format float[y][x][frame]
\param buffer temporary buffer to use. Size must be float[size][data width+size]
\param frame frame to extract from data
\param width width of the image
\param height height of the image
\param kernel convolution kernel
\param size convolution kernel size
 */
void imgConvoluteData(
  float ***data,
  float **buffer,
  int frame,
  int width,
  int height,
  float **kernel,
  int size
) {
  const int size2 = size/2;
  int x,y;

  /* Fill the initial buffer */
  for(y=0;y<size2;y++) {
    /* Pixels above the border */
    for(x=0;x<width+size;x++) buffer[y][x] = 0;

    /* Padding for the borders */
    for(x=0;x<size2;x++) {
      buffer[y+size2][x] = 0;
      buffer[y+size2][x+width+size2] = 0;
    }

    /* Rows from the image */
    for(x=0;x<width;x++)
      buffer[y+size2][x+size2] = data[y][x][frame];
  }

  /* Filter */
  for(y=0;y<height;y++) {
    float *tmp;
    /* Filter a row */
    for(x=0;x<width;x++) {
      float filtered = 0;
      int kx,ky;
      for(kx=0;kx<size;kx++) {
        for(ky=0;ky<size;ky++) {
          filtered += buffer[ky][x+kx] * kernel[kx][ky];
        }
      }
      data[y][x][frame] = filtered;
    }

    /* Shift rows */
    tmp = buffer[size-1];
    buffer[size-1] = buffer[0];
    for(x=0;x<size-2;x++) buffer[x] = buffer[x+1];
    buffer[size-2] = tmp;

    /* Get next row from image or blank */
    if(y+size2<height) {
      for(x=0;x<width;x++) buffer[size-1][x+size2] = data[y+size2][x][frame];
    } else {
      for(x=0;x<width;x++) buffer[size-1][x+size2] = 0;
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
/** \brief Perform a convolution operation on image

  The bordering size/2 pixels of the image do not containg valid data.
  imgConvolute is quite memory efficient.
  Only (img->dimx+size)*size*sizeof(float) bytes are consumed by a
  temporary buffer. When processing multiple planes/frames/images with
  equal width and kernel size, efficiency can be improved further by
  allocating the temporary buffer yourself and calling imgConvoluteData
  directly.

  \param img image
  \param frame frame to perform operation on
  \param plane plane to perform operation on
  \param kernel convolution kernel
  \param size convolution kernel size
  \pre size must be smaller than img->dimx and img->dimy
  \return nonzero in case of (malloc) error
*/
int imgConvolute(IMG *img, int frame, int plane, float **kernel, int size)
{
  float **buffer;
  int y;

  /* Allocate memory for temporary buffer */
  buffer = mallocMatrix(img->dimx+size,size);
  if(!buffer) return 1;

  /* Convolute */
  imgConvoluteData(img->m[plane],buffer,frame,img->dimx, img->dimy,
                   kernel,size);

  /* Free buffer */
  for(y=0;y<size;++y) free(buffer[y]);
  free(buffer);
  return 0;
}
/*****************************************************************************/

/*****************************************************************************/
