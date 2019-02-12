/*
CALCULATOR OF CELL AREA AND WIDTH FOR WGS84 NOT PROJECTED WORLD RASTERS%

Carme Font (2017) after Francesco Bregoli (2016) after Rafa Marce (2015),
Catalan Institute for Water Research(ICRA), Girona, www.icra.cat

This function calculate the cells area in square meters and the cells width in meters for each latitude for a
single longitude (the distorition is the same in longitude). This script
works only for WGS84 (not projected) world rasters
Input are:
(1) area, the matrix of accumulated area which is indeed cumulated cells (NOT AREA)
(2) nrows, is the number of rows of the raster
(4) yllcorner, is the Southern cell latitude (as in ascii grid format). Need to be transformed in matlab matrix format
(5)R=6371007m is the authalic earth radius at equator
The output are
 (1) the area [m2] for each cell in latitude direction (so the result is only a vector coloumn)
 (2) the horizontal size (width) of the cell [m] for each cell in latitude direction (so the result is only a vector coloumn
 (3) the vertical size (height)  of the cell [m] only one value (the cell vertical length variation is here neglected)
*/

#include "lib.h"
#include "math.h"

void area_m2_fun(double resolution, int nrows, double yllcorner, double R, double *a_m2, double *width, double *height){

     int j;
     double latitude;

     *height = resolution*M_PI/180*R; //height of the cells, same value for the whole grid

     for(j=0; j<nrows; j++){
         latitude = yllcorner + resolution*(nrows-j-1); //the latitude in degree from the first matlab matrix cell(Northern)
         width[j] = (sin((latitude+resolution/2)*M_PI/180)-sin((latitude-resolution/2)*M_PI/180))*R; //cells width
         a_m2[j] = width[j]*(*height); //cells area depending on latitude
     }
}


