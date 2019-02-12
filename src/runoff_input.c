#include "lib.h"
void runoff_input(int r, int c, int nrows, int ncols, double** direction, double** runoff){

     //runoff from NW
     if(c>0 && r>0)
          if(direction[r-1][c-1]==2 && runoff[r-1][c-1]>0)
               runoff[r][c] += runoff[r-1][c-1];

     if(c==0 && r>0)
          if(direction[r-1][ncols-1]==2 && runoff[r-1][ncols-1]>0)
               runoff[r][c] += runoff[r-1][ncols-1];

     //runoff from NN
     if(r>0)
          if(direction[r-1][c]==4 && runoff[r-1][c]>0)
               runoff[r][c] += runoff[r-1][c];

     //runoff from NE
     if(c<(ncols-1) && r>0)
          if(direction[r-1][c+1]==8 && runoff[r-1][c+1]>0)
               runoff[r][c] += runoff[r-1][c+1];

     if(c==(ncols-1) && r>0)
          if(direction[r-1][0]==8 && runoff[r-1][0]>0)
               runoff[r][c] += runoff[r-1][0];

     //runoff from WW
     if(c>0)
          if(direction[r][c-1]==1 && runoff[r][c-1]>0)
               runoff[r][c] += runoff[r][c-1];

     if(c==0)
          if(direction[r][ncols-1]==1 && runoff[r][ncols-1]>0)
               runoff[r][c] += runoff[r][ncols-1];

     //runoff from EE
     if(c<(ncols-1))
          if(direction[r][c+1]==16 && runoff[r][c+1]>0)
               runoff[r][c] += runoff[r][c+1];

     if(c==(ncols-1))
          if(direction[r][0]==16 && runoff[r][0]>0)
               runoff[r][c] += runoff[r][0];

     //runoff from SW
     if(c>0 && r<(nrows-1))
          if(direction[r+1][c-1]==128 && runoff[r+1][c-1]>0)
               runoff[r][c] += runoff[r+1][c-1];

     if(c==0 && r<(nrows-1))
          if(direction[r+1][nrows-1]==128 && runoff[r+1][nrows-1]>0)
               runoff[r][c] += runoff[r+1][nrows-1];

     //runoff from SS
     if(r<(nrows-1))
          if(direction[r+1][c]==64 && runoff[r+1][c]>0)
               runoff[r][c] += runoff[r+1][c];

     //runoff from SE
     if(c<(ncols-1) && r<(nrows-1))
          if(direction[r+1][c+1]==32 && runoff[r+1][c+1]>0)
               runoff[r][c] += runoff[r+1][c+1];

     if(c==(ncols-1) && r<(nrows-1))
          if(direction[r+1][0]==32 && runoff[r+1][0]>0)
               runoff[r][c] += runoff[r+1][0];
}

