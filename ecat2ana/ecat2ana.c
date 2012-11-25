/******************************************************************************

  Copyright (c) 2003-2012 Turku PET Centre

  Program:       ecat2ana
  Description:   Convert ECAT 6.3 or 7 image format to Analyze 7.5.
  Compatibility: This code uses POSIX (non-ANSI) function mkdir().

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307 USA.

  Turku PET Centre hereby disclaims all copyright interest in the program.
  Juhani Knuuti
  Director, Professor
  Turku PET Centre, Turku, Finland, http://www.turkupetcentre.fi/

  Modification history:
  0.1   2003-10-04 Vesa Oikonen
        First created.
  0.2   2003-10-07 VO
        Processes multiple files.
        Gives automatically names for Analyze files.
  0.3   2003-12-05 VO
        Added option to override default setting for image flipping in
        x,y,z-directions.
        Added option to save all frames in separate files.
        Optional parameters can be given with '='.
  0.4   2003-12-11 VO
        Works even when input directory is not current directory.
        Change in error message.
  0.5   2004-02-05 VO
        Flipping option determines whether image is flipped in z-direction;
        image is always flipped in x,y-directions.
  0.6   2004-02-23 VO
        New libpet version: zoom factor is not written into/read from funused2,
        because that field is used by SPM2 to another purpose.
        2004-06-27 VO
        New libpet version.
  0.6.1 2004-11-07 VO
        Option processing rewritten.
        New libpet version.
  0.6.2 2005-02-11 VO
        Option -o did not work, now corrected.
  0.6.3 2005-10-10 CL/JJ
        Works with mingw.
        Output is flushed more often.
        Uses libtpcimgio_1_1_2 where imgWriteAnalyze() writes frame by frame.
        print_build() added.
  0.6.4 2007-02-05 VO
        Lots of additions and changes in user info text.
        Accepts option --silent.
        Options -v, --version, --build, -test[=<level>], -h, --help.
  1.0.0 2007-03-25 VO
        Changed to read and write data one frame at a time to conserve memory.
        Path is written with backslash in WIN32.
        Additions in info text.
        Identifies options -little and -big.
        Output path can set to current working directory with -o=
        Rewritten the checking for that original ECAT file is not overwritten.
        Error if no ECAT files given as argument.
  1.0.1 2009-10-29 VO
        Identifies options -pc, -sun and -sparc, as was told in user info text.
        Added option --verbose.
  1.0.2 2010-04-29 VO
        When writing Analyze file one frame in one file (option -frames),
        the scale factor is set for each frame separately.
        Added reference to ImageConverter in user help text.
  1.0.3 2012-09-25 VO
        Updated to compile with new library versions.
        Updated build info.
        Globbing turned on.
  1.0.4 2012-10-02 VO
        SIF can be saved optionally.
      

******************************************************************************/
#define PROGRAM "ecat2ana 1.0.4  (c) 2003-2012 by Turku PET Centre"
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
/*****************************************************************************/
#include "libtpcmisc.h"
#include "imgio.h"
#include "imgproc.h"
/*****************************************************************************/
int TEST=0;
int SILENT=0;
int VERBOSE=0;
/*****************************************************************************/
/* Local functions */
void print_usage();
void print_build();
/*****************************************************************************/

/*****************************************************************************/
/* Turn on the globbing of the command line, since it is disabled by default in
   mingw-w64 (_dowildcard=0); in MinGW32 define _CRT_glob instead, if necessary;
   In Unix&Linux wildcard command line processing is enabled by default. */
/*
#undef _CRT_glob
#define _CRT_glob -1
*/
int _dowildcard = -1;
/*****************************************************************************/

/*****************************************************************************/
/*
 *  main()
 */
