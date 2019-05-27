#include "lib.h"

//this function must accept a variable number of arguments
double** initial_contaminant_load(int nrows, int ncols, double excretion_rate, double **consum, double **population, double **wwtp_connection, double wwtp_decay){
     int i,j;

     double **L=calloc(nrows, sizeof(double*));
     for(i=0; i<nrows; i++) L[i]=calloc(ncols, sizeof(double));
     for(i=0; i<nrows; i++)
          for(j=0; j<ncols; j++) L[i][j]=-9999;

     for(i=0; i<nrows; i++)
          for(j=0; j<ncols; j++){
               if(consum[i][j]>=0 && population[i][j]>=0 && wwtp_connection[i][j]>=0){
                    L[i][j] = consum[i][j] * population[i][j] * excretion_rate * (1 - wwtp_connection[i][j]/100*wwtp_decay) * pow(10, 6);//microgram/year
               }
          }
     return L;
}
