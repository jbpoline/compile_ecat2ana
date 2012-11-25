/******************************************************************************
  Copyright (c) 2005,2010 by Turku PET Centre

  File: imgtransform.c
  
  Purpose: Additional procedures for processing 4D PET image data.
  Image transformation functions.

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
  2005-02-25 Calle Laakkonen, Jarkko Johansson
    First created.
  2010-11-30 Vesa Oikonen
    Added function img2cube().


******************************************************************************/

/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
/*****************************************************************************/
#include "img.h"
#include "include/imgtransform.h"
/*****************************************************************************/

/*****************************************************************************/
/** Resample image to cubic image volume, where all three dimensions are the
 *  the same, with cubic voxel sizes. Note that some smoothing occurs in
 *  the process, therefore the use of this function should be limited to
 *  illustrations of image data.
\return Returns 0 if successful, otherwise <>0.
*/
int img2cube(
  /** Pointer to IMG to be resliced; this image is not modified. */
  IMG *img1,
  /** New dimension; enter 0 to use the largest dimension in image */
  int dim,
  /** Pointer to initiated IMG where resliced data will be stored */
  IMG *img2
) {
  int ret, n, testf;
  int xi, yi, zi, xj, yj, zj, fi, xp, yp, zp;
  double xsize, ysize, zsize, newsize;
  double xdist, ydist, zdist;
  double xpc, ypc, zpc, xd, yd, zd;
  double w, wsum, d;

  if(IMG_TEST>0) printf("img2cube(img1, img2)\n"); 
  if(img1->status!=IMG_STATUS_OCCUPIED) return(1);
  if(img2->status==IMG_STATUS_OCCUPIED) imgEmpty(img2);

  if(IMG_TEST>1)
    printf("original dimensions (x,y,z) := %d,%d,%d\n",
      img1->dimx, img1->dimy, img1->dimz);
  if(dim<1) {
    /* Get the largest dimension of original image volume */
    dim=img1->dimz; n=0;
    if(img1->dimx>dim) {dim=img1->dimx; n++;}
    if(img1->dimy>dim) {dim=img1->dimy; n++;}
  }
  if(IMG_TEST>1)
    printf("new dimensions (x,y,z) := %d,%d,%d\n", dim, dim, dim);
  testf=dim*dim*dim/10;

  /* Allocate memory for the new IMG */
  ret=imgAllocate(img2, dim, dim, dim, img1->dimt);
  if(ret) return(10+ret);
  imgCopyhdr(img1, img2);

  /* Calculate the physical extensions of original image volume */
  xsize=img1->dimx*(img1->sizex + img1->gapx); 
  ysize=img1->dimy*(img1->sizey + img1->gapy); 
  zsize=img1->dimz*(img1->sizez + img1->gapz);
  if(IMG_TEST>1) {
    printf("original image size (x,y,z) in mm := %g,%g,%g\n",
      xsize, ysize, zsize);
  }
  /* New image volume will be a cube of highest size of those */
  newsize=xsize; n=0;
  if(ysize>newsize) {newsize=ysize; n++;}
  if(zsize>newsize) {newsize=zsize; n++;}
  if(IMG_TEST>1) {
    printf("original image size (x,y,z) in mm := %g,%g,%g\n",
      xsize, ysize, zsize);
    if(n>0) printf("new image size (x,y,z) in mm:= %g,%g,%g\n",
      newsize, newsize, newsize);
    else printf("original image size needs not to be changed.\n");
  }
  img2->gapx=img2->gapy=img2->gapz=0.0;
  img2->sizex=img2->sizey=img2->sizez=newsize/(double)dim;

  /* Resample the image volume */
  n=0;
  for(zj=0; zj<dim; zj++) for(yj=0; yj<dim; yj++) for(xj=0; xj<dim; xj++) {
    if(IMG_TEST>3 && n%testf==0) printf("zj=%d yj=%d xj=%d\n", zj, yj, xj);
    /* Calculate the distance of this voxel from the mid of new image volume */
    xdist=((double)xj+0.5)*img2->sizex - 0.5*newsize;
    ydist=((double)yj+0.5)*img2->sizey - 0.5*newsize;
    zdist=((double)zj+0.5)*img2->sizez - 0.5*newsize;
    if(IMG_TEST>3 && n%testf==0)
      printf("  xdist=%g ydist=%g zdist=%g\n", xdist, ydist, zdist);
    /* The place in coordinates of original image */
    xpc=(0.5*xsize+xdist)/(img1->sizex + img1->gapx);
    ypc=(0.5*ysize+ydist)/(img1->sizey + img1->gapy);
    zpc=(0.5*zsize+zdist)/(img1->sizez + img1->gapz);
    /* Inside which voxel it resides? */
    xp=(int)(xpc+0.5); yp=(int)(ypc+0.5); zp=(int)(zpc+0.5);
    if(IMG_TEST>3 && n%testf==0)
      printf("  inside pixel %d,%d,%d\n", xp, yp, zp);
    /* Calculate 1/distance-weighted average of 3x3x3 pixels around it */
    for(fi=0; fi<img1->dimt; fi++) img2->m[zj][yj][xj][fi]=0.0;
    wsum=0.0;
    for(zi=zp-1; zi<=zp+1; zi++)
      for(yi=yp-1; yi<=yp+1; yi++) for(xi=xp-1; xi<=xp+1; xi++) {
        /* Distance between voxels */
        xd=(0.5+xi)-xpc; yd=(0.5+yi)-ypc; zd=(0.5+zi)-zpc;
        d=xd*xd+yd*yd+zd*zd; //if(d>0.0) d=sqrt(d); else d=0.0;
        if(IMG_TEST>3 && n%testf==0)
          printf("    distance^2 from (%d,%d,%d) := %g\n", xi, yi, zi, d);
        /* Add weight to the weight sum */
        w=exp(-d); wsum+=w;
        if(IMG_TEST>3 && n%testf==0) printf("    weight := %g\n", w);
        /* If voxel is inside the image, add value to the sum */
        if(zi>=0 && yi>=0 && xi>=0 &&
           zi<img1->dimz && yi<img1->dimy && xi<img1->dimx)
          {
            for(fi=0; fi<img1->dimt; fi++) {
              img2->m[zj][yj][xj][fi]+=w*img1->m[zi][yi][xi][fi];
            }
          }
      }
    /* Divide the sum with sum weight */
    for(fi=0; fi<img1->dimt; fi++) img2->m[zj][yj][xj][fi]/=wsum;
    if(IMG_TEST>3 && n%testf==0)
      printf("  weighted avg in 1st frame := %g\n", img2->m[zj][yj][xj][0]);
    n++;
  }

  return 0;
}
/*****************************************************************************/

