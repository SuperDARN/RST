/* noise_stat.c
   ============
   Author: R.J.Barnes & K.Baker & P.Ponomarenko
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



#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rmath.h"
#include "fitblk.h"

#include "badsmp.h"
#include "badlags.h"

#define PLIM (1.6)
#define ROOT_3 1.7


 double lag_power(struct complex *a) {
   return sqrt(a->x*a->x + a->y*a->y);
}

/**
Calculates cutoff power for fit_acf. The cutoff power is 
equal to the average of all non-zero lags of all acfs that have
a lag 0 power lower within a std dev of 1.6 * min_pwr
*/
double noise_stat(double mnpwr,struct FitPrm *fitted_prms,
                  struct FitACFBadSample *badsmp,
                  struct complex *acf) {

     double plim;
     int i, j, np0, npt;
     int *lag;
     double var, sigma, P, P2;
     double temp, fluct, low_lim, high_lim;

     plim = PLIM * mnpwr;

     P = 0.0;
     P2 = 0.0;
     var = 0.0;
     np0 = 0;
     npt = 0;

     lag=malloc(sizeof(int)*fitted_prms->mplgs);
     if (lag==NULL) return -1;
     memset(lag,0,sizeof(int)*fitted_prms->mplgs);

     for (i=0; i < fitted_prms->nrang; ++i) { 
          if ((acf[i*fitted_prms->mplgs].x > plim) || (acf[i*fitted_prms->mplgs].x <= 0.0)) continue;

          FitACFMarkBadLags((i+1), lag,badsmp, fitted_prms);
          ++np0;
          fluct = ((double) acf[i*fitted_prms->mplgs].x)/sqrt(fitted_prms->nave);
          low_lim = acf[i*fitted_prms->mplgs].x - 2.0*fluct;

          if (low_lim < 0) low_lim = low_lim + fluct;

          high_lim = acf[i*fitted_prms->mplgs].x + fluct;

          for (j=1; j < fitted_prms->mplgs; ++j) {
              if (lag[j] != GOOD) continue;
              temp = lag_power(&acf[i*fitted_prms->mplgs+j]);
              if (temp < low_lim || temp > high_lim) continue;
              ++npt;
              P = P + temp;
              P2 = P2 + temp*temp;
          }
     }

     free(lag);

     if (npt < 2) {
       /*  *signal = 0; */
     	return plim/sqrt((double) fitted_prms->nave);
     }

     P = P/npt;
     var = (P2 - P*P*npt)/((double) (npt-1));
     sigma = (var > 0.0) ? sqrt(var) : 0.0;

       /*if ((P >= sigma * ROOT_3) && (sigma > 0.0)) *signal = P;
       else *signal = 0.0; */
     return (P > sigma) ? P : sigma;
}
