/******************************************************************************

  Copyright (c) 2002-2004,2009 Turku PET Centre

  Library:      imgsegm
  Description:  Functions for segmentation of 4D PET images.

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
  2002-05-24 Vesa Oikonen
    First created, based on Bentourkia M. A flexible image segmentation prior 
    to parametric estimation. Comput Med Imaging Graph. 2001;25:501-506.
  2003-05-16 VO
    filterClusterExpand() goes to corner pixels also in plane dimension.
    filterClusterTACs() sets DFT ._type to 1 by default.
    Added functions filterCheckNeighbours() and filterFindBestNeighbour().
  2003-05-31 VO
    Added function filterSimilar().
  2003-11-15 VO
    Removed unnecessary includes.
  2003-11-16 VO
    Added function filterCalcMRL().
    MRL part is removed from filterSimilar(); filterCalcMRL() is used instead.
  2004-07-20 VO
    Removed functions filterIntegral() and filterMaximum().
    Comment styles changed.
  2004-07-25 VO
    Corrected a great number of bugs.
  2009-02-04 VO
    Moved into libtpcimgp: function names changed to start with imgsegm,
    and suitable functions moved to other .c files in this library. 


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
/*****************************************************************************/
#include "petc99.h"
#include "img.h"
/*****************************************************************************/
#include "include/imgproc.h"
#include "include/imgsegm.h"
/*****************************************************************************/

/*****************************************************************************/
/** Local definitions */
typedef struct {
  /** index in image z direction */
  int p;        
  /** index in image y direction */
  int r;        
  /** index in image x direction */
  int c;        
  /** maximum run-length */
  int mrl;      
  /** TAC area-under-curve */
  double dauc;
  /** order */
  int order;
} IMGSEGMMASK;
/*****************************************************************************/
/** Local functions */
int imgsegmSimilarSort(const void *mask1, const void *mask2);
/*****************************************************************************/

/*****************************************************************************/
/** Allocate and fill a template image based on the specified image and
    threshold values.
    If pixel value in original image is <minValue, sets the template pixel to
    1, if >maxValue, sets template pixel to 2, and to 0, if value is between
    minValue and maxValue. Only first frame is used, allocated and filled.
\return Returns 0 if ok.
 */
