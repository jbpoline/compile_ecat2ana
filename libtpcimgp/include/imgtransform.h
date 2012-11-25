/******************************************************************************
  Copyright (c)  2005,2010 by Turku PET Centre

  imgtransform.h - definitions for imgtransform.c

  Version:
  2005-02-25 Calle Laakkonen, Jarkko Johansson
  2010-11-30 Vesa Oikonen

******************************************************************************/
#ifndef _IMGTRANSFORM_H
#define _IMGTRANSFORM_H
/*****************************************************************************/
#include <img.h>
/*****************************************************************************/
extern int img2cube(IMG *img1, int dim, IMG *img2);
extern void imgScale(IMG *src, IMG *targ, float zoom, int method);
extern void integerScale(int frame, float ***src, float **targ, int width,
                         int height, int zoom);
/*****************************************************************************/

/*****************************************************************************/
#endif /* _IMGTRANSFORM_H */
