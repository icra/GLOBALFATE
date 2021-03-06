#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <omp.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
//#include <sys/stat.h>
//#include <sys/types.h>


/*
gcc -lm -fopenmp main.c read_write_raster.c area_m2_fun.c other_functions.c Contaminant_accumulation.c Flow_accumulation_m2.c RT_lakes_incorporation.c RT_rivers_calculator.c runoff_input.c -o GLOBFATE
*/

int main(int argc, char *argv[]);
void area_m2_fun(double resolution, int nrows, double yllcorner, double R, double *a_m2, double *width, double *height);
double **Contaminant_accumulation(double **L, double **direction, double **hierarchy, int nl, double **flow_acc, double **RT, double *parameters, int nrows, int ncols);
double** initial_contaminant_load(int nrows, int ncols, double **consum, double **population, double **wwtp, double *parameters);
void Flow_accumulation_m2(double **direction, double **hierarchy, double *area_m2, double **runoff, int nrows, int ncols, int nl);
void runoff_input(int r, int c, int nrows, int ncols, double** direction, double** runoff);
void Residence_time_rivers(double **direction, double **slope, double **flow_acc, double *cell_width, double cell_height, double *parameters, double **v, double **RT, int nrows, int ncols);
double* lakes_RT_incorporation(double **lake_label, double **V, double **flow_acc, double **RT, int nrows, int ncols, int *num_lak);

/** in read_write_rasters **/
double read_head(FILE *file);
double** read_raster(char *name, double *ref_X);
void write_raster(char *name, double *ref_X, double **X);
void read_parameters(char *name, double *parameters, int *I);
int load_names(char **names, char *dir, char *name);
void write_vector(char *name, double *X, int nrows);

/** in other_functions **/
int check (double *v, int n);
void sortVector (double *v, int n);
int* find(double** area, double value, int *k, int nrows, int ncols);
double** hierarchy_area(double **area, int nrows, int ncols, int *nl);
double* unique2(double **X, int nl, int *KK);
double* unique(double **X, int nrows, int ncols, int *KK);
