/******************************************************************************
  Copyright (c) 2007,2010 Turku PET Centre

  substitutions.h

  Version history:
  2006-07-17 Harri Merisaari
    First introduced.
  2010-07-01 Vesa Oikonen
    Added strcasestr().


******************************************************************************/
#ifdef __STRICT_ANSI__
char* strdup(const char* s);
int strcasecmp(char *s1, char *s2);
#endif
#ifndef _GNU_SOURCE
char *strcasestr(char *src, char *sub);
#endif
/*****************************************************************************/

