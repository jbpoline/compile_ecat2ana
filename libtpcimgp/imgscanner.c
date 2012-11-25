/******************************************************************************
  Copyright (c) 2007 by Turku PET Centre

  Library:     imgscanner.c
  Description: Scanner specific parameters for IMG data.
  
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
  2007-02-01 Vesa Oikonen
    Contents moved from imgproc.c inside the same library;
    Some GEMS PET-CT parameters set in imgSetScanner().


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
/*****************************************************************************/
#include "img.h"
/*****************************************************************************/
#include "include/imgscanner.h"
/*****************************************************************************/

/*****************************************************************************/
/** Sets scanner specific parameters in IMG data.
    If possible, set image zoom before calling this.
\return Returns 0, if ok.
 */
int imgSetScanner(
  /** IMG data which is filled with scanner specific information */
  IMG *img,
  /** SCANNER_ECAT931, SCANNER_ADVANCE, SCANNER_HRPLUS, SCANNER_HRRT */
  int scanner_type
) {
  int rayNr;
  
  if(img->status<IMG_STATUS_OCCUPIED) return(1);
  img->scanner=scanner_type;
  /* Set zoom to 1.0, if it is not set */
  if(img->zoom<=0.0) img->zoom=1.0;
  /* Then the others */
  if(scanner_type==SCANNER_ECAT931) {
    rayNr=192;
    img->axialFOV=108.;
    img->transaxialFOV=600.826;
    img->sampleDistance=3.12932;
    img->sizez=6.75;
  } else if(scanner_type==SCANNER_ADVANCE) {
    rayNr=281;
    img->axialFOV=153.;
    img->transaxialFOV=550.;
    img->sampleDistance=1.970177;
    img->sizez=4.25;
  } else if(scanner_type==SCANNER_HRPLUS) {
    rayNr=288;
    img->axialFOV=155.2;
    img->transaxialFOV=583.;
    img->sampleDistance=2.25; /* bin size */
    img->sizez=2.425;
  } else if(scanner_type==SCANNER_HRRT) {
    rayNr=256;
    img->axialFOV=252.28;
    img->transaxialFOV=312.;
    img->sampleDistance=1.08; /* bin size */
    img->sizez=img->sizex=img->sizey=1.218750;
  } else if(scanner_type==SCANNER_STEVCT_PET) {
    rayNr=0;
    img->sizez=3.27;
    img->sizex=img->sizey=5.46875;
  } else
    return(2);
  /* If this is image, then set also pixel sizes */
  if(img->type==IMG_TYPE_IMAGE) {
    if(scanner_type!=SCANNER_HRRT && scanner_type!=SCANNER_STEVCT_PET) {
      img->sizex=img->sizey=
        img->sampleDistance*(float)rayNr / ( (float)img->dimx*img->zoom );
    } else {
      img->sizex=img->sizey=
        img->transaxialFOV / ( (float)img->dimx*img->zoom );
    }
  }
  
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
