/******************************************************************************

  Copyright (c) 2006,2008,2009 Turku PET Centre

  Library:      imgframe
  Description:  Functions for setting image frame times.

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
  2008-07-14 VO
    imgDeleteFrameOverlap() renamed to imgDeleteFrameOverlap_old(), and
    NEW_imgDeleteFrameOverlap() moved from libtpcmodext() to here with name
    imgDeleteFrameOverlap().
  2009-02-03 VO
    Added function imgExistentTimes().
  2009-04-30 VO
    Bug correction in imgExistentTimes().


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
/*****************************************************************************/
#include "img.h"
/*****************************************************************************/
#include "include/imgframe.h"
/*****************************************************************************/

/*****************************************************************************/
/** Verify that IMG contains frame times.
\return Returns nonzero if frame times are there, and 0 if not.
 */
int imgExistentTimes(IMG *img)
{
  int fi;
  if(img->status!=IMG_STATUS_OCCUPIED || img->dimt<1) return 0;
  for(fi=0; fi<img->dimt; fi++)
    if(img->end[fi]>0.00000001) return 1;
  return 0;  
}
/*****************************************************************************/

/*****************************************************************************/
/** Correct frame times if frames are slightly overlapping or
 *  have small gaps in between.
 *  Large gap is not corrected and it does not lead to an error.
\return If overlap is considerable (>1 s), or other error is encountered,
    function returns a non-zero value. Otherwise 0 is returned.
 */
int imgDeleteFrameOverlap(IMG *img)
{
  int fi;
  float overlap, overlap_limit=1.8, flen1, flen2;

  if(IMG_TEST) fprintf(stdout, "imgDeleteFrameOverlap()\n");
  if(img->status!=IMG_STATUS_OCCUPIED || img->dimt<1) return(1);
  for(fi=0; fi<img->dimt-1; fi++) {
    overlap=img->end[fi] - img->start[fi+1];
    if(overlap==0.0) continue; // no gap or overlap
    else if(overlap<-overlap_limit) continue; // gap is large, then do nothing
    else if(overlap>overlap_limit) return(2); // overlap is large: error
    /* Correct the small gap/overlap by making frame durations more similar */
    flen1=img->end[fi]-img->start[fi]; flen2=img->end[fi+1]-img->start[fi+1];
    if(overlap>0.0) { // overlap
      if(flen1>flen2) img->end[fi]=img->start[fi+1]; else img->start[fi+1]=img->end[fi];
    } else { // gap
      if(flen1>flen2) img->start[fi+1]=img->end[fi]; else img->end[fi]=img->start[fi+1];
    }
  }
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Correct frame times so that frames are not overlapping.
\return If overlap is considerable (>1 s), or other error is encountered,
    function returns a non-zero value. Otherwise 0 is returned.
 */
int imgDeleteFrameOverlap_old(IMG *img)
{
  int fi;
  float overlap;

  if(IMG_TEST) fprintf(stdout, "imgDeleteFrameOverlap()\n");
  if(img->status!=IMG_STATUS_OCCUPIED || img->dimt<1) return(1);
  for(fi=0; fi<img->dimt-1; fi++) {
    overlap=img->end[fi] - img->start[fi+1];
    if(overlap==0.0) continue;
    else if(overlap>1.0) return(2);
    img->end[fi]=img->start[fi+1];
  }
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/

