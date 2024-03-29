/******************************************************************************
  Copyright (c) 2007,2009,2012 by Turku PET Centre

  Library:     imgmatch.c
  Description: Routines for checking if two IMG data are similar.
  
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
    original functions were made by Kaisa Sederholm.
    imgMatchHeader() checks the new IMG fields.
    Created functions imgMatchFrames(), imgMatchPlanes() and imgMatch().
  2009-12-29 VO
    Added function imgMaxDifference().
  2012-04-22 VO
    Fixed some comments to doxygen style.


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>
/*****************************************************************************/
#include "img.h"
#include "imgfile.h"
/*****************************************************************************/
#include "include/imgmatch.h"
/*****************************************************************************/

/*****************************************************************************/
/** Checks if two IMG data contents are similar (header information, frame times, 
 *  data dimensions, matrix contents inside specified accuracy).
\return Returns 0 if match was found, and >0 if not.
 */
int imgMatch(
  /** Pointer to the first IMG data */
  IMG *img1,
  /** Pointer to the second IMG data to be compared against the first one */
  IMG *img2,
  /** Pixel values must satisfy condition abs(x1-x2)/abs(mean(x1,x2))<=0.01.
      If you want exact match, set accuracy=0.0. */
  float accuracy
) {
  int ret=0;
  
  if(img1==NULL || img2==NULL) return(1);
  ret=imgMatchHeader(img1, img2); if(ret!=0) return(100+ret);
  ret=imgMatchFrames(img1, img2); if(ret!=0) return(200+ret);
  ret=imgMatchPlanes(img1, img2); if(ret!=0) return(300+ret);
  ret=imgMatchMatrix(img1, img2, (double)accuracy); if(ret!=0) return(400+ret);
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Checks if two image matrixes match in the accuracy of argument "accuracy".
    (set e.g. accuracy=0.99 and you will get match if all matrix 
     values satisfy abs(x1-x2)/abs(mean(x1,x2))<=0.01)
    If you want exact match, set accuracy=0.0.
  
\return Returns 0 if matrixes match, 1 if matrixes do not match.
 */
int imgMatchMatrix(IMG *img1, IMG *img2, double accuracy){
  
  int xi, yi, pi, fi;
  double s;

  for(pi=0; pi<img1->dimz; pi++) for(yi=0; yi<img1->dimy; yi++)
  for(xi=0; xi<img1->dimx; xi++) for(fi=0; fi<img1->dimt; fi++) {
    s=fabs(img1->m[pi][yi][xi][fi]+img2->m[pi][yi][xi][fi])/2.0;
    if(accuracy==0.0 || s==0.0) {
      if(img1->m[pi][yi][xi][fi]!=img2->m[pi][yi][xi][fi]){
        fprintf(stderr, "Mismatch in image matrix.\n");        
        return(1);
      }
    } else {
      if(fabs(img1->m[pi][yi][xi][fi]-img2->m[pi][yi][xi][fi])/s>(1.0-accuracy)){
        fprintf(stderr, "Mismatch in image matrix.\n");
        return(1);
      }
    }            
  }
  return 0;
}
/*****************************************************************************/

/*****************************************************************************/
/** Checks if two image headers match.
  
\return Returns 0 if headers match, 1 if headers do not match.
 */
int imgMatchHeader(IMG *img1, IMG *img2){


  if(img1->unit!=img2->unit){
    fprintf(stderr, "In the header: Mismatching unit.\n");
    return 1;
  }
  if(img1->zoom!=img2->zoom){
    fprintf(stderr, "In the header: Mismatching zoom.\n");
    return 1;
  }
  if(strcasecmp(img1->radiopharmaceutical,img2->radiopharmaceutical)){
    fprintf(stderr, "In the header: Mismatching radiopharmaceutical.\n");
    return 1;
  }
  if(img1->isotopeHalflife!=img2->isotopeHalflife){
    fprintf(stderr, "In the header: Mismatching isotope halflife.\n");
    return 1;
  }
  if(img1->decayCorrection!=img2->decayCorrection){
    fprintf(stderr, "In the header: One of the images is not corrected for decay.\n");
    return 1;
  }
  if(img1->scanStart!=img2->scanStart){
    fprintf(stderr, "In the header: Mismatching scan start times.\n");
    return 1;
  }
  if(img1->orientation!=img2->orientation){
    fprintf(stderr, "In the header: Mismatching orientation.\n");
    return 1;
  }
  if(img1->axialFOV!=img2->axialFOV){
    fprintf(stderr, "In the header: Mismatching axial FOV.\n");
    return 1;
  }
  if(img1->transaxialFOV!=img2->transaxialFOV){
    fprintf(stderr, "In the header: Mismatching transaxial FOV.\n");
    return 1;
  }
  if(img1->sampleDistance!=img2->sampleDistance){
    fprintf(stderr, "In the header: Mismatching sample distance.\n");
    return 1;
  } 

  if(strcasecmp(img1->studyNr,img2->studyNr)){
    fprintf(stderr, "In the header: Mismatching study number.\n");
    return 1;
  }
  if(strcmp(img1->userProcessCode,img2->userProcessCode)){
    fprintf(stderr, "In the header: Mismatching user process code.\n");
    return 1;
  }
  if(strcasecmp(img1->patientName,img2->patientName)){
    fprintf(stderr, "In the header: Mismatching patient name.\n");
    return 1;
  }
  if(strcasecmp(img1->patientID,img2->patientID)){
    fprintf(stderr, "In the header: Mismatching patient ID.\n");
    return 1;
  }
  if(img1->sizex!=img2->sizex){
    fprintf(stderr, "In the header: Mismatching size (x-axis).\n");
    return 1;
  }
  if(img1->sizey!=img2->sizey){
    fprintf(stderr, "In the header: Mismatching size (y-axis).\n");
    return 1;
  }
  if(img1->sizez!=img2->sizez){
    fprintf(stderr, "In the header: Mismatching size (z-axis).\n");
    return 1;
  }
  if(img1->resolutionx!=img2->resolutionx){
    fprintf(stderr, "In the header: Mismatching resolution (x-axis).\n");
    return 1;
  }
  if(img1->resolutiony!=img2->resolutiony){
    fprintf(stderr, "In the header: Mismatching resolution (y-axis).\n");
    return 1;
  }
  if(img1->resolutionz!=img2->resolutionz){
    fprintf(stderr, "In the header: Mismatching resolution (z-axis).\n");
    return 1;
  }
  if(img1->scanner!=img2->scanner){
    fprintf(stderr, "In the header: Mismatching scanner.\n");
    return 1;
  }
  if(img1->_dataType!=img2->_dataType){
    fprintf(stderr, "In the header: Mismatching data type.\n");
    return 1;
  }
  if(img1->_fileFormat!=img2->_fileFormat){
    fprintf(stderr, "In the header: Mismatching file format.\n");
    return 1;
  }
  if(img1->isWeight!=img2->isWeight){
    fprintf(stderr, "In the header: Mismatching in weights.\n");
    return 1;
  }
  
  return 0;
}
/*****************************************************************************/

/*****************************************************************************/
/** Checks if the frame times of two IMG data do match: 
 return Returns 0 if match was found, and >0 if they do not match.
 */
int imgMatchFrames(IMG *img1, IMG *img2)
{
  if(img1->dimt!=img2->dimt) return(1);
  for(int fi=0; fi<img1->dimt; fi++) {
    if(fabs(img1->start[fi]-img2->start[fi])>0.001) return(11);
    if(fabs(img1->end[fi]-img2->end[fi])>0.001) return(12);
    if(fabs(img1->mid[fi]-img2->mid[fi])>0.001) return(13);
  }
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Checks if the planes of two IMG data do match:
 *  plane numbers. 
\return Returns 0 if match was found, and >0 if no match.
 */
int imgMatchPlanes(IMG *img1, IMG *img2)
{
  if(img1->dimz!=img2->dimz) return(1);
  for(int zi=0; zi<img1->dimz; zi++) {
    if(img1->planeNumber[zi]!=img2->planeNumber[zi]) return(11);
  }
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Checks if pixel values in two image matrices are similar withing
 *  specified allowed absolute difference.
\return Returns 0 if successful, and difference was found, -1 if no difference
    was found, and >0 in case of an error.
 */
int imgMaxDifference(
  /** Pointer to the first IMG */
  IMG *img1,
  /** Pointer to the second IMG, with the same dimensions as img1 */
  IMG *img2,
  /** Voxel where absolute difference was found largest;
      enter NULL if not needed; returns [0,0,0,0] if no difference was found */
  VOXEL_4D *absdiff,
  /** Pointer where max absolute difference is written; NULL if not needed */
  float *abs_max,
  /** Voxel where relational difference was found largest;
     enter NULL if not needed; returns [0,0,0,0] if no difference was found */
  VOXEL_4D *reldiff,
  /** Pointer where max relational difference is written; NULL if not needed */
  float *rel_max
) {
  
  int xi, yi, zi, ti;
  float s, v, absmax=0.0, relmax=0.0;

  /* Initiate result voxels */
  if(absdiff!=NULL) absdiff->x=absdiff->y=absdiff->z=absdiff->t=0;
  if(abs_max!=NULL) *abs_max=0;
  if(reldiff!=NULL) reldiff->x=reldiff->y=reldiff->z=reldiff->t=0;
  if(rel_max!=NULL) *rel_max=0;

  /* Check input */
  if(img1==NULL || img2==NULL) return 1;
  if(img1->dimx!=img2->dimx) return 2;
  if(img1->dimy!=img2->dimy) return 3;
  if(img1->dimz!=img2->dimz) return 4;
  if(img1->dimt!=img2->dimt) return 5;


  /* Search for max absolute difference */
  for(zi=0; zi<img1->dimz; zi++) for(yi=0; yi<img1->dimy; yi++)
  for(xi=0; xi<img1->dimx; xi++) for(ti=0; ti<img1->dimt; ti++) {
    s=fabs(img1->m[zi][yi][xi][ti]-img2->m[zi][yi][xi][ti]);
    if(s>absmax) {
      absmax=s;
      if(absdiff!=NULL) {
        absdiff->x=xi+1; absdiff->y=yi+1; absdiff->z=zi+1; absdiff->t=ti+1;}
    }
  }
  //printf("absmax := %g\n", absmax);
  if(abs_max!=NULL) *abs_max=absmax;


  /* Search for max relational difference.
     If relational difference is Inf (image average is zero), then relmax
     is set to FLT_MAX, and from those pixels the one where
     absolute difference is highest is searched */
  float local_absmax=0.0;
  for(zi=0; zi<img1->dimz; zi++) for(yi=0; yi<img1->dimy; yi++)
  for(xi=0; xi<img1->dimx; xi++) for(ti=0; ti<img1->dimt; ti++) {
    s=fabs(img1->m[zi][yi][xi][ti]-img2->m[zi][yi][xi][ti]);
    v=0.5*fabs(img1->m[zi][yi][xi][ti]+img2->m[zi][yi][xi][ti]);
    if(v>0.0) {
      s/=v; 
      if(s>relmax && local_absmax==0.0) {
        relmax=s;
        if(reldiff!=NULL) {
          reldiff->x=xi+1; reldiff->y=yi+1; reldiff->z=zi+1; reldiff->t=ti+1;}
      }
    } else if(s>0.0) { // if relational max is find out the pixel where 
      if(s>local_absmax) {
        local_absmax=s;
        if(reldiff!=NULL) {
          reldiff->x=xi+1; reldiff->y=yi+1; reldiff->z=zi+1; reldiff->t=ti+1;}
      }
    }
  }
  if(local_absmax>0.0) relmax=FLT_MAX;
  //printf("relmax := %g\n", relmax);
  if(rel_max!=NULL) *rel_max=relmax;

  /* Was there any difference? 0=yes, -1=no */
  if(absmax>0.0 || relmax>0.0) return 0; else return -1;
}
/*****************************************************************************/

/*****************************************************************************/
