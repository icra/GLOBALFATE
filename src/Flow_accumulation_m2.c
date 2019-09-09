#include "lib.h"

void Flow_accumulation_m2(double **direction, double **hierarchy, double *area_m2, double **runoff, int nrows, int ncols, int nl){
     int i, k, c, r, unics, J;

     double *val_area = unique2(hierarchy, nl, &unics); /**works**/
     /** 3 seconds, no parallel **/
     for(i=0; i<nrows; i++)
          for(k=0; k<ncols; k++)
          if(runoff[i][k]>0) runoff[i][k] /= 1000;

     for(i=0; i<unics; i++)/* unics=19697 */
          /*#pragma omp parallel
          {
               omp_set_num_threads(4);
          #pragma omp for private(r,c,J)*/
          for(k=0; k<nl; k++)
               if(hierarchy[k][1]==val_area[i]){
               J=hierarchy[k][0];
               c=J/nrows;
               r=J-c*nrows;
               if(runoff[r][c]>0)
                    runoff[r][c] *= area_m2[r];
               else
                    runoff[r][c]=0;

               runoff_input(r, c, nrows, ncols, direction, runoff);
               }
          //}
}
