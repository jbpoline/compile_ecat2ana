/******************************************************************************
  Copyright (c) 2007 by Turku PET Centre

  Library:     imgflip.c
  Description: Functions for turning IMG image volume data.
  
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
    Contents moved from imgproc.c inside the same library.


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
/*****************************************************************************/
#include "img.h"
/*****************************************************************************/
#include "include/imgflip.h"
/*****************************************************************************/

/*****************************************************************************/
/** Flip IMG data horizontally (left-right) */
void imgFlipHorizontal(IMG *img)
{
  int zi, yi, from, to;
  float *col_ptr;

  for(zi=0; zi<img->dimz; zi++) {
    for(yi=0; yi<img->dimy; yi++) {
      for(from=0, to=img->dimx-1; from<to; from++, to--) {
        col_ptr=img->m[zi][yi][from];
        img->m[zi][yi][from]=img->m[zi][yi][to];
        img->m[zi][yi][to]=col_ptr;
      }
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
/** Flip IMG data vertically (up-down) */
void imgFlipVertical(IMG *img)
{
  int zi, from, to;
  float **row_ptr;

  for(zi=0; zi<img->dimz; zi++) {
    for(from=0, to=img->dimy-1; from<to; from++, to--) {
      row_ptr=img->m[zi][from];
      img->m[zi][from]=img->m[zi][to];
      img->m[zi][to]=row_ptr;
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
/** Flip IMG data planes (head-toes). To work properly, the plane numbers
    must be contiguous.
 */
void imgFlipPlanes(IMG *img)
{
  int from, to;
  float ***plane_ptr;

  for(from=0, to=img->dimz-1; from<to; from++, to--) {
    plane_ptr=img->m[from];
    img->m[from]=img->m[to];
    img->m[to]=plane_ptr;
  }
}
/*****************************************************************************/

/*****************************************************************************/
