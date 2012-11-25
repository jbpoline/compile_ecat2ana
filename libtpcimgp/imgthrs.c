/******************************************************************************

  Copyright (c) 2006-2009 Turku PET Centre

  Library:      imgthrs
  Description:  Functions for thresholding and filtering 
                dynamic and parametric PET images.

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307 USA.

  Turku PET Centre hereby disclaims all copyright interest in the program.
  Juhani Knuuti
  Director, Professor
  Turku PET Centre, Turku, Finland, http://www.turkupetcentre.fi/

  Modification history:
  2006-10-30 Vesa Oikonen
    First created.
  2007-02-01 VO
    imgThresholdTemplate() moved to here (inside the same library).
  2008-07-14 VO
    imgCutoff() added.
  2009-02-03 VO
    imgThresholding() works even if frame times are all zeroes; actually no
    change here.
    Added function imgThresholdingLowHigh().
    Changed imgThresholdTemplate(): can work with preset template.
  2009-08-21 VO
    Added function imgThresholdTemplateCount(), which wrks the same as
    imgThresholdTemplate(), but can also return the number of accepted pixels.


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
/*****************************************************************************/
#include "img.h"
#include "imgmax.h"
/*****************************************************************************/
#include "include/imgproc.h"
#include "include/imgthrs.h"
/*****************************************************************************/

/*****************************************************************************/
/** Threshold dynamic or static IMG data:
 *  pixel time-activity curves (TACs) which have
 *  AUC less than Threshold*Max_AUC will be set to zero.
\return Returns 0, if ok.
 */
