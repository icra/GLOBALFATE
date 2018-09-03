#include "lib.h"

double* lakes_RT_incorporation(double **lake_label, double **V, double **flow_acc, double **RT, int nrows, int ncols, int *num_lak){

     int i, r, c, number_of_lakes=0, I, Idx;
     double max_flow, current_lake;
     for(r=0; r<nrows; r++){
          for(c=0; c<ncols; c++){
               if(lake_label[r][c]>number_of_lakes)
                    number_of_lakes=lake_label[r][c];
          }
     }
     double *Qmax = calloc(number_of_lakes,sizeof(double));

     for (i=0; i<number_of_lakes; i++){
          current_lake=i+1;
          max_flow=-1;
          #pragma omp parallel
          {
               omp_set_num_threads(4);
          #pragma omp for private(c, I)
          for(r=0; r<nrows; r++)
          for(c=0; c<ncols; c++)
               if(lake_label[r][c]==current_lake){
               I=r+c*nrows;
               RT[r][c]=0;
               //find the max discharge within the lake, this point is considered the outlet
               #pragma omp flush (Idx, max_flow)
               if(flow_acc[r][c]>=max_flow){
                    #pragma omp critical
                    {
                    if(flow_acc[r][c]>=max_flow){
                     if(max_flow==flow_acc[r][c]){
                         if(Idx>I)
                              Idx=I;
                     }
                     if(flow_acc[r][c]>max_flow){
                         max_flow=flow_acc[r][c];
                         Idx=I;
                     }
                    }
                    }
               }
          }
          }
          Qmax[i] = max_flow/(365*24);//convert flow to m3/hours

          //assign the total RT at the outlet, leaving RT zero in the other part of  the lakes
          c=Idx/nrows;
          r=Idx-c*nrows;
          if (Qmax[i]>0)
               RT[r][c]=V[r][c]/Qmax[i];
          else
               RT[r][c]=0;
     }
     *num_lak=number_of_lakes;
     return Qmax;
}
