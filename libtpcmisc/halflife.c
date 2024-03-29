/*****************************************************************************

  Copyright (c) 2003-2007,2012 by Turku PET Centre

  File:        halflife.c
  Description: Contains functions for processing isotope half-life and
               decay correction.

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
  2003-07-07 Vesa Oikonen
  2004-01-28 VO
    hlFromIsotope() identifies Cu, Br and Rb.
    Function info text format is changed.
  2004-09-17 VO
    Doxygen style comments.
  2005-01-21 VO
    Added isotope code list.
    Added function hlCorrectIsotopeCode().
  2005-11-21 VO
    Correction in lambda2factor(): bug affected situations when 
    frame duration was to be considered and lambda was negative, meaning
    removal of decay correction.
  2006-12-28 VO
    More information about functions.
    hlFromIsotope() and hlCorrectIsotopeCode() are rewritten to separate
    more strictly between isotopes.
  2007-02-02 VO
    Renamed lambda2factor() to hlLambda2factor().
    Added float version of hlLambda2factor().
    Added enum tpc_isot.
    Added list of half-lives.
    Lists are also used in hlFromIsotope().
    Added function hlIsotopeCode().
  2007-03-30 Kaisa Liukko
    Added isotope I-124
  2012-02-23 VO
    Isotope code enums moved from here to halflife.h.


*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
/****************************************************************************/
#include "include/halflife.h"
/****************************************************************************/
#include "include/substitutions.h"
/****************************************************************************/
/** Valid isotope codes. Note: when adding isotopes, make sure that all
    isotopes with one letter are AFTER all two letter isotopes with the
    same initial letter.
 */
static char *isotope_code[] = {
  "Br-75", "Br-76", "Cu-62", "Cu-64", "Fe-52",
  "Ga-68", "Ge-68", "Na-22", "Rb-82", "Zn-62",
  "F-18",  "C-11",  "N-13",  "O-15",  "O-14", "I-124",
0};
/** Isotope half-lives in minutes */
static double isotope_halflife[] = {
  HL_Br75, HL_Br76, HL_Cu62, HL_Cu64, HL_Fe52,
  HL_Ga68, HL_Ge68, HL_Na22, HL_Rb82, HL_Zn62,
  HL_F18,  HL_C11,  HL_N13,  HL_O15,  HL_O14, HL_I124,
  0.0
};
/****************************************************************************/

/****************************************************************************/
/*!
 * Isotope code as a string, based on isotope list number.
 *
 * @param isotope index of PET isotope in the list in halflife.c 
 * @return pointer to static string or "unknown".
 */
char *hlIsotopeCode(int isotope) {
  static char unknown_isotope[]="unknown";
  int n=0;
  
  while(isotope_code[n]!=0) n++;
  if(isotope<0 || isotope>n-1) return(unknown_isotope);
  else return(isotope_code[isotope]);
}
/****************************************************************************/

/****************************************************************************/
/*!
 * Identify the isotope from the specified isotope code string and
 *   return the halflife (min).
 *   This function checks the validity of the isotope string using
 *   hlCorrectIsotopeCode(), but does not change it in any way.
 *
 * @param isocode Pointer to string "C-11", "18f" etc. This argument is not changed
 * @return A negative value is returned in case of error.
 */
double hlFromIsotope(char *isocode) {
  char *corrected_isocode;
  int i = 0;
  int ok = 0;

  /* Validate the isotope string */
  corrected_isocode=hlCorrectIsotopeCode(isocode);
  if(corrected_isocode==NULL) return(-1.0);

  /* Determine the isotope and return the half-life */
  while(isotope_code[i]!=0) {
    if(strcmp(isotope_code[i], corrected_isocode)==0) {ok=1; break;}
    i++;
  }
  if(ok==1) return(isotope_halflife[i]);
  else return(-2.0);
} 
/****************************************************************************/

/****************************************************************************/
/*!
 * Calculates the isotope lambda from specified halflife.
 *
 * @param halflife halflife time value
 * @return A negative value is returned in case of error.
 */
double hl2lambda(double halflife) {
  if(halflife>0.0) return(M_LN2/halflife); else return(-1.0); /* M_LN2=log(2.0) */
}
/****************************************************************************/

