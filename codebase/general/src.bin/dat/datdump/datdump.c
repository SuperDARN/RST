/* 
 Copyright (C) 2021 SuperDARN Canada, University of Saskatchewan
 Author: Marina Schmidt

 Modification:

 LICENSE AND DISCLAIMER
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "zlib.h"
#include "dmap.h"
#include "rtypes.h"
#include "option.h"
#include "rprm.h"
#include "rawdata.h"
#include "oldrawread.h"
#include "rawwrite.h"
#include "hlpstr.h"


struct OptionData opt;
int arg=0;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: datdump --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  struct OldRawFp *datfp=NULL;
  struct RadarParm *scalars=NULL;
  struct RawData *arrays=NULL;
  
  struct DataMap *ptr=NULL;
  struct DataMapScalar *scalar;
  struct DataMapArray *array;
  
  char **tmp;
  unsigned char dflg=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;


  int c;
  int x,n,s;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"d",'x',&dflg);

  
  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }

  if (arg !=argc) {
      datfp = OldRawOpen(argv[arg], NULL);
      if (datfp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
  } 
  else {
      OptionPrintInfo(stdout,hlpstr);
      exit(0);
  }
    
  scalars=RadarParmMake();
  arrays=RawMake();
  while( OldRawRead(datfp, scalars, arrays) == 0) {
      ptr=DataMapMake();
      if (ptr == NULL)
          exit(-1);
      RadarParmEncode(ptr, scalars);
      RawEncode(ptr, scalars, arrays);
      fprintf(stdout,"scalars:\n");
      for (c=0; c<ptr->snum; c++) {
          scalar = ptr->scl[c];  
          switch (scalar->type) {
              case DATACHAR:
                  fprintf(stdout,"\tchar");
                  break;
              case DATASHORT:
                  fprintf(stdout,"\tshort");
                  break;
              case DATAINT:
                  fprintf(stdout,"\tint");
                  break;
              case DATALONG:
                  fprintf(stdout,"\tlong");
                  break;
              case DATAUCHAR:
                  fprintf(stdout,"\tunsigned char");
                  break;
              case DATAUSHORT:
                  fprintf(stdout,"\tunsigned short");
                  break;
              case DATAUINT:
                  fprintf(stdout,"\tunsigned int");
                  break;
              case DATAULONG:
                  fprintf(stdout,"\tunsigned long");
                  break;
              case DATAFLOAT:
                  fprintf(stdout,"\tfloat");
                  break;
              case DATADOUBLE:
                  fprintf(stdout,"\tdouble");
                  break;
              case DATASTRING:
                  fprintf(stdout,"\tstring");
                  break;
          }
          fprintf(stdout,"\t%c%s%c",'"',scalar->name,'"');
          fprintf(stdout," = ");
          switch (scalar->type) {
              case DATACHAR:
                  fprintf(stdout,"%d",*(scalar->data.cptr));
                  break;
              case DATASHORT:
                  fprintf(stdout,"%d",*(scalar->data.sptr));
                  break;
              case DATAINT:
                  fprintf(stdout,"%d",*(scalar->data.iptr));
                  break;
              case DATALONG:
                  fprintf(stdout,"%lld",*(scalar->data.lptr));
                  break;
              case DATAUCHAR:
                  fprintf(stdout,"%u",*(scalar->data.ucptr));
                  break;
              case DATAUSHORT:
                  fprintf(stdout,"%u",*(scalar->data.usptr));
                  break;
              case DATAUINT:
                  fprintf(stdout,"%u",*(scalar->data.uiptr));
                  break;
              case DATAULONG:
                  fprintf(stdout,"%llu",*(scalar->data.ulptr));
                  break;
              case DATAFLOAT:
                  fprintf(stdout,"%f",*(scalar->data.fptr));
                  break;
              case DATADOUBLE:
                  fprintf(stdout,"%lf",*(scalar->data.dptr));
                  break;
              case DATASTRING:
	              tmp=(char **) scalar->data.vptr;
                  if (*tmp !=NULL) 
                      fprintf(stdout,"%c%s%c",'"',*tmp,'"');
                  else 
                      fprintf(stdout,"%c%c",'"','"');
                  break;
          }
          fprintf(stdout,"\n");
      }
      fprintf(stdout,"arrays:\n");
      for (c=0;c<ptr->anum;c++) {
          array=ptr->arr[c];
          switch (array->type) {
              case DATACHAR:
                  fprintf(stdout,"\tchar");
                  break;
              case DATASHORT:
                  fprintf(stdout,"\tshort");
                  break;
              case DATAINT:
                  fprintf(stdout,"\tint");
                  break;
              case DATAFLOAT:
                  fprintf(stdout,"\tfloat");
                  break;
              case DATADOUBLE:
                  fprintf(stdout,"\tdouble");
                  break;
              case DATASTRING:
                  fprintf(stdout,"\tstring");
                  break;
          }
          fprintf(stdout,"\t%c%s%c",'"',array->name,'"');
          fprintf(stdout," ");
          for (x=0;x<array->dim;x++) 
              fprintf(stdout,"[%d]",array->rng[array->dim-1-x]);
          if (dflg) {
              fprintf(stdout,"=");
              n=1;
              for (x=0;x<array->dim;x++) 
                  n=array->rng[x]*n;
              for (x=0;x<n;x++) {
                  if (x % array->rng[0]==0) 
                      fprintf(stdout,"\n\t\t");
                  else if (x !=0) 
                      fprintf(stdout,",\t");
                  switch (array->type) {
                      case DATACHAR:
                          fprintf(stdout,"%d",array->data.cptr[x]);
                          break;
                      case DATASHORT:
                          fprintf(stdout,"%d",array->data.sptr[x]);
                          break;
                      case DATAINT:
                          fprintf(stdout,"%d",array->data.iptr[x]);
                          break;
                      case DATAFLOAT:
                          fprintf(stdout,"%f",array->data.fptr[x]);
                          break;
                      case DATADOUBLE:
                          fprintf(stdout,"%lf",array->data.dptr[x]);
                          break;	    
                      case DATASTRING:
                          tmp=(char **) array->data.vptr;
                          if (tmp[x] !=NULL) 
                              fprintf(stdout,"%c%s%c",'"',tmp[x],'"');
                          else 
                              fprintf(stdout,"%c%c",'"','"');
                          break;
                  }
              }  
              fprintf(stdout,"\n");
          } 
          else 
              fprintf(stdout,"\n");
      }
      DataMapFree(ptr);
  }
  OldRawClose(datfp);
  return 0;  
}