int main(int argc, char *argv[])
{
  int      ai, ret, errorNr=0, flipping, fi=0, fileNr=0;
  int      ana_order=1; /* 0=little (intel), 1=big endian (sparc) */
  int      separate_frames=0;
  int      file_format=0;
  int      save_sif=0; // SIF is made (1) or not made (0)
  char     ctifile[FILENAME_MAX], dbname[FILENAME_MAX], *cptr;
  char     temp[FILENAME_MAX], outputdir[FILENAME_MAX], dbdir[FILENAME_MAX];
  IMG      img;
  float    fmin, fmax;
  SIF      sif;


  /*
   *  Get arguments
   */
  if(argc<2) {print_usage(); return(1);}
  outputdir[0]=ctifile[0]=dbname[0]=dbdir[0]=(char)0;
  flipping=anaFlipping();
  for(ai=1; ai<argc; ai++) if(*argv[ai]=='-') { /* options */
    cptr=argv[ai]+1; if(*cptr=='-') cptr++; if(cptr==NULL) continue;
    if(strncasecmp(cptr, "TEST", 4)==0) {
      cptr+=4; if(*cptr=='=') TEST=atoi(cptr+1); else TEST=1;
      continue;
    } else if(strcasecmp(cptr, "V")==0 || strcasecmp(cptr, "VERSION")==0 ||
              strcasecmp(cptr, "BUILD")==0) {
      print_build();
      return(0);
    } else if(strcasecmp(cptr, "H")==0 || strcasecmp(cptr, "HELP")==0) {
      print_usage(); return(0);
    } else if(strcasecmp(cptr, "SILENT")==0) {
      SILENT=1; VERBOSE=0; continue;
    } else if(strcasecmp(cptr, "VERBOSE")==0) {
      VERBOSE=1; SILENT=0; continue;
    } else if(strncasecmp(cptr, "FLIP", 4)==0) {
      cptr+=4; if(*cptr=='=') cptr++;
      if(*cptr=='n' || *cptr=='N') {flipping=0; continue;}
      else if(*cptr=='y' || *cptr=='Y') {flipping=1; continue;}
    } else if(strncasecmp(cptr, "FR", 2)==0) {
      separate_frames=1; continue;
    } else if(strcasecmp(cptr, "L")==0 || strcasecmp(cptr, "LITTLE")==0) {
      ana_order=0; continue;
    } else if(strcasecmp(cptr, "PC")==0) {
      ana_order=0; continue;
    } else if(strcasecmp(cptr, "I")==0) {
      ana_order=0; continue;
    } else if(strcasecmp(cptr, "B")==0 || strcasecmp(cptr, "BIG")==0) {
      ana_order=1; continue;
    } else if(strcasecmp(cptr, "SUN")==0) {
      ana_order=1; continue;
    } else if(strcasecmp(cptr, "SPARC")==0) {
      ana_order=1; continue;
    } else if(strncasecmp(cptr, "O", 1)==0) {
      cptr++; if(*cptr=='=') cptr++;
      if(strlen(cptr)>0) strcpy(outputdir, cptr); else strcpy(outputdir, ".");
      continue;
    } else if(strcasecmp(cptr, "SIF")==0) {
      save_sif=1; continue;
    }
    fprintf(stderr, "Error: invalid option '%s'\n", argv[ai]);
    return(1);
  } else {
    /* Other files are ECAT files which must exist */
    if(access(argv[ai], 0) == -1) {
      fprintf(stderr, "Error: file %s does not exist\n", argv[ai]);
      return(1);
    }
    fileNr++;
  }
  if(TEST) {
    printf("TEST := %d\n", TEST);
    printf("SILENT := %d\n", SILENT);
    printf("VERBOSE := %d\n", VERBOSE);
    printf("flipping := %d\n", flipping);
    printf("ana_order := %d\n", ana_order);
    printf("separate_frames := %d\n", separate_frames);
    printf("save_sif := %d\n", save_sif);
    printf("outputdir := %s\n", outputdir);
    printf("fileNr := %d\n", fileNr);
  }
  if(TEST>0) VERBOSE=1;
  if(TEST>2) IMG_TEST=ANALYZE_TEST=TEST-2;
  if(SILENT==0) {
    if(flipping==0) printf("image(s) will not be flipped in z-direction.\n");
    else printf("image(s) will be flipped in z-direction.\n");
  }
  if(fileNr<1) {
    fprintf(stderr, "Error: no ECAT files were specified.\n");
    return(1);
  }


  /*
   *  Process each ECAT file separately
   */
  if(VERBOSE) printf("processing...\n");
  sifInit(&sif); imgInit(&img); fileNr=0;
  for(ai=1; ai<argc; ai++) if(*argv[ai]!='-') {

    strcpy(ctifile, argv[ai]);
    if(SILENT==0) {fprintf(stdout, "%s : \n", ctifile); fflush(stdout);}
    imgEmpty(&img);
    sifEmpty(&sif);

    /*
     *  Make Analyze database name
     */
    /*  Make output directory name, if it was not given */
    if(strlen(outputdir)<1) {
      /* Get input directory */
      strcpy(dbdir, ctifile);
      cptr=strrchr(dbdir, '/'); if(cptr==NULL) cptr=strrchr(dbdir, '\\');
      if(cptr!=NULL) {cptr++; cptr[0]=(char)0;} else dbdir[0]=(char)0;
      /* Add default part */
      if(ana_order==1) strcat(dbdir, "ana_unix_files");
      else strcat(dbdir, "ana_pc_files");
    } else {
      strcpy(dbdir, outputdir);
      /* Remove trailing slashes */
      ret=strlen(dbdir)-1;
      if(dbdir[ret]=='/' || dbdir[ret]=='\\') dbdir[ret]=(char)0;
    }
    /* Create the subdirectory */
    if(access(dbdir, 0) == -1) {
      if(SILENT==0) {
        fprintf(stdout, "  Creating subdirectory %s\n", dbdir);
        fflush(stdout);
      }
#ifdef WIN32
      ret=mkdir(dbdir);
#else
      ret=mkdir(dbdir, 00775);
#endif
      if(ret!=0) {
        fprintf(stderr, "  Error: cannot create subdirectory.\n");
        fflush(stderr); errorNr++; continue;
      }
    }
    /* Combine path and filename */
    cptr=strrchr(ctifile, '/'); if(cptr==NULL) cptr=strrchr(ctifile, '\\');
    if(cptr==NULL) cptr=ctifile; else cptr++;
#ifdef WIN32
    sprintf(dbname, "%s\\%s", dbdir, cptr);
#else
    sprintf(dbname, "%s/%s", dbdir, cptr);
#endif
    cptr=strrchr(dbname, '.'); if(cptr!=NULL) cptr[0]=(char)0;
    if(VERBOSE) printf("  Analyze db_name: '%s'\n", dbname);
    /* Check if file exists: Analyze database can be overwritten, but existing
       ECAT 6.3 image (.img without .hdr) will lead to an error */
    if(separate_frames!=1 && anaExists(dbname)==0) {
      strcpy(temp, dbname); strcat(temp, ".img");
      if(access(temp, 0) != -1) {
        fprintf(stderr, "  Error: %s would be overwritten.\n", temp);
        fprintf(stdout, "  No conversion is done for %s\n", ctifile);
        fflush(stdout); fflush(stderr);
        errorNr++; continue;
      }
    }
    
    /*
     *  Remove possibly existing database
     */
    ret=anaRemove(dbname); if(ret) return STATUS_CANNOTERASE;
         
    /*
     *  Get the global min and max pixel values;
     *  those are needed for Analyze header, and for scaling pixels
     */
    if(SILENT==0) fprintf(stdout, "  searching min and max in %s\n", ctifile);
    ret=imgReadMinMax(ctifile, &fmin, &fmax);
    if(ret) {
      fprintf(stderr, "Error: %s\n", imgStatus(ret)); //if(TEST) imgInfo(&img);
      errorNr++; fflush(stderr); continue;
    }
    if(TEST) printf("    global_min := %g\n    global_max := %g\n", fmin, fmax);

    /*
     *  Get the frame number and allocate memory for SIF, if requested
     */
    if(save_sif) {
      /* Read information from input image for SIF */
      if(SILENT==0) printf("  reading header information\n");
      ret=imgReadHeader(ctifile, &img);
      if(ret) {
        fprintf(stderr, "Error: %s\n", imgStatus(ret));
        errorNr++; fflush(stderr); continue;
      }
      /* Allocate SIF to store frame times */
      ret=sifSetmem(&sif, img.dimt); if(ret!=0) {
        fprintf(stderr, "Error: out of memory.\n");
        errorNr++; fflush(stderr); continue;
      }
      sif.colNr=4; sif.version=1;
      imgEmpty(&img);
    }

    /*
     *  Conversion of ECAT file, one frame at a time
     */
    if(SILENT==0) fprintf(stdout, "  processing %s\n", ctifile);
    fi=0; imgEmpty(&img);
    while((ret=imgReadFrame(ctifile, fi+1, &img, 0)) == 0) {
      if(VERBOSE) printf("    frame %d\n", fi+1);
      /* reverse the flipping/no-flipping in imgWriteAnalyzeFrame, if required */
      if(flipping!=anaFlipping()) imgFlipPlanes(&img);
      /* Set the write file format including byte order */
      if(fi==0) file_format=img._fileFormat; // must be saved for reading next frame
      if(ana_order) img._fileFormat=IMG_ANA; else img._fileFormat=IMG_ANA_L;
      /* Write the frame in Analyze format */
      if(separate_frames!=1) { // into one Analyze file
        ret=imgWriteAnalyzeFrame(dbname, fi+1, &img, 0, fmin, fmax);
        if(ret) anaRemove(dbname);
        //printf("ret := %d\n", ret);
      } else { // each frame in a separate Analyze file
        sprintf(temp, "%s_fr%03d", dbname, fi+1); // make database name for frame
        ret=imgMinMax(&img, &fmin, &fmax);
        if(ret==0) ret=imgWriteAnalyzeFrame(temp, 1, &img, 0, fmin, fmax);
        if(ret) anaRemove(temp);
      }
      if(ret!=STATUS_OK) break;
      if(TEST>1) printf("    frame written.\n");
      /* Set SIF contents for this frame, if requested */
      if(save_sif) {
        sif.x1[fi]=img.start[0]; sif.x2[fi]=img.end[0];
        if(fi==0) {
          /* Set isotope */
          strcpy(sif.isotope_name, imgIsotope(&img) );
          /* Set studynumber */
          strcpy(sif.studynr, img.studyNr);
          /* Set scan start time */
          sif.scantime=img.scanStart;
        }
      }
      /* Prepare to the next frame */
      img._fileFormat=file_format;
      fi++;
    } // next frame
    if(SILENT==0) {
      if(ret==STATUS_NOMATRIX)
        fprintf(stdout, "  %d frame(s) processed.\n", fi);
    }
    if(ret!=STATUS_OK && ret!=STATUS_NOMATRIX) {
      fprintf(stderr, "Error: %s\n", imgStatus(ret)); //if(TEST) imgInfo(&img);
      errorNr++; fflush(stderr); continue;
    }
    imgEmpty(&img);

    /* Save SIF, if requested */
    if(save_sif) {
      sprintf(temp, "%s.sif", dbname);
      ret=sifWrite(&sif, temp);
      if(ret!=0) {
        fprintf(stderr, "Error: cannot write %s\n", temp);
        errorNr++; fflush(stderr); continue;
      }
      sifEmpty(&sif);
    }

  } /* next file */

  if(errorNr>0) return(errorNr+10);
  return(0);
}
/*****************************************************************************/

