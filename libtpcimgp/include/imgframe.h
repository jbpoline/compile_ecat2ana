/******************************************************************************

  imgframe.h    (c) 2006,2008,2009 by Turku PET Centre

  2006-10-30 Vesa Oikonen
  2008-07-14 VO
  2009-02-03 VO

******************************************************************************/
#ifndef _IMGFRAME_H
#define _IMGFRAME_H
/*****************************************************************************/
extern int imgExistentTimes(IMG *img);
extern int imgDeleteFrameOverlap_old(IMG *img);
extern int imgDeleteFrameOverlap(IMG *img);
/*****************************************************************************/
/* Deprecated functions. Please don't use these anymore */
#define NEW_imgDeleteFrameOverlap imgDeleteFrameOverlap
/*****************************************************************************/
#endif