int imgThresholding(
  /** IMG data */
  IMG *img,
  /** Threshold level, e.g. 0.50 will set to zero all pixels that are less than
      50% of maximum pixel */  
  float threshold_level,
  /** Number of pixels that will fall below the threshold level;
      give NULL pointer, if not needed. */
  int *thr_nr
) {
  int zi, yi, xi, fi;
  int nr=0, ret;
  IMG aucimg;
  float maxauc, thr_limit;

  if(img->status!=IMG_STATUS_OCCUPIED) return(1);
  if(img->dimt>1) { // dynamic image
    /* Calculate AUC image */
    imgInit(&aucimg);
    ret=imgFrameIntegral(img, 0, img->dimt-1, &aucimg, 0);
    if(ret) return(ret);
    /* Search the max AUC */
    ret=imgMax(&aucimg, &maxauc); if(ret) {imgEmpty(&aucimg); return(ret);}
    /* Calculate the limit value */
    thr_limit=threshold_level*maxauc;
    /* Set to zero the pixels with AUC less than this limit */
    for(zi=0; zi<img->dimz; zi++)
      for(yi=0; yi<img->dimy; yi++)
        for(xi=0; xi<img->dimx; xi++)
          if(aucimg.m[zi][yi][xi][0]<thr_limit) {
            for(fi=0; fi<img->dimt; fi++) img->m[zi][yi][xi][fi]=0.0;
            nr++;
          }
    imgEmpty(&aucimg);
  } else { // static image
    /* Search the max value */
    ret=imgMax(img, &maxauc); if(ret) return(ret);
    /* Calculate the limit value */
    thr_limit=threshold_level*maxauc;
    /* Set to zero the pixels with AUC less than this limit */
    for(zi=0; zi<img->dimz; zi++)
      for(yi=0; yi<img->dimy; yi++)
        for(xi=0; xi<img->dimx; xi++)
          if(img->m[zi][yi][xi][0]<thr_limit) {
            for(fi=0; fi<img->dimt; fi++) img->m[zi][yi][xi][fi]=0.0;
            nr++;
          }
  }
  if(thr_nr!=NULL) *thr_nr=nr;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Threshold dynamic or static IMG data:
 *  Checks whether pixel AUCs are lower or higher than the specified
 *  threshold levels * Max_AUC. Those pixel TACs are set to zero, or
 *  alternatively, if template IMG is given, corresponding template pixel
 *  is set to 0.
\return Returns 0, if ok.
 */
int imgThresholdingLowHigh(
  /** (Dynamic) IMG data */
  IMG *img,
  /** Lower threshold level, e.g. 0.10 will set to zero all pixels that are less
       than 10% of maximum pixel */  
  float lower_threshold_level,
  /** Upper threshold level, e.g. 0.90 will set to zero all pixels that are 
      over 90% of maximum pixel */  
  float upper_threshold_level,
  /** Template image; if empty, then it will be allocated here;
   *  if pre-allocated, then template value changed to 0 when necessary,
   *  but 0 is never changed to 1; enter NULL, if original TACs are to be
   *  thresholded to zeroes */
  IMG *timg,
  /** Number of pixels that will fall below the lower threshold level;
      give NULL pointer, if not needed. */
  int *lower_thr_nr,
  /** Number of pixels that will fall below the lower threshold level;
      give NULL pointer, if not needed. */
  int *upper_thr_nr
) {
  int zi, yi, xi, fi;
  int lnr=0, unr=0, ret;
  IMG aucimg;
  float maxauc, lower_thr_limit, upper_thr_limit;
  
  if(img->status!=IMG_STATUS_OCCUPIED) return(1);

  /* Check if template is given and if it exists */
  if(timg!=NULL && timg->status!=IMG_STATUS_OCCUPIED) {
    /* Allocate memory for the template data */
    ret=imgAllocate(timg, img->dimz, img->dimy, img->dimx, 1);
    if(ret) return(ret);
    /* set template header information */
    imgCopyhdr(img, timg);
    timg->start[0]=img->start[0]; timg->end[0]=img->end[img->dimt-1];
    timg->mid[0]=(timg->start[0]+timg->end[0])/2.0;
    /* Initiate template to 1 */
    for(zi=0; zi<img->dimz; zi++)
      for(yi=0; yi<img->dimy; yi++)
        for(xi=0; xi<img->dimx; xi++)
          timg->m[zi][yi][xi][0]=1.0;
  } else if(timg!=NULL) {
    /* Check that template dimensions are ok */
    if(timg->dimz!=img->dimz || timg->dimy!=img->dimy || timg->dimx!=img->dimx)
      return 2;
    if(timg->dimt<1) return 3;
  }

  /* Threshold */
  if(img->dimt>1) { // dynamic image
    /* Calculate AUC image */
    imgInit(&aucimg);
    ret=imgFrameIntegral(img, 0, img->dimt-1, &aucimg, 0);
    if(ret) return(ret);
    /* Search the max AUC */
    ret=imgMax(&aucimg, &maxauc); if(ret) {imgEmpty(&aucimg); return(ret);}
    /* Calculate the limit values */
    lower_thr_limit=lower_threshold_level*maxauc;
    upper_thr_limit=upper_threshold_level*maxauc;
    /* Set to zero the pixels with AUC less/more than these limits */
    for(zi=0; zi<img->dimz; zi++)
      for(yi=0; yi<img->dimy; yi++)
        for(xi=0; xi<img->dimx; xi++)
          if(aucimg.m[zi][yi][xi][0]<lower_thr_limit) {
            if(timg==NULL)
              for(fi=0; fi<img->dimt; fi++) img->m[zi][yi][xi][fi]=0.0;
            else
              timg->m[zi][yi][xi][0]=0.0;
            lnr++;
          } else if(aucimg.m[zi][yi][xi][0]>upper_thr_limit) {
            if(timg==NULL)
              for(fi=0; fi<img->dimt; fi++) img->m[zi][yi][xi][fi]=0.0;
            else
              timg->m[zi][yi][xi][0]=0.0;
            unr++;
          }
    imgEmpty(&aucimg);
  } else { // static image
    /* Search the max value */
    ret=imgMax(img, &maxauc); if(ret) return(ret);
    /* Calculate the limit values */
    lower_thr_limit=lower_threshold_level*maxauc;
    upper_thr_limit=upper_threshold_level*maxauc;
    /* Set to zero the pixels with AUC less/more than these limits */
    for(zi=0; zi<img->dimz; zi++)
      for(yi=0; yi<img->dimy; yi++)
        for(xi=0; xi<img->dimx; xi++)
          if(img->m[zi][yi][xi][0]<lower_thr_limit) {
            if(timg==NULL)
              for(fi=0; fi<img->dimt; fi++) img->m[zi][yi][xi][fi]=0.0;
            else
              timg->m[zi][yi][xi][0]=0.0;
            lnr++;
          } else if(img->m[zi][yi][xi][0]>upper_thr_limit) {
            if(timg==NULL)
              for(fi=0; fi<img->dimt; fi++) img->m[zi][yi][xi][fi]=0.0;
            else
              timg->m[zi][yi][xi][0]=0.0;
            unr++;
          }
  }

  if(lower_thr_nr!=NULL) *lower_thr_nr=lnr;
  if(upper_thr_nr!=NULL) *upper_thr_nr=unr;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Creates a template image based on lower and upper threshold values.
    This function allocates memory for the template.
    If pixel value in original image is >=minValue and <=maxValue, the
    corresponding template pixel is set to 1, otherwise to 0.
    Only the first frame of images are used.
\return Returns 0 if ok.
 */
int imgThresholdTemplateCount(
  /** Original image; only first frame is used here */
  IMG *img,
  /** Lower threshold */
  float minValue,
  /** Upper threshold */
  float maxValue,
  /** Template image; if empty, then it will be allocated here;
   *  if pre-allocated, then template value changed to 0 when necessary,
   *  but 0 is never changed to 1 */
  IMG *timg,
  /** The number of pixels that pass the threshold limits is written here;
   *  set to NULL if not needed */
  int *count
) {
  int fi=0, zi, xi, yi, n, ret;

  if(img->status!=IMG_STATUS_OCCUPIED) return(1);
  if(count!=NULL) *count=0;
  /* Check if template exists */
  if(timg->status!=IMG_STATUS_OCCUPIED) {
    /* Allocate memory for the template data */
    ret=imgAllocate(timg, img->dimz, img->dimy, img->dimx, 1);
    if(ret) return(ret);
    /* set template header information */
    imgCopyhdr(img, timg);
    timg->start[0]=img->start[0]; timg->end[0]=img->end[img->dimt-1];
    timg->mid[0]=(timg->start[0]+timg->end[0])/2.0;
    /* Initiate template to 1 */
    for(zi=0; zi<img->dimz; zi++)
      for(yi=0; yi<img->dimy; yi++)
        for(xi=0; xi<img->dimx; xi++)
          timg->m[zi][yi][xi][0]=1.0;
  } else {
    /* Check that template dimensions are ok */
    if(timg->dimz!=img->dimz || timg->dimy!=img->dimy || timg->dimx!=img->dimx)
      return 2;
    if(timg->dimt<1) return 3;
  }
  /* Set template */
  fi=0; n=0;
  for(zi=0; zi<img->dimz; zi++)
    for(yi=0; yi<img->dimy; yi++)
      for(xi=0; xi<img->dimx; xi++) {
        if(img->m[zi][yi][xi][fi]<minValue || img->m[zi][yi][xi][fi]>maxValue) 
          timg->m[zi][yi][xi][0]=0.0;
        else n++;
      }
  if(count!=NULL) *count=n;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Creates a template image based on lower and upper threshold values.
    This function allocates memory for the template.
    If pixel value in original image is >=minValue and <=maxValue, the
    corresponding template pixel is set to 1, otherwise to 0.
    Only the first frame of images are used.
\return Returns 0 if ok.
 */
int imgThresholdTemplate(
  /** Original image; only first frame is used here */
  IMG *img,
  /** Lower threshold */
  float minValue,
  /** Upper threshold */
  float maxValue,
  /** Template image; if empty, then it will be allocated here;
   *  if pre-allocated, then template value changed to 0 when necessary,
   *  but 0 is never changed to 1 */
  IMG *timg
) {
  return(imgThresholdTemplateCount(img, minValue, maxValue, timg, NULL));
}
/*****************************************************************************/

/*****************************************************************************/
/** Threshold IMG by a template.
    Sets pixel values in img to thrValue, if corresponding pixel value in
    template is == 0. Only first plane of template is used.
\return Returns 0, if ok.
 */
int imgThresholdByTemplate(
  /** Image to threshold */
  IMG *img,
  /** Threshold template (1 frame) where 0=cut off */
  IMG *templt,
  /** Value which is written in cut off pixels */
  float thrValue
) {
  int fi, zi, xi, yi;

  if(img->status!=IMG_STATUS_OCCUPIED) return(1);
  if(templt->status!=IMG_STATUS_OCCUPIED) return(2);
  for(zi=0; zi<img->dimz; zi++)
    for(yi=0; yi<img->dimy; yi++) for(xi=0; xi<img->dimx; xi++)
      if(templt->m[zi][yi][xi][0]==0.0) {  
        for(fi=0; fi<img->dimt; fi++) img->m[zi][yi][xi][fi]=thrValue;
      }
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Pixel values that exceed or go under a user-defined limit are set to that
 *  limit value.
 */
void imgCutoff(
  /** Pointer to IMG struct which will be filtered here */
  IMG *image,
  /** Cutoff value */
  float cutoff,
  /** Mode of operation: 0=pixels exceeding the limit are cut off,
   *  1=pixels which go under the limit are cut off */
  int mode
) {
  int zi, yi, xi, ti;

  if(mode==0) {
    for(zi=0; zi<image->dimz; zi++)
      for(yi=0; yi<image->dimy; yi++)
        for(xi=0; xi<image->dimx; xi++)
          for(ti=0; ti<image->dimt; ti++)
            if(image->m[zi][yi][xi][ti]>cutoff) image->m[zi][yi][xi][ti]=cutoff;
  } else {
    for(zi=0; zi<image->dimz; zi++)
      for(yi=0; yi<image->dimy; yi++)
        for(xi=0; xi<image->dimx; xi++)
          for(ti=0; ti<image->dimt; ti++)
            if(image->m[zi][yi][xi][ti]<cutoff) image->m[zi][yi][xi][ti]=cutoff;
  }
  return;
}
/*****************************************************************************/

/*****************************************************************************/
/** Filter out pixels that are over limit x higher than their closest 8
    neighbour pixels.
\return Returns the nr of filtered pixels, and negative value in case of
    an error.
 */
int imgOutlierFilter(IMG *img, float limit)
{
  int zi, yi, xi, fi, nr=0;
  float f;
  IMG tmp;

  if(img->status!=IMG_STATUS_OCCUPIED || img->dimt<1) return(-1);
  imgInit(&tmp);
  if(imgAllocate(&tmp, img->dimz, img->dimy, img->dimx, 1)!=0) return(-2);
  /* Process one frame at a time */  
  for(fi=0; fi<img->dimt; fi++) {
    /* Copy one frame to safety */
    for(zi=0; zi<tmp.dimz; zi++)
      for(yi=0; yi<tmp.dimy; yi++)
        for(xi=0; xi<tmp.dimx; xi++)
          tmp.m[zi][yi][xi][0]=img->m[zi][yi][xi][fi];
    /* Go through it */
    for(zi=0; zi<img->dimz; zi++) {
      for(yi=1; yi<img->dimy-1; yi++) for(xi=1; xi<img->dimx-1; xi++) {
        f=tmp.m[zi][yi][xi-1][0]+
          tmp.m[zi][yi][xi+1][0]+
          tmp.m[zi][yi-1][xi][0]+
          tmp.m[zi][yi+1][xi][0]+
          tmp.m[zi][yi+1][xi-1][0]+
          tmp.m[zi][yi+1][xi+1][0]+
          tmp.m[zi][yi-1][xi-1][0]+
          tmp.m[zi][yi-1][xi+1][0];
        f/=8.0;
        if(img->m[zi][yi][xi][fi]>(limit*f)) {img->m[zi][yi][xi][fi]=f; nr++;}
      }
    } /* next plane */
  } /* next frame */
  imgEmpty(&tmp);

  return(nr);
}
/*****************************************************************************/

/*****************************************************************************/