/****************************************************************************/
/*!
 * Calculate the decay correction factor for specified isotope lambda.
 *
 * @param lambda Negative lambda removes decay correction
 * @param frametime Frame start time, or mid time if framedur<=0
 * @param framedur If unknown, set <0 and give mid time for frametime
 * @return A negative value is returned in case of error.
 */
double hlLambda2factor(double lambda, double frametime, double framedur) {
  double cf, ff;

  if(frametime<0) return(-1.0);
  cf=exp(lambda*frametime);
  if(framedur>1.0E-5) {
    ff=fabs(lambda)*framedur/(1.0-exp(-fabs(lambda)*framedur));
    if(lambda<0.0) cf/=ff; else cf*=ff;
  }
  return(cf);
}
/*!
 * Calculate the decay correction factor for specified isotope lambda.
 *  Version for floats (mainly image data).
 *
 * @param lambda Negative lambda removes decay correction
 * @param frametime Frame start time, or mid time if framedur<=0
 * @param framedur If unknown, set <0 and give mid time for frametime
 * @return A negative value is returned in case of error.
 */
float hlLambda2factor_float(float lambda, float frametime, float framedur) {
  float cf, ff;

  if(frametime<0) return(-1.0);
  cf=exp(lambda*frametime);
  if(framedur>1.0E-5) {
    ff=fabs(lambda)*framedur/(1.0-exp(-fabs(lambda)*framedur));
    if(lambda<0.0) cf/=ff; else cf*=ff;
  }
  return(cf);
}  
/****************************************************************************/

/****************************************************************************/
/*!
 * Check that isotope code, e.g. F-18, is in valid format, containing '-'
 *   and in this order. Returns the correct isotope code.
 * 
 * @param isocode Pointer to string "C-11", "11c" etc; contents of this string is not
 * changed, and this is not returned in any case
 * @return pointer to correct isotope code, and NULL if it was not
 * valid and could not be corrected.
 */
char *hlCorrectIsotopeCode(char *isocode) {
  int i, ok=0, n, mass_nr=0, ic_mass_nr=0;
  char *cptr, atom[3], ic_atom[3];

  /* Check, if isocode can be found in the list */
  i=ok=0;
  while(isotope_code[i]!=0) {
    if(strcasecmp(isotope_code[i], isocode)==0) {
    	ok=1;
	break;
    }
    i++;
  }
  if(ok==1) return(isotope_code[i]);

  /* Try to figure out what it is */
  /* Separate the atom name and mass number */
  n=strcspn(isocode, "-1234567890");
  if(n>2) { /* cannot be */
    return(NULL);
  } else if(n>0) { /* start with atom name */
    strncpy(atom, isocode, n); atom[n]=(char)0;
    mass_nr=atoi(isocode+n); if(mass_nr<0) mass_nr=-mass_nr;
  } else { /* starts with mass number */
    mass_nr=atoi(isocode);
    cptr=isocode; while(isdigit((int)cptr[0])) cptr++;
    if(strlen(cptr)>2) return(NULL);
    strcpy(atom, cptr); 
  }
  /* Check if it matches any of the listed isotopes */
  i=ok=0;
  while(isotope_code[i]!=0) {
    /* Separate the atom name and mass number from the listed isotope */
    n=strcspn(isotope_code[i], "-1234567890");
    strncpy(ic_atom, isotope_code[i], n); ic_atom[n]=(char)0;
    ic_mass_nr=atoi(isotope_code[i]+n+1);
    /* Check the atom name */
    if(strcasecmp(ic_atom, atom)!=0) {i++; continue;}
    /* Check the mass number, if given */
    if(mass_nr>0 && ic_mass_nr!=mass_nr) {i++; continue;}
    /* Match was found! */
    ok=1; break;
  }
  if(ok==1) return(isotope_code[i]); else return(NULL);
}
/****************************************************************************/

/****************************************************************************/
/*!
 * Identify the isotope based on its halflife (in minutes).
 *
 * @param halflife Half-life in minutes
 * @return the isotope list number, or negative value if not identified.
 */
int hlIsotopeFromHalflife(double halflife) {
  int i=0, ok=0;
  if(halflife<=0.01) return(-1);
  while(isotope_halflife[i]>0.0) {
    if( fabs(halflife/isotope_halflife[i]-1.0)<0.05 ) {ok=1; break;}
    i++;
  }
  if(ok==1) return(i);
  else return(-2.0);
}
/****************************************************************************/

/****************************************************************************/

