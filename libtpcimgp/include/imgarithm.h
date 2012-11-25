/******************************************************************************
  Copyright (c) 2007-1012 by Turku PET Centre

  imgarithm.h - definitions for imgarithm.c

  2007-02-01 VO
  2009-08-21 VO
    Introduced new function.
  2012-11-07 VO
    Additional function arguments.


******************************************************************************/
#ifndef _IMGARITHM_H
#define _IMGARITHM_H
/*****************************************************************************/
#include "img.h"
/*****************************************************************************/
extern int imgArithm(
  IMG *img1, IMG *img2, char operation, float ulimit, int verbose);
extern int imgArithmConst(
  IMG *img, float operand, char operation, float ulimit, int verbose);
extern int imgArithmFrame(
  IMG *img1, IMG *img2, char operation, float ulimit, int verbose);
extern int imgAverageTAC(IMG *img, float *tac);
extern int imgAverageTemplateTAC(IMG *img, IMG *timg, float *tac);
extern int imgAverageAUC(IMG *img, float *avgauc);
extern int imgLog10(IMG *img);
extern int imgFrameIntegral(
  IMG *img, int first, int last, IMG *iimg, int verbose);
extern int imgRawCountsPerTime(IMG *img, int operation);
extern int imgConvertUnit(IMG *img, char *unit);
/*****************************************************************************/

/*****************************************************************************/
#endif /* _IMGARITHM_H */