/*****************************************************************************/
/*
 *   print_usage()
 */
void print_usage()
{
  int i=0;
  static char *info[] = {
  " ",
  PROGRAM,
  " ",
  "Convert ECAT 6.3 images or ECAT 7 image volumes to Analyze 7.5 image format.",
  " ",
  "The resulting Analyze database consists of two files, Analyze image (*.img)",
  "and header file (*.hdr).",
  "Analyze image format does not contain information on the frame times.",
  "Frame times can be retrieved from SIF file. If SIF file does not exist,",
  "it can be created using program cti2sif and kept with the Analyze files.",
  " ",
  "By default, data is saved in big endian byte order (Sun Sparc).",
  " ",
  "Usage: ecat2ana [Options] <ECAT file(s)>",
  " ",
  "Options:",
  " -L[ittle] or -pc or -i",
  "     Data is saved in little endian (PC Intel) byte order.",
  " -B[ig] or -sun or -sparc",
  "     Data is saved in big endian (Sun Sparc, Motorola, PowerPC) byte order",
  "     (default).",
  " -O=<output path>",
  "     Data directory for Analyze files; by default the output directory",
  "     ana_unix_files or ana_pc_files is created under input directory.",
  " -flip=<y|n>",
  "     Override the default and environment variable ANALYZE_FLIP setting",
  "     by always flipping/not flipping image in z-direction (planes).",
  "     If environment variable is not set, then default is y.",
  "     Images are always flipped in x,y-directions.",
  " -SIF",
  "     SIF is saved with Analyze files; note that existing SIF will be",
  "     overwritten.",
  " -frames",
  "     Frames are saved as separate Analyze files.",
  " -h or --help",
  "     Print this message and exit.",
  " -v, --version or --build",
  "     Print software build information and exit.",
  " --verbose",
  "     Program prints more information about what it is doing.",
  " --silent",
  "     Program works silently, printing only error and warning messages.",
  " ",
  "Example:",
  "  ecat2ana *.v",
  " ",
  "Specific extensions to Analyze 7.5 format:",
  " -Scale factor to retain quantitation in image_dimension.funused1",
  " -Isotope halflife (sec) in image_dimension.funused3;",
  "  this does not imply whether data is corrected for decay or not.",
  " -String in data_history.descrip tells whether data is corrected for decay;",
  "  'Decay corrected.' or 'No decay correction.'",
  " ",
  "See also: ana2ecat, cti2sif, anabyteo, ana_lhdr, eframe, ecat2flo",
  "and ImageConverter (.NET version) in",
  "http://www.turkupetcentre.net/programs/tpc_csharp.html",
  " ",
  "Keywords: image processing, format conversion, ECAT, Analyze",
  " ",
  "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and",
  "you are welcome to redistribute it under GNU General Public License.",
  " ",
  0};
  while(info[i]!=0) printf("  %s\n", info[i++]);
}
/*****************************************************************************/

