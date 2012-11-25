/******************************************************************************
  Copyright (c)  2007 by Turku PET Centre

  imgflip.h - definitions for imgflip.c

  Modification history:
  2007-02-01 Vesa Oikonen

******************************************************************************/
#ifndef _IMGFLIP_H
#define _IMGFLIP_H
/*****************************************************************************/
#include "img.h"
/*****************************************************************************/
extern void imgFlipHorizontal(IMG *img);
extern void imgFlipVertical(IMG *img);
extern void imgFlipPlanes(IMG *img);
/*****************************************************************************/

/*****************************************************************************/
#endif /* _IMGFLIP_H */