int imgsegmThresholdTemplate(
  /** Pointer to original (static) image */
  IMG *img,       
  /** Lower threshold limit */
  float minValue, 
  /** Upper threshold limit */
  float maxValue, 
  /** Pointer to initiated and empty template image */ 
  IMG *timg       
) {
  int frame, plane, i, j, ret;

  if(img->status!=IMG_STATUS_OCCUPIED) return(1);
  /* Allocate memory for the template data */
  ret=imgAllocate(timg, img->dimz, img->dimy, img->dimx, 1);
  if(ret) return(ret);

  /* set template header information */
  imgCopyhdr(img, timg);
  timg->start[0]=img->start[0]; timg->end[0]=img->end[img->dimt-1];
  timg->mid[0]=(timg->start[0]+timg->end[0])/2.0;
  timg->isWeight=0;

  /* Make template */
  for(plane=0; plane<img->dimz; plane++)
    for(j=0; j<img->dimy; j++) for(i=0; i<img->dimx; i++) {
      frame=0;
      if(img->m[plane][j][i][frame]<minValue) 
        timg->m[plane][j][i][frame]=1.0;
      else if(img->m[plane][j][i][frame]>maxValue) 
        timg->m[plane][j][i][frame]=2.0;
      else
        timg->m[plane][j][i][frame]=0.0;
    }
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Sets pixel values in img to minValue, if corresponding pixel value in
    template is == 2, and to maxValue, if template value is == 1.
    Only first plane of template is used.
\return Returns 0 if ok.
 */
int imgsegmThresholdByTemplate(
  /** Dynamic image which is modified based on the template */
  IMG *img,       
  /** Template image */
  IMG *template,  
  /** This value is put into img if template value is =2 */
  float minValue, 
  /** This value is put into img if template value is =1 */
  float maxValue  
) {
  int frame, plane, i, j;

  if(img->status!=IMG_STATUS_OCCUPIED) return(1);
  if(template->status!=IMG_STATUS_OCCUPIED) return(2);
  for(plane=0; plane<img->dimz; plane++)
    for(j=0; j<img->dimy; j++) for(i=0; i<img->dimx; i++)
      if(template->m[plane][j][i][0]==1.0) {  
        for(frame=0; frame<img->dimt; frame++)
          img->m[plane][j][i][frame]=minValue;
      } else if(template->m[plane][j][i][0]==2.0) {
        for(frame=0; frame<img->dimt; frame++)
          img->m[plane][j][i][frame]=maxValue;
      }
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Sets values <minValue to zero, and values >maxValue to maxValue. */
int imgsegmThreshold(
  /** Dynamic or static image */
  IMG *img,
  /** Min pixel value */
  float minValue,
  /** Max pixel value */
  float maxValue
) {
  int frame, plane, i, j;

  if(img->status!=IMG_STATUS_OCCUPIED) return(1);
  for(plane=0; plane<img->dimz; plane++)
    for(j=0; j<img->dimy; j++) for(i=0; i<img->dimx; i++)
      for(frame=0; frame<img->dimt; frame++)
        if(img->m[plane][j][i][frame]<minValue) 
          img->m[plane][j][i][frame]=0.0;
        else if(img->m[plane][j][i][frame]>maxValue) 
          img->m[plane][j][i][frame]=maxValue;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Sets templates 0 values to -1, and others to value 0. */
int imgsegmTemplateToCluster(IMG *img)
{
  int plane, i, j;

  if(img->status!=IMG_STATUS_OCCUPIED) return(1);
  if(img->dimt>1) return(2);
  for(plane=0; plane<img->dimz; plane++)
    for(j=0; j<img->dimy; j++) for(i=0; i<img->dimx; i++) {
      if(img->m[plane][j][i][0]>0.0) img->m[plane][j][i][0]=0.0;
      else img->m[plane][j][i][0]=-1.0;
    }
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Finds the maximum sumimg pixel value, excluding all pixels which already
    belong to clusters (cluster>=0).
\return Returns 0, if max was found, >1 in case of an error, and -1, 
    if all pixels already belong to clusters.
 */
int imgsegmFindMaxOutsideClusters(
  /** Integral image          */
  IMG *sumimg,                  
  /** Cluster image           */
  IMG *cluster,                 
  /** Found max value         */
  float *max,                   
  /** Found max pixel [Z,y,x] */
  int *plane,
  /** Found max pixel [z,Y,x] */
  int *row,
  /** Found max pixel [z,y,X] */
  int *col
) {
  int n=0, p, i, j, maxp=0, maxi=0, maxj=0;

  if(sumimg->status!=IMG_STATUS_OCCUPIED) return(1);
  if(cluster->status!=IMG_STATUS_OCCUPIED) return(2);
  if(sumimg->dimt>1) return(3);
  if(cluster->dimt>1) return(4);
  if(sumimg->dimx!=cluster->dimx || sumimg->dimy!=cluster->dimy) return(5);
  if(sumimg->dimz!=cluster->dimz) return(6);

  *max=-1.0e8;
  for(p=0; p<sumimg->dimz; p++) {
    for(j=0; j<sumimg->dimy; j++) for(i=0; i<sumimg->dimx; i++) 
      if(cluster->m[p][j][i][0]<-0.1) {
        if(sumimg->m[p][j][i][0]>*max) {
          *max=sumimg->m[p][j][i][0]; maxp=p; maxj=j; maxi=i;
        }
        n++;
      }
  }
  if(n==0) return(-1);
  *plane=maxp; *row=maxj; *col=maxi;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Expands the cluster locally to its neighbour pixels.
    Calls itself recursively until neighbouring pixels do not belong to cluster.
    Because of recursion, program which uses this function needs a large space
    for stack: add the following line to your program: unsigned _stklen = 4194304;
\return Returns 0, if test pixel belongs to cluster, and 1 if not, and
   >1 in case of an error.
 */
int imgsegmClusterExpand(
  /** pointer to cluster image data */
  IMG *cimg,
  /** pointer to sum image data */          
  IMG *simg,          
  /** pointer to dynamic image data */
  IMG *dimg,           
  /** number of cluster to be tested */
  int clusterID,       
  /** coordinates of test pixel [z,y,x] */
  int pi, 
  /** coordinates of test pixel [z,y,x] */
  int ri, 
  /** coordinates of test pixel [z,y,x] */
  int ci,
  /** coordinates of cluster start [z,y,x] */
  int pj,
  /** coordinates of cluster start [z,y,x] */
  int rj,
  /** coordinates of cluster start [z,y,x] */
  int cj,
  /** CV limit */
  float CVlim,         
  /** CC limit */
  float CClim,
  /** Verbose level; if zero, then only warnings are printed into stderr */
  int verbose
) {
  float cv, mean, a, b;
  int ret, pk, rk, ck;

  if(verbose>0) 
    printf("imgsegmClusterExpand(cimg, simg, dimg, %d, %d, %d, %d, %d, %d, %d, %f, %f, %d)\n",
       clusterID, pi, ri, ci, pj, rj, cj, CVlim, CClim, verbose);
  if(cimg==NULL || cimg->status!=IMG_STATUS_OCCUPIED) return(2);
  if(simg==NULL || simg->status!=IMG_STATUS_OCCUPIED) return(3);
  if(dimg==NULL || dimg->status!=IMG_STATUS_OCCUPIED) return(4);

  /* Check that test pixel resides inside image volume */
  if(pi<0 || ri<0 || ci<0 || pi>=cimg->dimz || ri>=cimg->dimy || ci>=cimg->dimx) {
    if(verbose>1) printf("pixels does not reside inside image\n");
    return(1);
  }

  /* Check that test pixel is not already part of any cluster */
  if(cimg->m[pi][ri][ci][0]>=-0.1) {
    if(verbose>1) 
      printf("pixel already belongs to cluster %g\n", cimg->m[pi][ri][ci][0]);
    return(1);
  }

  /* Check that AUCs are matching */
  mean=0.5*(simg->m[pi][ri][ci][0]+simg->m[pj][rj][cj][0]);
  a=simg->m[pi][ri][ci][0]-mean; b=simg->m[pj][rj][cj][0]-mean;
  if(fabs(mean)>1.0e-10) cv=(a*a + b*b) / mean; else cv=0.0;
  if(verbose>2) printf("cv=%g CVlim=%g mean=%g\n", cv, CVlim, mean);
  if(cv>CVlim) {
    if(verbose>2) printf("AUCs are not matching, %g>%g\n", cv, CVlim);
    return(1);
  }

  /* Check that TACs are correlating */
  a=imgsegmPearson(dimg->m[pj][rj][cj], dimg->m[pi][ri][ci], dimg->dimt);
  if(verbose>3) printf("  r=%g CClim=%g\n", a, CClim);
  if(a<CClim) {
    if(verbose>2) printf("TACs are not correlating, %g<%g\n", a, CClim);
    return(1);
  }

  /* Seems that the test pixel belongs to the cluster */
  cimg->m[pi][ri][ci][0]=clusterID;
  if(verbose>0)
    printf("  [%d][%d][%d] belongs to cluster %d\n", pi, ri, ci, clusterID);

  /* Check if the neighbouring pixels belong to this cluster */
  for(pk=pi-1; pk<=pi+1; pk++) if(pk>=0 && pk<cimg->dimz)
    for(rk=ri-1; rk<=ri+1; rk++) if(rk>=0 && rk<cimg->dimy)
      for(ck=ci-1; ck<=ci+1; ck++) if(ck>=0 && ck<cimg->dimx)
        if((pk!=pi || rk!=ri || ck!=ci) && cimg->m[pk][rk][ck][0]<-0.1) {
          ret=imgsegmClusterExpand(cimg, simg, dimg, clusterID,
                 pk, rk, ck, pj, rj, cj, CVlim, CClim, verbose);
          if(ret>1) return(ret);
        }

  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Calculates Pearson's correlation coefficient between x[] and y[y] values.
    Not corrected for sample size.
\return Returns Pearson's correlation coefficient; in case of an error returns 0.
 */
float imgsegmPearson(
  /** x axis values */
  float *x,
  /** y axis values */
  float *y,
  /** nr of samples */
  int nr
) {
  int i;
  float sumx=0.0, sumy=0.0, sumsx=0.0, sumsy=0.0, sumxy=0.0, r, q;

  if(nr<1 || x==NULL || y==NULL) return(0.0);
  if(nr<3) return(1.0);
  for(i=0; i<nr; i++) {
    sumx+=x[i]; sumy+=y[i];
    sumsx+=x[i]*x[i]; sumsy+=y[i]*y[i];
    sumxy+=x[i]*y[i];
  }
  q=(sumsx - sumx*sumx/(float)nr) * (sumsy - sumy*sumy/(float)nr);
  if(q<=0.0) return(1.0);
  r=(sumxy-((sumx*sumy)/(float)nr)) / sqrt(q);
  return(r);
}
/*****************************************************************************/

/*****************************************************************************/
/** Calculates the average of pixels belonging to the specified cluster, and
    returns this average for each frame in the specified float array.
    Cluster pixels do not have to be adjacent!
\return Returns the nr of pixels that belong to this cluster, or <0 in case of an error.
 */
int imgsegmClusterMean(
  /** Dynamic image */
  IMG *dimg,    
  /** Cluster image */
  IMG *cimg,    
  /** Cluster number 0.. */
  int clusterID,
  /** Pointer to a float array where cluster average TAC is written */
  float *avg, 
  /** Verbose level; if zero, then only warnings are printed into stderr */
  int verbose
) {
  int n=0, fi, pi, ri, ci;

  /* Check the arguments */
  if(dimg==NULL || cimg==NULL || avg==NULL) return(-1);
  if(cimg->dimx!=dimg->dimx || cimg->dimy!=dimg->dimy || cimg->dimz!=dimg->dimz)
    return(-2);
  
  if(verbose>0) printf("calculating avg of cluster %d:", clusterID);
  for(fi=0; fi<dimg->dimt; fi++) avg[fi]=0.0;
  for(pi=0; pi<cimg->dimz; pi++)
    for(ri=0; ri<cimg->dimy; ri++)
      for(ci=0; ci<cimg->dimx; ci++)
        if(fabs(cimg->m[pi][ri][ci][0]-(float)clusterID)<0.1) {
          for(fi=0; fi<dimg->dimt; fi++)
            avg[fi]+=dimg->m[pi][ri][ci][fi];
          n++;
        }
  if(n>0) for(fi=0; fi<dimg->dimt; fi++) avg[fi]/=(float)n;
  if(verbose>0) printf(" %d pixels\n", n);
  return(n);
}
/*****************************************************************************/

/*****************************************************************************/
/** Checks if neighbours of the specified pixel belong to any cluster.
\return Returns 1, if all are in clusters, and 0, if at least one is 'free'.
 */
int imgsegmCheckNeighbours(
  /** Cluster image */
  IMG *cimg,            
  /** Pixel definition [z,y,x] */
  int pi,
  /** Pixel definition [z,y,x] */
  int ri,
  /** Pixel definition [z,y,x] */
  int ci
) {
  int pj, rj, cj;
  
  for(pj=pi-1; pj<=pi+1; pj++) if(pj>=0 && pj<cimg->dimz)
    for(rj=ri-1; rj<=ri+1; rj++) if(rj>=0 && rj<cimg->dimy)
      for(cj=ci-1; cj<=ci+1; cj++) if(cj>=0 && cj<cimg->dimx)
        if((pi!=pj || ri!=rj || ci!=cj) && cimg->m[pj][rj][cj][0]<-0.1) return(0);
  return(1);
}
/*****************************************************************************/

/*****************************************************************************/
/** Combines this pixel to the cluster of the neighbour which has the best
    correlation. All neighbours of the specified pixel must belong to some
    cluster (must be checked before calling this function).
\return If ok, returns 0.
 */
/* Assuming that all neighbour pixels belong to some cluster */
int imgsegmFindBestNeighbour(
  /** Dynamic image */
  IMG *dimg,           
  /** Cluster image */
  IMG *cimg,           
  /** Pixel definition [z,y,x] */
  int pi,
  /** Pixel definition [z,y,x] */
  int ri,
  /** Pixel definition [z,y,x] */
  int ci
) {
  int pj, rj, cj;
  float cc, best_cc, best_ID;

  best_ID=-1.0; best_cc=-1.0e+20;
  for(pj=pi-1; pj<=pi+1; pj++) if(pj>=0 && pj<cimg->dimz)
    for(rj=ri-1; rj<=ri+1; rj++) if(rj>=0 && rj<cimg->dimy)
      for(cj=ci-1; cj<=ci+1; cj++) if(cj>=0 && cj<cimg->dimx)
        if((pi!=pj || ri!=rj || ci!=cj)) {
          cc=imgsegmPearson(dimg->m[pj][rj][cj], dimg->m[pi][ri][ci], dimg->dimt);
          if(cc>best_cc) {
            best_cc=cc; 
            best_ID=cimg->m[pj][rj][cj][0];
          }
        }
  if(best_ID<0.0) return(1);
  cimg->m[pi][ri][ci][0]=best_ID;
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Computes a smoothed image from the specified dynamic image with noise.
  The TACs inside a matrix smoothDim*smoothDim*smoothDim are sorted by the MRL
  and AUC, and each TAC is replaced by the mean of the best smoothNr TACs.
  This function allocates memory for output and copies the header info.
\return Returns 0 if ok.
*/
int imgsegmSimilar(
  /** Dynamic input image */
  IMG *input,  
  /** Smoothing mask dimensions; either 3 or 5 (default) */
  int smoothDim,
  /** Nr of TACs to average; default is 9 */
  int smoothNr,
  /** Smoothed image */
  IMG *output,
  /** Verbose level; if zero, then only warnings are printed into stderr */
  int verbose
) {
  int ret, pi, ri, ci, pj, rj, cj, mi, mj, fi, maskNr, smod;
  IMG sum;
  IMGSEGMMASK mask[125];
  

  if(verbose>0) printf("in filterSimilar(input, %d, %d, output)\n",
    smoothDim, smoothNr);
  /* Check the parameters */
  if(input->status!=IMG_STATUS_OCCUPIED) return(1);
  if(input->dimt<2) return(2);
  if(smoothDim==3) smod=1; else smod=2;
  if(smoothNr<2) smoothNr=9;
  
  /* Compute AUC image */
  imgInit(&sum);
  ret=imgFrameIntegral(input, 0, input->dimt-1, &sum, verbose);
  if(ret) return(10+ret);
  
  /* Allocate space for smoothed image */
  ret=imgAllocateWithHeader(
    output, input->dimz, input->dimy, input->dimx, input->dimt, input);
  if(ret) {imgEmpty(&sum); return(20+ret);}
    
  /* One pixel at a time */
  for(pi=0; pi<input->dimz; pi++) {
    if(input->dimz>1) {fprintf(stdout, "."); fflush(stdout);}
    for(ri=0; ri<input->dimy; ri++) {
      for(ci=0; ci<input->dimx; ci++) {
        /* Fill the smoothing mask values */
        for(mi=0; mi<125; mi++) mask[mi].order=0;
        mi=0;
        for(pj=pi-smod; pj<=pi+smod; pj++) if(pj>=0 && pj<input->dimz) {
          for(rj=ri-smod; rj<=ri+smod; rj++) if(rj>=0 && rj<input->dimy) {
            for(cj=ci-smod; cj<=ci+smod; cj++) if(cj>=0 && cj<input->dimx) {
              mask[mi].p=pj; mask[mi].r=rj; mask[mi].c=cj;
              /* Calculate the AUC difference */
              mask[mi].dauc=fabs(sum.m[pi][ri][ci][0]-sum.m[pj][rj][cj][0]);
              /* Calculate the MRL */
              mask[mi].mrl=imgsegmCalcMRL(input->m[pi][ri][ci], input->m[pj][rj][cj], input->dimt);
              mi++;
            }
          }
        }
        maskNr=mi;
        /* Put mask values in similarity order */
        for(mi=0; mi<maskNr; mi++) {
          mask[mi].order=0;
          for(mj=0; mj<maskNr; mj++) {
            if(mask[mj].dauc>mask[mi].dauc) mask[mi].order++;
            if(mask[mj].mrl>mask[mi].mrl) mask[mi].order++;
          }
        }
        qsort(mask, maskNr, sizeof(IMGSEGMMASK), imgsegmSimilarSort);
        /* Calculate average */
        mj=smoothNr; if(mj>maskNr/2) mj=maskNr/2;
        for(fi=0; fi<input->dimt; fi++) {
          output->m[pi][ri][ci][fi]=0.0;
          for(mi=0; mi<mj; mi++) {
            /*printf("order=%d pj=%d rj=%d cj=%d\n", mask[mi].order, mask[mi].p, mask[mi].r, mask[mi].c);*/
            output->m[pi][ri][ci][fi]+=
              input->m[mask[mi].p][mask[mi].r][mask[mi].c][fi];
          }
          output->m[pi][ri][ci][fi]/=(double)mj;
        }
      }
    }
  }
  if(input->dimz>1) {fprintf(stdout, "\n"); fflush(stdout);}
  imgEmpty(&sum);
  return(0);
}

int imgsegmSimilarSort(const void *mask1, const void *mask2)
{
  if( ((IMGSEGMMASK*)mask1)->order > ((IMGSEGMMASK*)mask2)->order ) return(-1);
  else if( ((IMGSEGMMASK*)mask1)->order < ((IMGSEGMMASK*)mask2)->order ) return(+1);
  else return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Return the maximum run length between given n length arrays of data */
int imgsegmCalcMRL(float y1[], float y2[], int n)
{
  int i, mrl=0, rl=0;
  char last_sign=0, sign;
  
  for(i=0; i<n; i++) {
    if(y1[i]>y2[i]) sign=1; else if(y1[i]<y2[i]) sign=-1; else sign=0;
    if(sign!=last_sign) {
      rl=0; last_sign=sign;
    } else {
      if(sign!=0) {rl++; if(rl>mrl) mrl=rl;}
    }
  }
  return(mrl);    
}
/*****************************************************************************/

/*****************************************************************************/

