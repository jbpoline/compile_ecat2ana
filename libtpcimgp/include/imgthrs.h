/******************************************************************************

  Copyright (c) 2006-2009 by Turku PET Centre

  imgthrs.h - definitions for imgthrs.c

  2006-10-30 Vesa Oikonen
  2007-02-01 VO
  2008-07-14 VO
  2009-02-03 VO
  2009-08-21 VO

******************************************************************************/
#ifndef _IMGTHRS_H
#define _IMGTHRS_H
/*****************************************************************************/
extern int imgThresholding(IMG *img, float threshold_level, int *thr_nr);
extern int imgThresholdingLowHigh(
  IMG *img, float lower_threshold_level, float upper_threshold_level,
  IMG *timg, int *lower_thr_nr, int *upper_thr_nr
);
extern int imgOutlierFilter(IMG *img, float limit);
extern int imgThresholdTemplateCount(
  IMG *img, float minValue, float maxValue, IMG *timg, int *count
);
extern int imgThresholdTemplate(
  IMG *img, float minValue, float maxValue, IMG *timg
);
extern int imgThresholdByTemplate(IMG *img, IMG *templt, float thrValue);
extern void imgCutoff(IMG *image, float cutoff, int mode);
/*****************************************************************************/
#endif
