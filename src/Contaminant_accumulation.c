#include "lib.h"

double **Contaminant_accumulation(double **L, double **direction, double **hierarchy, int nl, double **flow_acc, double **RT, double *parameters, int nrows, int ncols){
     int i, j, unics, c, r, J;
     double dk = parameters[6];
     int conc = parameters[10];

     //Hierarchization of the fluvial network
     double *val_area = unique2(hierarchy, nl, &unics);

     for(i=0; i<unics; i++){
          //#pragma omp parallel
          {
               omp_set_num_threads(4);
          //#pragma omp for private(r,c,J)
          for(j=0; j<nl; j++)
               if(hierarchy[j][1]==val_area[i]){
               J=hierarchy[j][0];
               c=J/nrows;
               r=J-c*nrows;
               if(L[r][c]<0)
                L[r][c]=0;
               runoff_input(r, c, nrows, ncols, direction, L);
               L[r][c]*=exp(-dk*RT[r][c]);
          }
          }
     }
     //#pragma omp parallel for private(j) <- does not improve run-time
     if(conc==1)
     for(i=0; i<nrows; i++)
          for(j=0; j<ncols; j++){
               if(flow_acc[i][j]>0 && L[i][j]>=0)
                    L[i][j]/=flow_acc[i][j];
               else
                    L[i][j]=-9999;
               }

     return L;
}
