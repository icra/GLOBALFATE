#include "lib.h"

void Residence_time_rivers(double **direction, double **slope, double **flow_acc, double *cell_width, double cell_height, double *parameters, double **v, double **RT, int nrows, int ncols){
     int i, j;
     double w, h, Rh, flow;
     double dt=0.66666666666666666;

     for(i=0; i<nrows; i++)
          for(j=0; j<ncols; j++)
          v[i][j]=-9999.0;


     for(i=0; i<nrows; i++){
          for(j=0; j<ncols; j++){
               if(flow_acc[i][j]>0 && slope[i][j]>0){
                    flow = flow_acc[i][j]/31536000; //Convert from m3/year to m3/s
                    w = parameters[0] * pow(flow,parameters[1]); //[m], raster of channels width: a*q^b
                    h = parameters[2] * pow(flow,parameters[3]); //[m], raster of channels heigth:c*q^d
                    Rh = (w * h) / (2*h + w); // [m] raster hydraulic radius
                    v[i][j] = 1/parameters[4] * pow(Rh,dt) * pow(slope[i][j],0.5);
               }
               if(flow_acc[i][j]==0 || slope[i][j]==0)
                    v[i][j]=0;
               //
          }
     }
     //Retention time calculation is different when the river path is N,S,W,E
     //direction (straigth direction) or in NW,NE,SE,SW (diagonal direction)

     //OBSERVE that if the flow direction cell has value 0, the algorithm
     //gives RT = 0. It means that in those cells there will be no decay (to stay in worst conditions).

     //initialize -9999
     for(i=0; i<nrows; i++)
          for(j=0; j<ncols; j++)
          RT[i][j]=-9999.0;

     for(i=0; i<nrows; i++){
          for(j=0; j<ncols; j++){
               if(flow_acc[i][j]>=0 && v[i][j]>0){
               //diagonal
               if(direction[i][j]== 2 || direction[i][j] == 8 || direction[i][j] == 32 || direction[i][j] == 128)
                    RT[i][j] = pow(pow(cell_height,2) + pow(cell_width[i],2),0.5) / v[i][j];

               //long
               if(direction[i][j] == 1 || direction[i][j] == 16)
                    RT[i][j] = cell_width[i] / v[i][j];

               //lat
               if(direction[i][j] == 4 || direction[i][j] == 64)
                    RT[i][j] = cell_height / v[i][j];

               //no dir
               if(direction[i][j] == 0)
                    RT[i][j] = (pow(pow(cell_height,2) + pow(cell_width[i],2),0.5) + cell_width[i] + cell_height)/3 / v[i][j]; //this is valid where the flow still rute, especially at the end point of a river (often river mouths were many observation are availables, around 43)
               //if the flow_acc have being posed to Nan where zero, the previous division
               //should be finite or NaN

               //convert RT from seconds to hours for next function application purpouse
               RT[i][j]=RT[i][j]/3600; //[hours]
               }
               if(v[i][j]==0)
                    RT[i][j]=0;
          }
     }
}
