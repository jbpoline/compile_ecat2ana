/*****************************************************************************

  Copyright (c) 2003-2008,2012 by Turku PET Centre

  Library:      analyze.c
  Description:  Procedures for reading and writing Analyze 7.5 images.

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

  Turku PET Centre, Turku, Finland, http://www.turkupetcentre.fi/

  Modification history:
  2003-10-05 Vesa Oikonen
    First created.
  2003-12-05 VO
    Included function anaFlipping().
  2003-12-10 VO
    Setting of maximum nr of characters for string printing in
    anaPrintHeader().
  2004-02-05 VO
    Change in function information, no change in compiled code.
  2004-09-17 VO
    Doxygen style comments.
  2007-02-27 VO
    Included functions anaRemove(), anaRemoveFNameExtension(),
    anaDatabaseExists() and anaMakeSIFName().
  2007-17-07 Harri Merisaari
    modified anaRemoveFNameExtension(char *fname) for ANSI 
    compatibility build option
  2007-09-11 VO
    Corrected a bug in anaRemoveFNameExtension().
  2008-10-03 VO
    AnaReadImageData() accepts ANALYZE_DT_FLOAT and ANALYZE_DT_SIGNED_INT
    with both 16 and 32 bit pixel values (previously only 16 bits).
    Function might need to be rewritten.
  2008-10-09 VO
    AnaReadImageData() accepts ANALYZE_DT_COMPLEX, since PVELab writes pixel
    values in this (32/32) format.
  2012-02-04 VO
    Added function anaExistsNew() which may replace anaExist() at some point.
  2012-02-28 VO
    Fixed a tiny bug in test print in anaReadHeader().
    Fixed a bad bug in anaExistsNew() leading to not identifying Analyze header.


******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
/*****************************************************************************/
#include "swap.h"
#include "substitutions.h"
/*****************************************************************************/
#include "include/analyze.h"
/*****************************************************************************/

/*****************************************************************************/
/*! Check if Analyze files exist. 
 *
 * @param dbname basename of Analyze7.5 file.
 * @returns 0, if they do not, 1 if .img and .hdr do exist, and 2, if also .sif exists.
 */
int anaExists(
  const char *dbname
) {
  char temp[FILENAME_MAX];

  if(dbname==NULL || strlen(dbname)==0) return(0);
  /* Header file? */
  strcpy(temp, dbname); strcat(temp, ".hdr");
  if(access(temp, 0) == -1) return(0);
  /* Image data? */
  strcpy(temp, dbname); strcat(temp, ".img");
  if(access(temp, 0) == -1) return(0);
  /* SIF? */
  strcat(temp, ".sif"); if(access(temp, 0) != -1) return(2);
  strcpy(temp, dbname); strcat(temp, ".sif");
  if(access(temp, 0) != -1) return(2);
  return(1);
}
/*****************************************************************************/

/*****************************************************************************/
/** Check if specified filename is a Analyze file. 
\return Returns 0 if it is not, 1 if it is, and both image and header is found,
    and 2, if sif file is found too.
 */