/*****************************************************************************/
/**
   Image scaling using the defined method.
*/
void imgScale(
  /** Source image to be scaled */
  IMG *src,
  /** Scaled image; must be allocated to the size of resulting image */
  IMG *targ,
  /** Scaling factor */
  float zoom,
  /** Method code number */
  int method)
{
  int frame, plane, x, y;
  float **tmp_targ;

  // Allocate memory for temporary target matrix:
  tmp_targ = malloc(targ->dimy * sizeof(float*));
  for(y=0; y<targ->dimy; y++)
    tmp_targ[y] = malloc(targ->dimx * sizeof(float));

  for(plane=0; plane<src->dimz; plane++)
    for(frame=0; frame<src->dimt; frame++){
      integerScale(frame, src->m[plane], tmp_targ, src->dimx, src->dimy, zoom);
      // Copy scaled image matrix to target image buffer:
      for(y=0; y<targ->dimy; y++)
	for(x=0; x<targ->dimx; x++)
	  targ->m[plane][y][x][frame] = tmp_targ[y][x];	
    }
}
/*****************************************************************************/

/*****************************************************************************/
/**
   Simple integer scaling function using pixel duplication.
*/
void integerScale(
  /** Serial number of the frame to be scaled*/
  int frame,
  /** Source data in three dimensional array: srcp[y][x][frame] */
  float ***src, 
  /** Target data array: targ[y][x] */
  float **targ, 
  /** Width of the source data matrix */
  int width,
  /** Height of the source data matrix*/
  int height,
  /** Scaling factor */
  int zoom)
{
  int w, h, z;

  for(h=0; h<height; h++){
    for(w=0; w<width; w++){
      float val;
      val=src[h][w][frame];
      for(z=0;z<zoom;z++) {
        targ[h*zoom][w*zoom + z]=val;
      }
    }
    for(z=1;z<zoom;z++) {
      memcpy(targ[h*zoom+z],targ[h*zoom],width*zoom*sizeof(float));
    }
  }
}
/*****************************************************************************/

/*****************************************************************************/
