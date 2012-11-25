/******************************************************************************

  Copyright (c) 2007,2009,2012 Turku PET Centre

  Library:      imgmax
  Description:  Functions for searching min and max in IMG data.

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

  Turku PET Centre, Turku, Finland, http://www.turkupetcentre.fi

  Modification history:
  2007-01-24 Vesa Oikonen
    First created this file in libtpcimgio 1.2.0.
    Functions not changed, but just moved from libtpcimgp 1.3.1.
  2007-03-25 VO
    Added function imgReadMinMax().
  2009-12-01 VO
    Added function imgSmoothMax().
  2012-10-19 VO
    Added function imgGetPeak().
  2012-10-27 VO
    Changed interface of imgSmoothMax() to optionally save max position.


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
/*****************************************************************************/
#include "include/img.h"
#include "include/imgfile.h"
#include "include/imgmax.h"
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Searches the max pixel value in the IMG data.
 *
 * @param img image structure
 * @param maxvalue pointer to output
 * @return 0 if ok, 1 invalid image status, 2 invalid output pointer,
 * 3 invalid image dimensions
 */
int imgMax(IMG *img, float *maxvalue) {
  int pi, yi, xi, fi;
  float f;

  if(img->status<IMG_STATUS_OCCUPIED) return(1);
  if(maxvalue==NULL) return(2); else *maxvalue=0.0;
  if(img->dimt<1 || img->dimz<1 || img->dimy<1 || img->dimx<1) return(3);
  f=img->m[0][0][0][0];
  for(pi=0; pi<img->dimz; pi++)
    for(yi=0; yi<img->dimy; yi++)
      for(xi=0; xi<img->dimx; xi++)
        for(fi=0; fi<img->dimt; fi++) {
          if(img->m[pi][yi][xi][fi]>f) f=img->m[pi][yi][xi][fi];
        }
  *maxvalue=f;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Searches the max absolute pixel value in the IMG data.
 * Sets maxvalue to the absolute max value with sign.
 *
 * @param img image structure
 * @param maxvalue pointer to output
 * @return 0 if ok, 1 invalid image status, 2 invalid output pointer,
 * 3 invalid image dimensions
 */
int imgAbsMax(IMG *img, float *maxvalue) {
  int pi, yi, xi, fi;
  float f;

  if(img->status<IMG_STATUS_OCCUPIED) return(1);
  if(maxvalue==NULL) return(2); else *maxvalue=0.0;
  if(img->dimt<1 || img->dimz<1 || img->dimy<1 || img->dimx<1) return(3);
  f=img->m[0][0][0][0];
  for(pi=0; pi<img->dimz; pi++)
    for(yi=0; yi<img->dimy; yi++)
      for(xi=0; xi<img->dimx; xi++)
        for(fi=0; fi<img->dimt; fi++) {
          if(fabs(img->m[pi][yi][xi][fi])>fabs(f)) f=img->m[pi][yi][xi][fi];
        }
  *maxvalue=f;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Searches the min and max pixel value in the IMG data.
 *
 * @param img image structure
 * @param minvalue pointer to output of minimum
 * @param maxvalue pointer to output of maximum
 * @return 0 if ok, 1 invalid image status, 2 invalid output pointer,
 * 3 invalid image dimensions
 */
int imgMinMax(IMG *img, float *minvalue, float *maxvalue) {
  int pi, yi, xi, fi;
  float mi, ma;
  
  if(img->status<IMG_STATUS_OCCUPIED) return(1);
  if(minvalue==NULL || maxvalue==NULL) return(2);
  *minvalue=*maxvalue=0.0;
  if(img->dimt<1 || img->dimz<1 || img->dimy<1 || img->dimx<1) return(3);
  mi=ma=img->m[0][0][0][0];
  for(pi=0; pi<img->dimz; pi++)
    for(yi=0; yi<img->dimy; yi++)
      for(xi=0; xi<img->dimx; xi++)
        for(fi=0; fi<img->dimt; fi++) {
          if(img->m[pi][yi][xi][fi]>ma) ma=img->m[pi][yi][xi][fi];
          else if(img->m[pi][yi][xi][fi]<mi) mi=img->m[pi][yi][xi][fi];
        }
  *minvalue=mi; *maxvalue=ma;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Searches the min and max pixel value in one frame (1..dimt) of the IMG data.
 *
 * @param img image structure
 * @param frame frame number [1..number of frames]
 * @param minvalue pointer to output of minimum
 * @param maxvalue pointer to output of maximum
 * @return 0 if ok, 1 invalid image status, 2 invalid output pointer,
 * 3 invalid image dimensions
 */
int imgFrameMinMax(IMG *img, int frame, float *minvalue, float *maxvalue) {
  int pi, yi, xi, fi;
  float mi, ma;

  if(img->status<IMG_STATUS_OCCUPIED) return(1);
  if(minvalue==NULL || maxvalue==NULL) return(2);
  *minvalue=*maxvalue=0.0; fi=frame-1;
  if(img->dimt<frame || img->dimz<1 || img->dimy<1 || img->dimx<1) return(3);
  if(frame<1) return(4);
  mi=ma=img->m[0][0][0][fi];
  for(pi=0; pi<img->dimz; pi++)
    for(yi=0; yi<img->dimy; yi++)
      for(xi=0; xi<img->dimx; xi++) {
        if(img->m[pi][yi][xi][fi]>ma) ma=img->m[pi][yi][xi][fi];
        else if(img->m[pi][yi][xi][fi]<mi) mi=img->m[pi][yi][xi][fi];
      }
  *minvalue=mi; *maxvalue=ma;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Read the calibrated maximum and minimum pixel values in the specified file
 *  in ECAT 7, ECAT 6.3, or Analyze 7.5 format.
 *  File is read frame-by-frame with normal IMG functions.
 *
 * @param fname ECAT 7 or ECAT 6.3 filename, or Analyze 7.5 database
 * @param fmin pointer to minimum pixel value that will be set by this function
 * @param fmax pointer to maximum pixel value that will be set by this function
 * @return errstatus, which is STATUS_OK (0) when call was successful,
 * and >0 in case of an error.
 */
int imgReadMinMax(const char *fname, float *fmin, float *fmax) {
  int fi=0, ret;
  IMG img;
  float frmin, frmax;

  if(IMG_TEST) printf("imgReadMinMax(%s, *fmin, *fmax)\n", fname);
  imgInit(&img);
  while((ret=imgReadFrame(fname, fi+1, &img, 0)) == 0) {
    if(imgMinMax(&img, &frmin, &frmax)!=0) {imgEmpty(&img); return STATUS_FAULT;}
    if(fi==0) {
      if(fmin!=NULL) *fmin=frmin; if(fmin!=NULL) *fmax=frmax;
    } else {
      if(fmin!=NULL && *fmin>frmin) *fmin=frmin;
      if(fmax!=NULL && *fmax<frmax) *fmax=frmax;
    }
    fi++;
  } /* next frame */
  imgEmpty(&img);
  if(ret==STATUS_NOMATRIX && fi>0) return STATUS_OK;
  else return ret;
}
/*****************************************************************************/

/*****************************************************************************/
/** Searches the spatially (3x3) smoothed max pixel value in the IMG data.
 *
 * @return 0 if ok, 1 invalid image status, 2 invalid output pointer,
 * 3 invalid image dimensions
 */
int imgSmoothMax(
  /** Pointer to IMG struct */
  IMG *img,
  /** Pointer to float in which max pixel value will be written;
   *  enter NULL if not needed */
  float *maxvalue,
  /** Pointer to struct in which the position of max pixel will be written
   *  (1-based positions); enter NULL if not needed */
  IMG_PIXEL *p 
) {
  int pi, yi, xi, fi;
  float f, v;

  if(img->status<IMG_STATUS_OCCUPIED) return(1);
  if(maxvalue==NULL && p==NULL) return(2);
  if(img->dimt<1 || img->dimz<1 || img->dimy<3 || img->dimx<3) return(3);
  if(maxvalue!=NULL) *maxvalue=0.0;
  if(p!=NULL) p->x=p->y=p->z=p->f=1;
  f=-1.0E20;
  for(pi=0; pi<img->dimz; pi++)
    for(yi=1; yi<img->dimy-1; yi++)
      for(xi=1; xi<img->dimx-1; xi++)
        for(fi=0; fi<img->dimt; fi++) {
          v=img->m[pi][yi-1][xi-1][fi]+
            img->m[pi][yi-1][xi  ][fi]+
            img->m[pi][yi-1][xi+1][fi]+
            img->m[pi][yi  ][xi-1][fi]+
            img->m[pi][yi  ][xi  ][fi]*2.0+
            img->m[pi][yi  ][xi+1][fi]+
            img->m[pi][yi+1][xi-1][fi]+
            img->m[pi][yi+1][xi  ][fi]+
            img->m[pi][yi+1][xi+1][fi];
          v*=0.1;
          if(v>f) {
            f=v; if(p!=NULL) {p->x=xi+1; p->y=yi+1; p->z=pi+1; p->f=fi+1;}}
        }
  if(maxvalue!=NULL) *maxvalue=f;
  return(0);
}
/******************************************************************************/

/******************************************************************************/
/** Searches the max pixel value in the IMG data, which occurs before
 *  specified time.
\return Returns 0 if successful.
 */
int imgGetPeak(
  /** Pointer to IMG struct */
  IMG *img,
  /** Time (sec) after which max value is not searched */
  float beforeTime,
  /** Pointer to struct where max pixel position is written */
  IMG_PIXEL *p,
  /** Verbose level; 0 if nothing is to be printed in stdout */
  int verbose
) {
  int zi, yi, xi, fi, mf;
  float f;

  if(verbose>0) printf("imgGetPeak(img, %g, p, %d)\n", beforeTime, verbose);
  if(img->status<IMG_STATUS_OCCUPIED) return(1);
  if(p==NULL) return(2);
  if(img->dimt<1 || img->dimz<1 || img->dimy<1 || img->dimx<1) return(3);
  if(beforeTime<img->mid[0]) {
    if(verbose>0) fprintf(stderr, "Error: invalid max search time setting.\n");
    return(4);
  }
  f=img->m[0][0][0][0]-1.0; mf=img->dimt; p->x=p->y=p->z=p->f=1;
  for(zi=0; zi<img->dimz; zi++) {
    for(yi=0; yi<img->dimy; yi++) {
      for(xi=0; xi<img->dimx; xi++) {
        for(fi=0; fi<img->dimt; fi++) if(img->mid[fi]<=beforeTime) {
          if(img->m[zi][yi][xi][fi]<f) // lower
            continue;
          if(img->m[zi][yi][xi][fi]==f) { // equal
            // only use this if earlier than in prev max
            if(fi>=mf) continue;
          } 
          f=img->m[zi][yi][xi][fi];
          p->x=xi+1; p->y=yi+1; p->z=zi+1; p->f=fi+1;
          mf=fi;
        }
      }
    }
  }
  if(verbose>2) printf("maxval := %g\n", f);
  return(0);
}
/******************************************************************************/

/******************************************************************************/
