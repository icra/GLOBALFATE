#include "lib.h"

double** initial_contaminant_load(int nrows, int ncols, double **consum, double **population, double **wwtp_connection, double *parameters){
     int i,j;
     double excretion_rate=parameters[5];
     double w=parameters[7];

     double **L=calloc(nrows, sizeof(double*));
     for(i=0; i<nrows; i++) L[i]=calloc(ncols, sizeof(double));
     for(i=0; i<nrows; i++)
          for(j=0; j<ncols; j++) L[i][j]=-9999;

     for(i=0; i<nrows; i++)
          for(j=0; j<ncols; j++){
               if(consum[i][j]>=0 && population[i][j]>=0 && wwtp_connection[i][j]>=0){
                    L[i][j] = consum[i][j] * population[i][j] * excretion_rate * (1 - w*wwtp_connection[i][j]/100) * pow(10, 6);//ng/year
               }
          }
     return L;
}
