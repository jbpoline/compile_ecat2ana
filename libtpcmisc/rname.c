/******************************************************************************

  Copyright (c) 2012 Turku PET Centre

  File:        rname.c
  Description: Functions for processing region names.

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

  Turku PET Centre, Turku, Finland, http://www.turkupetcentre.fi

  Modification history:
  2012-09-28 Vesa Oikonen
    First introduced, most of the contents moved from studynr.c.


******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
/*****************************************************************************/
#include "include/rname.h"
/*****************************************************************************/
#include "include/substitutions.h"
#include "include/studynr.h"
/*****************************************************************************/

/*****************************************************************************/
/**
 * Split region name into 1-3 subparts of given max length.
\return Returns the number of subparts.
 */
int rnameSplit(
  /** Region name to split (string is not edited) */
  char *rname,
  /** Pointer to 1st subname (anatomical region) */
  char *name1,
  /** Pointer to 2nd subname (usually hemisphere) */
  char *name2,
  /** Pointer to 3rd subname (usually image plane) */
  char *name3,
  /** Max lenght of subnames, excluding terminal null */
  int max_name_len
) {
  char temp[MAX_REGIONNAME_LEN+1], *cptr;
  int nr=0;

  if(rname==NULL || name1==NULL || name2==NULL || name3==NULL) return(0);
  if(max_name_len<1) return(0);
  name1[0]=name2[0]=name3[0]=(char)0;
  strncpy(temp, rname, MAX_REGIONNAME_LEN); temp[MAX_REGIONNAME_LEN]=(char)0;
  cptr=strtok(temp, " \t\n\r"); if(cptr==NULL) return(nr);
  strncpy(name1, cptr, max_name_len); name1[max_name_len]=(char)0; nr++;
  cptr=strtok(NULL, " \t\n\r"); if(cptr==NULL) return(nr);
  strncpy(name2, cptr, max_name_len); name2[max_name_len]=(char)0; nr++;
  cptr=strtok(NULL, " \t\n\r"); if(cptr==NULL) return(nr);
  strncpy(name3, cptr, max_name_len); name3[max_name_len]=(char)0; nr++;
  return(nr);
}
/*****************************************************************************/

/*****************************************************************************/
/** Region name may contain dots marking non-existing identification of
 *  hemisphere or image plane etc. This function removes the dots and extra
 *  space characters.
\return Returns <0 in case of an error, otherwise the number of tokens that
    were left.
 */  
int rnameRmDots(
  /** String which contains the original region name; the modified string
   *  is written in next string, if pointer to it is given next. */
  char *rname1,
  /** Pointer to previously allocated string, into which the modified
   *  region name will be written.
   *  Enter NULL, if previous string is to be modified instead. */
  char *rname2
) {
  char *temp, *out, *cptr;
  int len, c=0;

  if(rname1==NULL) return -1;
  len=strlen(rname1);
  if(len<1) {if(rname2!=NULL) strcpy(rname2, ""); return 0;}
  temp=malloc(len+1); if(temp==NULL) return -3;
  strcpy(temp, rname1);
  if(rname2==NULL) out=rname1; else out=rname2;
  strcpy(out, "");

  /* remove dots and extra spaces */
  cptr=strtok(temp, " \t\n\r");
  while(cptr!=NULL) {
    if(strcmp(cptr, ".")!=0) {
      if(strlen(out)>0) strcat(out, " ");
      strcat(out, cptr); c++;
    }
    cptr=strtok(NULL, " \t\n\r");
  }
  free(temp);
  return c;
}
/*****************************************************************************/

/*****************************************************************************/
/** Test whether region name or number matches with a test string.
 *  Test string can contain wildcards.
 *  If test string contains only one subname, it is tested against whole rname.
 *  If it contains 2-3 subnames, those are tested against the corresponding
 *  tokens in rname. Subname '.' stands for empty name.
 *  Number is tested only if test string contains one token of all digits.
 *
\return Returns 1, in case of match, or 0 if not matched.
 */
int rnameMatch(
  /** Region name which is tested */
  char *rname,
  /** Region number (1..) */
  int rnr,
  /** Test string */
  char *test_str
) {
  char region[3][MAX_REGIONNAME_LEN+1];
  char test[3][MAX_REGIONNAME_LEN+1];
  int ni, m, rtoknr=0, ttoknr=0;

  /* Check the input */
  if(rname==NULL || strlen(rname)<1) return(0);
  if(test_str==NULL || strlen(test_str)<1) return(0);
  /* Split region names into 1-3 subnames */
  rtoknr=rnameSplit(rname, region[0], region[1], region[2], MAX_REGIONNAME_LEN);
  ttoknr=rnameSplit(test_str, test[0], test[1], test[2], MAX_REGIONNAME_LEN);
  if(rtoknr==0 || ttoknr==0) return(0);
  /* If more than one subname to test for, then test against corresponding
     subnames */
  if(ttoknr>1) {
    for(ni=0; ni<ttoknr; ni++) {
      if( strcmp(test[ni], ".") && fncasematch(test[ni], region[ni])==0 ) {
        return(0);}
    }
    return(1);
  }
  /* If just one subname to test for */
  /* If it contains only digits, then we assume that it is region number */
  for(ni=m=0; ni<strlen(test[0]); ni++)
    if(isdigit((int)test[0][ni])==0) {m++; break;}
  if(m==0 && (ni=atoi(test[0]))>0) { /* it indeed is an acceptable  number */
    if(rnr==ni) return(1); else return(0);
  }
  /* It is name; chack if it is found anywhere in the region name */
  for(ni=0; ni<rtoknr; ni++) if(fncasematch(test[0], region[ni])) return(1);
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/** Construct full TAC name from up to three subnames.
\return Returns 0 when successful, <>0 if failed.
 */
int rnameCatenate(
  /** Pointer to string of length max_ranme_len, in where the full name
   *  will be placed */  
  char *rname,
  /** Length of full TAC name, not including null char */
  int max_rname_len,
  /** Pointer to 1st subname (anatomical region); NULL if not available */
  char *name1,
  /** Pointer to 1st subname (usually hemisphere); NULL if not available */
  char *name2,
  /** Pointer to 1st subname (usually image plane); NULL if not available */
  char *name3,
  /** Spacing character, for example ' ', '_', or '-' */
  char space
) {
  int len, rlen;

  if(rname==NULL || max_rname_len<1) return 1;
  if(name1==NULL && name2==NULL && name3==NULL) return 2;

  strcpy(rname, "");

  len=strlen(name1);
  if(len>0 && len<max_rname_len && strcmp(name1, ".")!=0)
    strcpy(rname, name1);

  len=strlen(name2); rlen=strlen(rname);
  if(len>0 && (len+strlen(rname)+1)<max_rname_len && strcmp(name2, ".")!=0) {
    if(rlen>0) {rname[rlen]=(char)space; rname[rlen+1]=(char)0;}
    strcat(rname, name2);
  }

  len=strlen(name3); rlen=strlen(rname);
  if(len>0 && (len+strlen(rname)+1)<max_rname_len && strcmp(name3, ".")!=0) {
    if(rlen>0) {rname[rlen]=(char)space; rname[rlen+1]=(char)0;}
    strcat(rname, name3);
  }

  if(strlen(rname)<1) return 3;
  return 0;
}
/*****************************************************************************/

/*****************************************************************************/