/*****************************************************************************/
/**
 * Prints build information to stdout.
 */
void print_build(){

  printf("\n %s\n", PROGRAM);
  printf("\n Build %s %s\n\n",__DATE__,__TIME__);
#if defined(__FILE__) && defined(__TIMESTAMP__)
  printf(" File %s last edited on %s\n", __FILE__, __TIMESTAMP__);
#endif
  printf(" _______________________________________________\n");

  libtpcimgp_print_build(stdout);
  libtpcimgio_print_build(stdout);
  libtpcmisc_print_build(stdout);
  printf(" _______________________________________________\n");

#if defined(__STDC_VERSION__)
  printf(" Version of C: %ld\n", __STDC_VERSION__);
#endif
#if defined(__STDC_ISO_10646__)
  printf(" Compiler ISO/IEC 10646 specification: %ld\n", __STDC_ISO_10646__);
#endif
#if defined(__GNUC__) && defined(__VERSION__)
  printf(" GNU C version: %s\n", __VERSION__);
#endif
#ifdef _OPENMP
  printf(" OpenMP version: %d\n", _OPENMP);
#endif
#if defined(__x86_64__) || defined(__LP64__) || defined(__ppc64__)
  printf(" Architecture: 64-bit\n");
#else
  printf(" Architecture: 32-bit\n");
#endif

  printf(" _______________________________________________\n\n");
}
/*****************************************************************************/

/*****************************************************************************/
