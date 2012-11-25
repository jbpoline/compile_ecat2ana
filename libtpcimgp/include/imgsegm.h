/******************************************************************************
  Copyright (c)  2002-2004,2009 by Turku PET Centre

  imgsegm.h - definitions for imgsegm.c

  2009-02-04 Vesa Oikonen

******************************************************************************/
#ifndef _IMGSEGM_H
#define _IMGSEGM_H
/*****************************************************************************/

/*****************************************************************************/
extern int imgsegmThresholdTemplate(
  IMG *img, float minValue, float maxValue, IMG *timg
);
extern int imgsegmThresholdByTemplate(
  IMG *img, IMG *template, float minValue, float maxValue
);
extern int imgsegmThreshold(
  IMG *img, float minValue, float maxValue
);
extern int imgsegmTemplateToCluster(
  IMG *img
);
extern int imgsegmFindMaxOutsideClusters(
  IMG *sumimg, IMG *cluster, float *max, int *plane, int *row, int *col
);
extern int imgsegmClusterExpand(
  IMG *cluster, IMG *sum, IMG *dynamic, int clusterID,
  int pi, int ri, int ci, int pj, int rj, int cj, float CVlim, float CClim,
  int verbose
);
extern float imgsegmPearson(
  float *x, float *y, int nr
);
extern int imgsegmClusterMean(
  IMG *dimg, IMG *cimg, int clusterID, float *avg, int verbose
);
extern int imgsegmCheckNeighbours(
  IMG *cimg, int pi, int ri, int ci
);
extern int imgsegmFindBestNeighbour(
  IMG *dimg, IMG *cimg, int pi, int ri, int ci
);
extern int imgsegmSimilar(
  IMG *input, int smoothDim, int smoothNr, IMG *output, int verbose
);
extern int imgsegmCalcMRL(
  float y1[], float y2[], int n
);
/*****************************************************************************/
#endif /* _IMGSEGM_H */

