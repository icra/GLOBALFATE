/**
 GLOBFATE Model
 Authors: Carme Font, Francesco Bregoli and Rafael Marcé
 Contacts: cfont@icra.cat,rmarce@icra.cat
          Catalan Institute for Water Research (ICRA),
          Carrer Emili Grahit 101, 17003, Girona (Spain), Tel (+34) 972 18 33 80
          www.icra.cat
 Description: // The description will be supplied upon publication of the model in a peer-reviewed journal
 Last modification: February 2019
**/

#include "lib.h"
#include <unistd.h>
#include <omp.h>

int main(int argc, char *argv[]){
     /** Declaration of variables **/
     time_t time0, time1, time_i=time(NULL), time_e;
     double ref_raster[6], ref_area[6], ref_runoff[6], ref_slope[6], ref_lake_label[6], ref_V[6], ref_consum[6],
          ref_population[6], ref_treated[6];
     double R = 6371007; // The authalic Earth radius at equator
     char name[200], P[10];
     char dir[100]={"data/"};
     char dir2[100]={"results/"};
     int i, j, nl, save_all=1, num_lakes;
     float P1, P2;//
     /** Raster's pointers **/
     double **area, **direction, **runoff, **slope, **consum, **population, **lake_label, **V, **treated;
     /** Rasters for new data **/
     double *area_m2, *cell_width, cell_height, **hierarchy, **vel, **RT, *Qmax, **C, **L0;

     printf("Loading data...\n");
     /** Parameters from file **/
     double *parameters=calloc(11, sizeof(double));
     int args;
     strcpy(name, dir);
     strcat(name, "input_parameters.txt");
     read_parameters(name, parameters, &args);
     if(args<10){
          printf("Too few parameters\n");
          printf("Closing program...\n");
          return 0;
     }
     save_all = parameters[7];

     #pragma omp parallel sections
     {
     #pragma omp section
          area = read_raster(1, ref_area);
     #pragma omp section
          direction = read_raster(2, ref_raster);
     }
     
     /** Parameters from raster's header **/
     int ncols = ref_raster[0];         //number of columns available in ascii grid header
     int nrows = ref_raster[1];         //number of rows available in ascii grid header
     double yllcorner = ref_raster[3];  //in decimal degrees available in ascii grid header
     double resolution=ref_raster[4];   //in decimal degrees available in ascii grid header
     
     hierarchy=hierarchy_area(area, nrows, ncols, &nl);
     for(i=0; i<nrows; i++) free(area[i]);//area no longer needed

     /** GEOGRAPHICAL RELATED PROCESSES **/
     if(parameters[8]==1){
     printf("Running geographical related processes\n");
     #pragma omp parallel sections
     {
     #pragma omp section
          runoff = read_raster(3, ref_runoff);
     #pragma omp section
          slope = read_raster(4, ref_slope);
     #pragma omp section
          lake_label = read_raster(5, ref_lake_label);
     #pragma omp section
          V = read_raster(6, ref_V);
     }

     /** 1. running the area_m2 function **/
     printf("Running area m2...\n");
     area_m2=(double*)calloc(nrows,sizeof(double));
     cell_width=(double*)calloc(nrows,sizeof(double));
     area_m2_fun(resolution, nrows, yllcorner, R, area_m2, cell_width, &cell_height);

     /** 2. running the flow accumulation function; result in m3/years **/
     printf("Running flow accumulation...\n");
     Flow_accumulation_m2(direction,hierarchy,area_m2,runoff, nrows, ncols, nl);

     /** 3. running the retention time function **/
     printf("Running residence time...\n");
     vel=(double**)calloc(nrows,sizeof(double*));
     RT=(double**)calloc(nrows,sizeof(double*));
     for(i=0; i<nrows; i++){
          vel[i]=(double*)calloc(ncols, sizeof(double));
          RT[i]=(double*)calloc(ncols, sizeof(double));
     }
     Residence_time_rivers(direction,slope,runoff,cell_width, cell_height,parameters, vel, RT, nrows, ncols);
     for(i=0; i<nrows; i++) free(slope[i]);

     /** 4. running the lakes_RT_incorporation function **/
     printf("Running RT in lakes...\n");
     Qmax=lakes_RT_incorporation(lake_label,V,runoff,RT, nrows, ncols, &num_lakes);

     if(save_all==1){
     printf("Saving partial results\n");
     #pragma omp parallel sections
     {
          #pragma omp section
          write_vector("results/width.txt", cell_width, nrows);
          #pragma omp section
          write_vector("results/area_m2.txt", area_m2, nrows);
          #pragma omp section
          write_vector("results/Qmax.txt", Qmax, num_lakes);
          #pragma omp section
          write_raster("results/flow_acc.txt", ref_raster, runoff);
          #pragma omp section
          write_raster("results/vel.txt", ref_raster, vel);
          #pragma omp section
          write_raster("results/RT.txt", ref_raster, RT);
     }}

     /** Free data **/
     free(cell_width);
     free(area_m2);
     free(Qmax);
     for(i=0; i<nrows; i++) {
          free(vel[i]);
          free(V[i]);
          free(lake_label[i]);}
     }

     /** CONTAMINANT RELATED PROCESSES **/
     printf("Running contaminant related processes\n");
     #pragma omp parallel sections private(name)
     {
     #pragma omp section
          {
               if(parameters[8]==0) runoff = read_raster(10, ref_runoff);
          }
     #pragma omp section
     {
          if(parameters[8]==0) RT = read_raster(11, ref_raster);
     }
     #pragma omp section
          consum = read_raster(7, ref_consum);
     #pragma omp section
          population = read_raster(8, ref_population);
     #pragma omp section
          treated = read_raster(9, ref_treated);
     }
     
     /** parameters from call **/
     if(argc>1){
          P1 = atof(argv[1]);
          parameters[6] = P1;//k
		printf("k=%g\n", parameters[6]);
     }
     if(argc>2){
          P2 = atof(argv[2]);
          printf("e=%g\n", P2);
          parameters[10]=P2;//wwtp removal rate
     }
     if(argc>3){
          P2 = atof(argv[2]);
          printf("e=%g\n", P2);
          parameters[11]=P2;//wwtp removal rate
     }


     /** 5. running the Contaminant accumulation function **/
     printf("Running contaminant...\n");
     L0 = initial_contaminant_load(nrows, ncols, parameters[5], consum, population, treated, parameters[10]);
     C = Contaminant_accumulation(L0, direction, hierarchy, nl, runoff, RT, parameters, nrows, ncols);

     /** Save result **/
     strcpy(name, dir2);
     if(parameters[9]==1)//concentration
          strcat(name, "contaminant_C");
     if(parameters[9]==0)//load
          strcat(name, "contaminant_L");
     strcat(name, ".txt");
     write_raster(name, ref_raster, C);

     /** Free all **/
     for(i=0; i<nrows; i++){
          free(RT[i]);
          free(C[i]);
          free(hierarchy[i]);
          free(direction[i]);
          free(runoff[i]);
          free(consum[i]);
          free(population[i]);
          free(treated[i]);
     }
     free(RT); free(C); free(hierarchy); free(direction); free(runoff);
     free(consum); free(population); free(treated);

     time_e=time(NULL);
     printf("\nTotal elapsed time %ld s\n\n", time_e-time_i);
     if(argc==1) {printf("\nPress Enter to close\n\n");
     getchar();}
}