int anaExistsNew(
  /** Filename, either header file, image file, or base name without extensions.
   *  this string is never modified. */
  const char *filename,
  /** If filename refers to a Analyze file, then header filename will be
     written in this char pointer (space needs to allocated by caller);
     NULL if not needed. */
  char *hdrfile,
  /** If filename refers to a Analyze file, then image filename will be
     written in this char pointer (space needs to allocated by caller);
     NULL if not needed. */
  char *imgfile,
  /** If filename refers to a Analyze file, and if SIF exists, then SIF filename
     will be written in this char pointer (space needs to allocated by caller);
     NULL if not needed. */
  char *siffile
) {
  char *cptr, basefile[FILENAME_MAX], temp[FILENAME_MAX];
  ANALYZE_DSR h;
  int ret;

  if(filename==NULL || strlen(filename)==0) return(0);
  if(ANALYZE_TEST>1) printf("\nanaExistsNew(%s, *str, *str, *str)\n", filename);

  /* Construct the base file name wo extensions */
  strcpy(basefile, filename);
  cptr=strrchr(basefile, '.');
  if(cptr!=NULL) {
    if(strncasecmp(cptr, ".HDR", 4)==0 || strncasecmp(cptr, ".IMG", 4)==0 )
      *cptr=(char)0;
  } 
  cptr=strrchr(basefile, '.');
  if(cptr!=NULL) {
    if(strncasecmp(cptr, ".IMG", 4)==0 )
      *cptr=(char)0;
  }
  if(ANALYZE_TEST>2) printf("\n  basefile := %s\n", basefile);

  /* Header file exists? */
  strcpy(temp, basefile); strcat(temp, ".hdr");
  if(access(temp, 0) == -1) {
    strcpy(temp, basefile); strcat(temp, ".img.hdr");
    if(access(temp, 0) == -1) {
      if(ANALYZE_TEST) printf("\n  hdr file not found or accessible.\n");
      return(0);
    }
  }
  /* Is this Analyze header file? */
  if((ret=anaReadHeader(temp, &h))!=0) {
    if(ANALYZE_TEST) {
      printf("\n  %s was not identified as Analyze header file (%d).\n",
        temp, ret);
    }
    return(0);
  }
  /* Preserve header filename */
  if(hdrfile!=NULL) strcpy(hdrfile, temp);

  /* Image file exists? */
  strcpy(temp, basefile); strcat(temp, ".img");
  if(access(temp, 0) == -1) {
    if(ANALYZE_TEST) printf("\n  %s not found or accessible.\n", temp);
    return(0);
  }
  /* Preserve image filename */
  if(imgfile!=NULL) strcpy(imgfile, temp);

  /* SIF exists? */
  strcpy(temp, basefile); strcat(temp, ".sif");
  if(access(temp, 0) == -1) {
    strcpy(temp, basefile); strcat(temp, ".img.sif");
    if(access(temp, 0) == -1) {
      if(ANALYZE_TEST) printf("\n  SIF not found or accessible.\n");
     if(siffile!=NULL) strcpy(siffile, "");
      return(1); // but otherwise ok
    }
  }
  /* Preserve SIF filename */
  if(siffile!=NULL) strcpy(siffile, temp);
  return(2);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Read analyze header contents.
 *
 * @param filename name of file to read (including path and extension)
 * @param h Pointer to previously allocated header structure
 * @return 0, if ok, 1 if invalid input, 2 if file cannot be opened, 3 if
 * header cound not be read, 4 if header image dimension could not be read
 */
int anaReadHeader(char *filename, ANALYZE_DSR *h) {
  unsigned char buf1[ANALYZE_HEADER_KEY_SIZE];
  unsigned char buf2[ANALYZE_HEADER_IMGDIM_SIZE];
  unsigned char buf3[ANALYZE_HEADER_HISTORY_SIZE];
  int little; /* 1 if current platform is little endian (i386), else 0 */
  FILE *fp;
  int ret, nr=0, s1, s2, same_order;

  if(ANALYZE_TEST) printf("anaReadHeader(%s, *dsr)\n", filename);

  /* Check arguments */
  if(strlen(filename)<1 || h==NULL) return(1);
  little=little_endian();
  /* Open file */
  fp=fopen(filename, "rb"); if(fp==NULL) return(2);
  /* Get file size */
  nr=0; while((ret=fgetc(fp))!=EOF) nr++; rewind(fp);
  if(nr<1) {fclose(fp); return(3);}
  /* Read Analyze header key */
  if(fread(buf1, ANALYZE_HEADER_KEY_SIZE, 1, fp)<1) return(3);
  /* Read Analyze header image dimension */
  if(fread(buf2, ANALYZE_HEADER_IMGDIM_SIZE, 1, fp)<1) return(3);
  /* Read Analyze header image data history */
  memset(buf3, 0, sizeof(ANALYZE_HEADER_HISTORY));
  ret=fread(buf3, ANALYZE_HEADER_HISTORY_SIZE, 1, fp);
  if(ANALYZE_TEST>1 && ret<1) printf(" complete data_history not found.\n");
  /* Close file */
  fclose(fp);
  /* Compare file size from header contents to the calculated value */
  /* to determine whether Analyze file is in little or big endian */
  memcpy(&s1, buf1+0, 4); s2=s1; swawbip(&s2, 4);
  if(abs(s1-nr)<abs(s2-nr)) same_order=1; else same_order=0;
  if(ANALYZE_TEST>1) printf("same byte order: %d (s1=%d s2=%d nr=%d)\n",
    same_order, s1, s2, nr);
  if(same_order) h->little=little;
  else {if(little) h->little=0; else h->little=1;}

  /* Set key header structure contents */
  if(!same_order) swawbip(buf1+0, 4); memcpy(&h->hk.sizeof_hdr, buf1+0, 4);
  memcpy(h->hk.data_type, buf1+4, 10);
  memcpy(h->hk.db_name, buf1+14, 18);
  if(!same_order) swawbip(buf1+32, 4); memcpy(&h->hk.extents, buf1+32, 4);
  if(!same_order) swabip(buf1+36, 2); memcpy(&h->hk.session_error, buf1+36, 2);
  memcpy(&h->hk.regular, buf1+38, 1);
  memcpy(&h->hk.hkey_un0, buf1+39, 1);

  /* Set image dimension header structure contents */
  if(!same_order) swabip(buf2+0, 16); memcpy(h->dime.dim, buf2+0, 16);
  if(!same_order) swabip(buf2+16, 2); memcpy(&h->dime.unused8, buf2+16, 2);
  if(!same_order) swabip(buf2+18, 2); memcpy(&h->dime.unused9, buf2+18, 2);
  if(!same_order) swabip(buf2+20, 2); memcpy(&h->dime.unused10, buf2+20, 2);
  if(!same_order) swabip(buf2+22, 2); memcpy(&h->dime.unused11, buf2+22, 2);
  if(!same_order) swabip(buf2+24, 2); memcpy(&h->dime.unused12, buf2+24, 2);
  if(!same_order) swabip(buf2+26, 2); memcpy(&h->dime.unused13, buf2+26, 2);
  if(!same_order) swabip(buf2+28, 2); memcpy(&h->dime.unused14, buf2+28, 2);
  if(!same_order) swabip(buf2+30, 2); memcpy(&h->dime.datatype, buf2+30, 2);
  if(!same_order) swabip(buf2+32, 2); memcpy(&h->dime.bitpix, buf2+32, 2);
  if(!same_order) swabip(buf2+34, 2); memcpy(&h->dime.dim_un0, buf2+34, 2);
  if(!same_order) swawbip(buf2+36, 32); memcpy(h->dime.pixdim, buf2+36, 32);
  if(!same_order) swawbip(buf2+68, 4); memcpy(&h->dime.vox_offset, buf2+68, 4);
  if(!same_order) swawbip(buf2+72, 4); memcpy(&h->dime.funused1, buf2+72, 4);
  if(!same_order) swawbip(buf2+76, 4); memcpy(&h->dime.funused2, buf2+76, 4);
  if(!same_order) swawbip(buf2+80, 4); memcpy(&h->dime.funused3, buf2+80, 4);
  if(!same_order) swawbip(buf2+84, 4); memcpy(&h->dime.cal_max, buf2+84, 4);
  if(!same_order) swawbip(buf2+88, 4); memcpy(&h->dime.cal_min, buf2+88, 4);
  if(!same_order) swawbip(buf2+92, 4); memcpy(&h->dime.compressed, buf2+92, 4);
  if(!same_order) swawbip(buf2+96, 4); memcpy(&h->dime.verified, buf2+96, 4);
  if(!same_order) swawbip(buf2+100, 4); memcpy(&h->dime.glmax, buf2+100, 4);
  if(!same_order) swawbip(buf2+104, 4); memcpy(&h->dime.glmin, buf2+104, 4);

  /* Set data history header structure contents */
  memcpy(h->hist.descrip, buf3+0, 80);
  memcpy(h->hist.aux_file, buf3+80, 24);
  memcpy(&h->hist.orient, buf3+104, 1);
  memcpy(h->hist.originator, buf3+105, 10);
  memcpy(h->hist.generated, buf3+115, 10);
  memcpy(h->hist.scannum, buf3+125, 10);
  memcpy(h->hist.patient_id, buf3+135, 10);
  memcpy(h->hist.exp_date, buf3+145, 10);
  memcpy(h->hist.exp_time, buf3+155, 10);
  memcpy(h->hist.hist_un0, buf3+165, 3);
  if(!same_order) swawbip(buf3+168, 4); memcpy(&h->hist.views, buf3+168, 4);
  if(!same_order) swawbip(buf3+172, 4); memcpy(&h->hist.vols_added, buf3+172, 4);
  if(!same_order) swawbip(buf3+176, 4); memcpy(&h->hist.start_field, buf3+176, 4);
  if(!same_order) swawbip(buf3+180, 4); memcpy(&h->hist.field_skip, buf3+180, 4);
  if(!same_order) swawbip(buf3+184, 4); memcpy(&h->hist.omax, buf3+184, 4);
  if(!same_order) swawbip(buf3+188, 4); memcpy(&h->hist.omin, buf3+188, 4);
  if(!same_order) swawbip(buf3+192, 4); memcpy(&h->hist.smax, buf3+192, 4);
  if(!same_order) swawbip(buf3+196, 4); memcpy(&h->hist.smin, buf3+196, 4);

  /* Check header contents */
  if(h->hk.extents!=16384 && h->hk.extents!=0) {
    if(ANALYZE_TEST>1) printf("hk.extents := %d\n", h->hk.extents);
    return(11);
  }
  if(h->hk.regular!='r') {
    if(ANALYZE_TEST>1) printf("hk.regular := %c\n", h->hk.regular);
    return(12);
  }

  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Write analyze header contents. Header field 'little' is used to determine the required byte order.
 *
 * @param filename name of file to read (including path and extension)
 * @param h pointer to Analyze header structure
 * @returns 0, if ok, 1 if invalid input, 2 if file could not be opened for writing, 
 * 3 if data header write failed, 4 if image dimension wri te failed, 5 if header history wri te failed
 */
int anaWriteHeader(char *filename, ANALYZE_DSR *h) {
  unsigned char buf1[ANALYZE_HEADER_KEY_SIZE];
  unsigned char buf2[ANALYZE_HEADER_IMGDIM_SIZE];
  unsigned char buf3[ANALYZE_HEADER_HISTORY_SIZE];
  FILE *fp;
  int same_order, little;


  if(ANALYZE_TEST) printf("anaWriteHeader(%s, *dsr)\n", filename);

  /* Check arguments */
  if(strlen(filename)<1 || h==NULL) return(1);
  little=little_endian();
  if(little==h->little) same_order=1; else same_order=0;
  
  /* Copy header contents into buffers */
  /* Header key */
  memset(buf1, 0, sizeof(ANALYZE_HEADER_KEY_SIZE));
  memcpy(buf1+0, &h->hk.sizeof_hdr, 4); if(!same_order) swawbip(buf1+0, 4);
  memcpy(buf1+4, &h->hk.data_type, 10);
  memcpy(buf1+14, &h->hk.db_name, 18);
  memcpy(buf1+32, &h->hk.extents, 4); if(!same_order) swawbip(buf1+32, 4);
  memcpy(buf1+36, &h->hk.session_error, 2); if(!same_order) swabip(buf1+36, 2);
  memcpy(buf1+38, &h->hk.regular, 1);
  memcpy(buf1+39, &h->hk.hkey_un0, 1);
  /* Image dimension */
  memset(buf2, 0, sizeof(ANALYZE_HEADER_IMGDIM_SIZE));
  memcpy(buf2+0, h->dime.dim, 16); if(!same_order) swabip(buf2+0, 16);
  memcpy(buf2+16, &h->dime.unused8, 2); if(!same_order) swabip(buf2+16, 2);
  memcpy(buf2+18, &h->dime.unused9, 2); if(!same_order) swabip(buf2+18, 2);
  memcpy(buf2+20, &h->dime.unused10, 2); if(!same_order) swabip(buf2+20, 2);
  memcpy(buf2+22, &h->dime.unused11, 2); if(!same_order) swabip(buf2+22, 2);
  memcpy(buf2+24, &h->dime.unused12, 2); if(!same_order) swabip(buf2+24, 2);
  memcpy(buf2+26, &h->dime.unused13, 2); if(!same_order) swabip(buf2+26, 2);
  memcpy(buf2+28, &h->dime.unused14, 2); if(!same_order) swabip(buf2+28, 2);
  memcpy(buf2+30, &h->dime.datatype, 2); if(!same_order) swabip(buf2+30, 2);
  memcpy(buf2+32, &h->dime.bitpix, 2); if(!same_order) swabip(buf2+32, 2);
  memcpy(buf2+34, &h->dime.dim_un0, 2); if(!same_order) swabip(buf2+34, 2);
  memcpy(buf2+36, h->dime.pixdim, 32); if(!same_order) swawbip(buf2+36, 32);
  memcpy(buf2+68, &h->dime.vox_offset, 4); if(!same_order) swawbip(buf2+68, 4);
  memcpy(buf2+72, &h->dime.funused1, 4); if(!same_order) swawbip(buf2+72, 4);
  memcpy(buf2+76, &h->dime.funused2, 4); if(!same_order) swawbip(buf2+76, 4);
  memcpy(buf2+80, &h->dime.funused3, 4); if(!same_order) swawbip(buf2+80, 4);
  memcpy(buf2+84, &h->dime.cal_max, 4); if(!same_order) swawbip(buf2+84, 4);
  memcpy(buf2+88, &h->dime.cal_min, 4); if(!same_order) swawbip(buf2+88, 4);
  memcpy(buf2+92, &h->dime.compressed, 4); if(!same_order) swawbip(buf2+92, 4);
  memcpy(buf2+96, &h->dime.verified, 4); if(!same_order) swawbip(buf2+96, 4);
  memcpy(buf2+100, &h->dime.glmax, 4); if(!same_order) swawbip(buf2+100, 4);
  memcpy(buf2+104, &h->dime.glmin, 4); if(!same_order) swawbip(buf2+104, 4);
  /* Data history */
  memset(buf3, 0, sizeof(ANALYZE_HEADER_HISTORY_SIZE));
  memcpy(buf3+0, &h->hist.descrip, 80);
  memcpy(buf3+80, &h->hist.aux_file, 24);
  memcpy(buf3+104, &h->hist.orient, 1);
  memcpy(buf3+105, &h->hist.originator, 10);
  memcpy(buf3+115, &h->hist.generated, 10);
  memcpy(buf3+125, &h->hist.scannum, 10);
  memcpy(buf3+135, &h->hist.patient_id, 10);
  memcpy(buf3+145, &h->hist.exp_date, 10);
  memcpy(buf3+155, &h->hist.exp_time, 10);
  memcpy(buf3+165, &h->hist.hist_un0, 3);
  memcpy(buf3+168, &h->hist.views, 4); if(!same_order) swawbip(buf3+168, 4);
  memcpy(buf3+172, &h->hist.vols_added, 4); if(!same_order) swawbip(buf3+172, 4);
  memcpy(buf3+176, &h->hist.start_field, 4); if(!same_order) swawbip(buf3+176, 4);
  memcpy(buf3+180, &h->hist.field_skip, 4); if(!same_order) swawbip(buf3+180, 4);
  memcpy(buf3+184, &h->hist.omax, 4); if(!same_order) swawbip(buf3+184, 4);
  memcpy(buf3+188, &h->hist.omin, 4); if(!same_order) swawbip(buf3+188, 4);
  memcpy(buf3+192, &h->hist.smax, 4); if(!same_order) swawbip(buf3+192, 4);
  memcpy(buf3+196, &h->hist.smin, 4); if(!same_order) swawbip(buf3+196, 4);

  /* Open header file for write */
  fp=fopen(filename, "wb"); if(fp==NULL) return(2);
  /* Write header key */
  if(fwrite(buf1, 1, ANALYZE_HEADER_KEY_SIZE, fp) != ANALYZE_HEADER_KEY_SIZE) {
    fclose(fp); return(3);
  }
  /* Write image dimension */
  if(fwrite(buf2, 1, ANALYZE_HEADER_IMGDIM_SIZE, fp) != ANALYZE_HEADER_IMGDIM_SIZE) {
    fclose(fp); return(4);
  }
  /* Write data history */
  if(fwrite(buf3, 1, ANALYZE_HEADER_HISTORY_SIZE, fp) != ANALYZE_HEADER_HISTORY_SIZE) {
    fclose(fp); return(5);
  }
  fclose(fp);

  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Print the contents of Analyze header to specified file pointer.
 *
 * @param h analyze header stucture
 * @param fp file pointer
 * @return Returns 0 if ok, 1 if invalid input
 */
int anaPrintHeader(ANALYZE_DSR *h, FILE *fp) {
  int i;

  if(fp==NULL || h==NULL) return(1);
  fprintf(fp, "original_byte_order := %d (1=little, 0=big)\n", h->little);
  /* Key */
  fprintf(fp, "header_key.sizeof_hdr := %d\n", h->hk.sizeof_hdr);
  fprintf(fp, "header_key.data_type := %.10s\n", h->hk.data_type);
  fprintf(fp, "header_key.db_name := %.18s\n", h->hk.db_name);
  fprintf(fp, "header_key.extents := %d\n", h->hk.extents);
  fprintf(fp, "header_key.session_error := %d\n", h->hk.session_error);
  fprintf(fp, "header_key.regular := %d (%c)\n", (int)h->hk.regular, h->hk.regular);
  fprintf(fp, "header_key.hkey_un0 := %d\n", (int)h->hk.hkey_un0);
  /* Image dimension */
  fprintf(fp, "header_image_dimension.dim :=");
  for(i=0; i<8; i++) fprintf(fp, " %d", h->dime.dim[i]);
  fprintf(fp, "\n");
  fprintf(fp, "header_image_dimension.unused8 := %d\n", h->dime.unused8);
  fprintf(fp, "header_image_dimension.unused9 := %d\n", h->dime.unused9);
  fprintf(fp, "header_image_dimension.unused10 := %d\n", h->dime.unused10);
  fprintf(fp, "header_image_dimension.unused11 := %d\n", h->dime.unused11);
  fprintf(fp, "header_image_dimension.unused12 := %d\n", h->dime.unused12);
  fprintf(fp, "header_image_dimension.unused13 := %d\n", h->dime.unused13);
  fprintf(fp, "header_image_dimension.unused14 := %d\n", h->dime.unused14);
  fprintf(fp, "header_image_dimension.datatype := %d\n", h->dime.datatype);
  fprintf(fp, "header_image_dimension.bitpix := %d\n", h->dime.bitpix);
  fprintf(fp, "header_image_dimension.dim_un0 := %d\n", h->dime.dim_un0);
  fprintf(fp, "header_image_dimension.pixdim :=");
  for(i=0; i<8; i++) fprintf(fp, " %g", h->dime.pixdim[i]);
  fprintf(fp, "\n");
  fprintf(fp, "header_image_dimension.vox_offset := %g\n", h->dime.vox_offset);
  fprintf(fp, "header_image_dimension.funused1 := %g\n", h->dime.funused1);
  fprintf(fp, "header_image_dimension.funused2 := %g\n", h->dime.funused2);
  fprintf(fp, "header_image_dimension.funused3 := %g\n", h->dime.funused3);
  fprintf(fp, "header_image_dimension.cal_max := %g\n", h->dime.cal_max);
  fprintf(fp, "header_image_dimension.cal_min := %g\n", h->dime.cal_min);
  fprintf(fp, "header_image_dimension.compressed := %g\n", h->dime.compressed);
  fprintf(fp, "header_image_dimension.verified := %g\n", h->dime.verified);
  fprintf(fp, "header_image_dimension.glmax := %d\n", h->dime.glmax);
  fprintf(fp, "header_image_dimension.glmin := %d\n", h->dime.glmin);
  /* Data history */
  fprintf(fp, "header_data_history.descrip := %s.80\n", h->hist.descrip);
  fprintf(fp, "header_data_history.aux_file := %.24s\n", h->hist.aux_file);
  fprintf(fp, "header_data_history.orient := %d\n", (int)h->hist.orient);
  fprintf(fp, "header_data_history.originator := %.10s\n", h->hist.originator);
  fprintf(fp, "header_data_history.generated := %.10s\n", h->hist.generated);
  fprintf(fp, "header_data_history.scannum := %.10s\n", h->hist.scannum);
  fprintf(fp, "header_data_history.patient_id := %.10s\n", h->hist.patient_id);
  fprintf(fp, "header_data_history.exp_date := %.10s\n", h->hist.exp_date);
  fprintf(fp, "header_data_history.exp_time := %.10s\n", h->hist.exp_time);
  fprintf(fp, "header_data_history.hist_un0 := %.3s\n", h->hist.hist_un0);
  fprintf(fp, "header_data_history.views := %d\n", h->hist.views);
  fprintf(fp, "header_data_history.vols_added := %d\n", h->hist.vols_added);
  fprintf(fp, "header_data_history.start_field := %d\n", h->hist.start_field);
  fprintf(fp, "header_data_history.field_skip := %d\n", h->hist.field_skip);
  fprintf(fp, "header_data_history.omax := %d\n", h->hist.omax);
  fprintf(fp, "header_data_history.omin := %d\n", h->hist.omin);
  fprintf(fp, "header_data_history.smax := %d\n", h->hist.smax);
  fprintf(fp, "header_data_history.smin := %d\n", h->hist.smin);

  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Read Analyze 7.5 image data, convert byte order if necessary,
 * and scale values to floats. Reads only one frame at a time!
 *
 * @param fp file opened previously in binary mode
 * @param h analyze header read previously
 * @param frame frame number to read [1..number of frames]
 * @param data pointer to image float data allocated previously
 */
int anaReadImagedata(FILE *fp, ANALYZE_DSR *h, int frame, float *data) {
  int dimNr, dimx, dimy, dimz=1, dimt=1, pxlNr=0;
  int i, n, little, start_pos, rawSize;
  char *mdata, *mptr;
  float f, *fptr;
  short int *sptr;
  int *iptr;
  double d;


  if(ANALYZE_TEST) printf("anaReadImagedata(fp, h, %d, data)\n", frame);

  /* Check the arguments */
  if(frame<=0 || fp==NULL || h==NULL || data==NULL) return(1);

  /* Get the image dimensions from header */
  dimNr=h->dime.dim[0]; if(dimNr<2) return(2);
  dimx=h->dime.dim[1];
  dimy=h->dime.dim[2];
  if(dimNr>2) dimz=h->dime.dim[3];
  if(dimNr>3) dimt=h->dime.dim[4]; if(frame>dimt) return(3);
  pxlNr=dimx*dimy*dimz; if(pxlNr<1) return(4);

  /* Allocate memory for the binary data */
  if(h->dime.bitpix<8) return(5); /* We don't support bit data */
  rawSize=pxlNr*(h->dime.bitpix/8); if(rawSize<1) return(5);
  if(ANALYZE_TEST>0) printf("  pxlNr=%d  rawSize=%d\n", pxlNr, rawSize);
  mdata=(char*)malloc(rawSize); if(mdata==NULL) return(11);

  /* Seek the start of current frame data */
  start_pos=(frame-1)*rawSize;
  n=(int)h->dime.vox_offset; if((n>0 && frame==1) || (n<0)) start_pos+=abs(n);
  if(ANALYZE_TEST>2) printf("start_pos=%d\n", start_pos);
  fseek(fp, start_pos, SEEK_SET);
  if(ftell(fp)!=start_pos) {
    if(ANALYZE_TEST>5) printf("could not move to start_pos\n");
    free(mdata); return(7);
  }

  /* Read the data */
  mptr=mdata;
  if((n=fread(mptr, rawSize, 1, fp)) < 1) {
    if(ANALYZE_TEST>5)
      printf("could read only %d bytes when request was %d\n", n, rawSize);
    free(mdata); return(8);
  }

  /* Convert byte order if necessary */
  little=little_endian(); mptr=mdata;
  if(little!=h->little) {
    if(ANALYZE_TEST>0) printf("byte conversion\n");
    switch(h->dime.bitpix) {
      case 8: /* no conversion needed */ break;
      case 16: swabip(mptr, rawSize); break;
      case 32: swawbip(mptr, rawSize); break;
      case 64: swawbip(mptr, rawSize); break;
      default: 
        if(ANALYZE_TEST>5)
          printf("unsupported anahdr.dime.bitpix := %d\n", h->dime.bitpix);
        free(mdata); return(5);
    }
  }

  /* Get scale factor */
  f=1.0;
  if(h->dime.funused1>0.0) f*=h->dime.funused1;

  /* Copy data to float pixel values */
  mptr=mdata; fptr=data;
  switch(h->dime.datatype) {
    case ANALYZE_DT_UNSIGNED_CHAR:
      if(h->dime.bitpix!=8) {
        if(ANALYZE_TEST>5)
          printf("invalid combination of datatype and bitpix (%d, %d)\n",
            h->dime.datatype, h->dime.bitpix);
        free(mdata); return(5);
      }
      for(i=0; i<pxlNr; i++, mptr++, fptr++) *fptr=f*(float)(*mptr);
      break;
    case ANALYZE_DT_SIGNED_SHORT:
      if(h->dime.bitpix!=16) {
        if(ANALYZE_TEST>5)
          printf("invalid combination of datatype and bitpix (%d, %d)\n",
            h->dime.datatype, h->dime.bitpix);
        free(mdata); return(5);
      }
      for(i=0; i<pxlNr; i++, mptr+=2, fptr++) {
        sptr=(short int*)mptr; *fptr=f*(float)(*sptr);
      }
      break;
    case ANALYZE_DT_SIGNED_INT:
      if(h->dime.bitpix!=16 && h->dime.bitpix!=32) {
        if(ANALYZE_TEST>5)
          printf("invalid combination of datatype and bitpix (%d, %d)\n",
            h->dime.datatype, h->dime.bitpix);
        free(mdata); return(5);
      }
      if(h->dime.bitpix==16) {
        for(i=0; i<pxlNr; i++, mptr+=4, fptr++) {
          iptr=(int*)mptr; *fptr=f*(float)(*iptr);
        }
      } else if(h->dime.bitpix==32) {
        for(i=0; i<pxlNr; i++, mptr+=4, fptr++) {
          iptr=(int*)mptr; *fptr=f*(float)(*iptr);
        }
      }
      break;
    case ANALYZE_DT_FLOAT:
      if(h->dime.bitpix!=16 && h->dime.bitpix!=32) {
        if(ANALYZE_TEST>5)
          printf("invalid combination of datatype and bitpix (%d, %d)\n",
            h->dime.datatype, h->dime.bitpix);
        free(mdata); return(5);
      }
      if(h->dime.bitpix==16) {
        memcpy(fptr, mptr, pxlNr*4);
        for(i=0; i<pxlNr; i++, fptr++) *fptr*=f;
      } else if(h->dime.bitpix==32) {
        memcpy(fptr, mptr, pxlNr*4);
        for(i=0; i<pxlNr; i++, fptr++) *fptr*=f;
      }
      break;
    case ANALYZE_DT_COMPLEX:
      if(h->dime.bitpix!=32) {
        if(ANALYZE_TEST>5)
          printf("invalid combination of datatype and bitpix (%d, %d)\n",
            h->dime.datatype, h->dime.bitpix);
        free(mdata); return(5);
      }
      if(h->dime.bitpix==32) {
        memcpy(fptr, mptr, pxlNr*4);
        for(i=0; i<pxlNr; i++, fptr++) *fptr*=f;
      }
      break;
    case ANALYZE_DT_DOUBLE:
      if(h->dime.bitpix!=32) {
        if(ANALYZE_TEST>5)
          printf("invalid combination of datatype and bitpix (%d, %d)\n",
            h->dime.datatype, h->dime.bitpix);
        free(mdata); return(5);
      }
      for(i=0; i<pxlNr; i++, mptr+=8, fptr++) {
        memcpy(&d, mptr, 8); *fptr=f*d;
      }
      break;
    default:
      if(ANALYZE_TEST>5)
        printf("unsupported anahdr.dime.datatype := %d\n", h->dime.datatype);
      free(mdata); return(5);
  }

  free(mdata);
  if(ANALYZE_TEST>1) printf("anaReadImagedata() succeeded\n");
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Check whether Analyze image is flipped in z-direction when it
 * is read from/written to file (x,y-flipping is done always)
 *
 * @return 1 if Analyze data is flipped
 */
int anaFlipping() {
  int ret;
  char *cptr;

  /* Is there an environment variable name for flipping? */
  cptr=getenv("ANALYZE_FLIP");
  if(cptr==NULL) cptr=getenv("ANALYZE_FLIPPING");
  if(cptr==NULL) cptr=getenv("analyze_flip");
  if(cptr==NULL) cptr=getenv("analyze_flipping");
  if(cptr==NULL) {
    if(ANALYZE_TEST>1) printf("ANALYZE_FLIP = not defined\n");
    ret=ANALYZE_FLIP_DEFAULT; /* if not, then use default value */
  } else {
    if(ANALYZE_TEST>1) printf("ANALYZE_FLIP = '%s'\n", cptr);
    if(*cptr=='y' || *cptr=='Y' || *cptr=='1') ret=1;
    else if(*cptr=='n' || *cptr=='N' || *cptr=='0') ret=0;
    else ret=ANALYZE_FLIP_DEFAULT;
  }
  if(ANALYZE_TEST) printf("anaFlipping()=%d\n", ret);
  return(ret);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Remove header and data files belonging to specified Analyze 7.5 database.
 * SIF is not deleted in any case.
 * @return 0 when call was successful, 1 if header file deletion failed, 
 * 2 if data file deletion failed.
 * Call is considered successful, if database does not exist initially.
 */
int anaRemove(const char *dbname) {
  char datfile[FILENAME_MAX], hdrfile[FILENAME_MAX], siffile[FILENAME_MAX];

  if(ANALYZE_TEST) printf("anaRemove(%s)\n", dbname);
  if(anaDatabaseExists(dbname, hdrfile, datfile, siffile)==0) return 0;
  if(ANALYZE_TEST>2) printf("  removing %s and %s\n", hdrfile, datfile);
  if(remove(hdrfile)!=0) return 1;
  if(remove(datfile)!=0) return 2;
  return 0;
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Check if Analyze 7.5 filename was given accidentally with extension.
 * Remove the extension if necessary
 *
 * @param fname full name of file
 */
void anaRemoveFNameExtension(char *fname) {
  char *cptr;
  cptr=strrchr(fname, '.'); if(cptr==NULL) return;
  if(strcasecmp(cptr, ".")==0 || strcasecmp(cptr, ".img")==0 ||
     strcasecmp(cptr, ".hdr")==0 || strcasecmp(cptr, ".sif")==0)
	*cptr=(char)0;
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Check if Analyze database files exist (*.hdr, *.img, and optionally *.sif).
 *
 * @param dbname analyze database name that is tested. String may contain standard
 * extensions .hdr, .img or .sif.
 * @param hdrfile pointer to an allocated string, where existing header filename is written.
 *  If not found, then set to "". NULL can be entered, if not needed.
 * @param imgfile pointer to an allocated string, where existing image filename is written.
 *  If not found, then set to "". NULL can be entered, if not needed.
 * @param siffile pointer to an allocated string, where existing sif filename is written.
 *  If not found, then set to "". NULL can be entered, if not needed.
 * @return 0, if files do not exist, 1 if .img and .hdr do exist,
 * and 2, if also .sif exists.
 */
int anaDatabaseExists(const char *dbname, char *hdrfile, char *imgfile, char *siffile) {
  char temp[FILENAME_MAX], database[FILENAME_MAX];
  int checked=0;

  if(ANALYZE_TEST)
    printf("\nanaDatabaseExists(%s, *hdrfile, *imgfile, *siffile)\n", dbname);

  /* Check the input */
  if(hdrfile!=NULL) strcpy(hdrfile, "");
  if(imgfile!=NULL) strcpy(imgfile, "");
  if(siffile!=NULL) strcpy(siffile, "");
  if(dbname==NULL || strlen(dbname)==0) return(0);

  strcpy(database, dbname);
  while(1) {
    /* Header file? */
    strcpy(temp, database); strcat(temp, ".hdr");
    if(access(temp, 0) != -1) {
      /* Also image file? */
      strcpy(temp, database); strcat(temp, ".img");
      if(access(temp, 0) != -1) {
        if(hdrfile!=NULL) sprintf(hdrfile, "%s.hdr", database);
        if(imgfile!=NULL) sprintf(imgfile, "%s.img", database);
        /* Even SIF? */
        if(anaMakeSIFName(database, temp)==0) { /* yes! */
          if(siffile!=NULL) strcpy(siffile, temp); return(2);
        }
        /* Image and header files did exist anyway */
        return(1);
      }
    }
    if(checked==1) break;
    /* Try to remove extension */
    anaRemoveFNameExtension(database);
    checked=1;
  } /* try again */
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*!
 * Make SIF filename from Analyze 7.5 database name.
 *
 * @param dbname analyze 7.5 database name (including possible path but not extension
 * @param siffile pointer to allocated space for SIF filename
 * @return Returns 0 if SIF file is accessible, 1 if invalid input, 2 if sif name not found.
 */
int anaMakeSIFName(const char *dbname, char *siffile) {
  if(dbname==NULL || siffile==NULL) return(1);
  sprintf(siffile, "%s.sif", dbname); if(access(siffile, 0) != -1) return(0);
  sprintf(siffile, "%s.SIF", dbname); if(access(siffile, 0) != -1) return(0);
  sprintf(siffile, "%s.img.sif", dbname); if(access(siffile, 0) != -1) return(0);
  sprintf(siffile, "%s.IMG.SIF", dbname); if(access(siffile, 0) != -1) return(0);
  sprintf(siffile, "%s.sif", dbname); return(2);
}
/*****************************************************************************/

/*****************************************************************************/

