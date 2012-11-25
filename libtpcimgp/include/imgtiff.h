/******************************************************************************
  Copyright (c)  2003,2004,2009,2010 by Turku PET Centre

  imgtiff.h - definitions for imgtiff.c

  Version:
  2003-07-01 Vesa Oikonen
  2003-07-07 VO
  2004-07-06 VO
  2004-07-10 VO
  2009-04-28 VO
  2010-11-02 VO
  2012-10-26 VO
    Added to libtpcimgp. 


******************************************************************************/
#ifndef _IMGTIFF_H
#define _IMGTIFF_H
/*****************************************************************************/
/* Color scales */
#define	PET_GRAYSCALE     1
#define	PET_GRAYSCALE_INV 2
#define PET_RAINBOW       3
#define PET_RAINBOW_WB    4
/*****************************************************************************/
//int TIFF_TEST;
/*****************************************************************************/
extern int tiffWriteImg(
  IMG *img, int plane, int frame, float *maxvalue, int colorscale,
  char *fname, int matXdim, int matYdim,
  int verbose, char *status
);
/*****************************************************************************/
#endif /* _IMGTIFF_H */

