/******************************************************************************

Copyright (c) 2006 Turku PET Centre

File: quots.c

Description:
Functions for processing strings with quotation marks.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
For more information on open source licenses see http://opensource.org.

Modification history:
2006-10-23 Vesa Oikonen
  First created.


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
/*****************************************************************************/
#include "include/quots.h"
/*****************************************************************************/

/*****************************************************************************/
/*!
 * The strstr_noquotation() function returns a pointer to the first occurrence
 *   in the string pointed to by str1, excluding parts that are inside quotation
 *   marks "" or '', of the string pointed to by str2.
 *
 * @param str1 ponter to string
 * @param str2 pointer to string
 * @return NULL pointer if no match is found.
 */ 
char *strstr_noquotation(const char *str1, const char *str2) {
  int i, test_len;
  int single_quotation=0;
  int double_quotation=0;
  char *cptr;
  
  if(str1==NULL) return((char*)NULL);
  if(str2==NULL) return((char*)str1);
  test_len=strlen(str2); if(test_len<1) return((char*)str1);
  for(i=0, cptr=(char*)str1; i<strlen(str1); i++, cptr++) {
    if(*cptr=='\'') {
      if(single_quotation==0) single_quotation=1; else single_quotation=0;
      continue;
    }
    if(*cptr=='\"') {
      if(double_quotation==0) double_quotation=1; else double_quotation=0;
      continue;
    }
    if(single_quotation==1 || double_quotation==1) continue;
    if(strncmp(cptr, str2, test_len)==0) return(cptr);
  }
  return((char*)NULL);
}
/*****************************************************************************/

/*****************************************************************************/

