/* merge_grid.c
   ==============
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "hlpstr.h"



struct GridData *rcd;
struct GridData *mrg;

struct OptionData opt;

int main(int argc,char *argv[]) {
  
 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */

  int arg;

  int old=0;
  int new=0;


  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  FILE *fp;   
  char *fname=NULL;
 
  double tme;
  int yr,mo,dy,hr,mt;
  double sc;
 
  rcd=GridMake();
  mrg=GridMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);

  OptionAdd(&opt,"vb",'x',&vb);


  arg=OptionProcess(1,argc,argv,&opt,NULL);

  old=!new;


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (arg<argc) fname=argv[arg];
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  if (old) {
    while (OldGridFread(fp,rcd) !=-1) {

      GridMerge(rcd,mrg);

      if (vb) {
        tme=rcd->st_time;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stderr,"%.4d %.2d %.2d %.2d %.2d %.2d merge vectors=%d\n",
		       yr,mo,dy,hr,mt,(int) sc,mrg->vcnum);
      }
      OldGridFwrite(stdout,mrg);
    }
  } else {

    while (GridFread(fp,rcd) !=-1) {

      GridMerge(rcd,mrg);

      if (vb) {
        tme=rcd->st_time;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stderr,"%.4d %.2d %.2d %.2d %.2d %.2d merge vectors=%d\n",
		       yr,mo,dy,hr,mt,(int) sc,mrg->vcnum);
      }
      GridFwrite(stdout,mrg);
    }
  }
  if (fp !=stdout) fclose(fp);
  

  return 0;
}






















