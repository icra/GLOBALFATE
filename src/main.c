/**
 GLOBFATE Model
 Authors: Carme Font, Francesco Bregoli and Rafa Marcé
 Contact: cfont@icra.cat
          Catalan Institute for Water Research (ICRA),
          Carrer Emili Grahit 101, 17003, Girona (Spain), Tel (+34) 972 18 33 80
          www.icra.cat
 Description:
 Last modification: dec 2017
**/

#include "lib.h"

int main(int argc, char *argv[]){
     /** Declaration of variables **/
     time_t time0, time1, time_i=time(NULL), time_e;
     double ref_raster[6], ref_area[6], ref_runoff[6], ref_slope[6], ref_lake_label[6], ref_V[6], ref_consum[6],
          ref_population[6], ref_trated[6], ref_nosewage[6];
     double R = 6371007; // The authalic earth radius at equator
     char name[200], P[10];
     char dir[100]={"C:/Users/cfont/Documents/projectes/GLOBAQUA - Model/GLOBAL-FATE C/"};
     int i, j, nl, save_all=1, num_lakes;
     float P1, P2;
     /** Raster's pointers **/
     double **area, **direction, **runoff, **slope, **consum, **population, **lake_label, **V, **treated, **nosewage;
     /** Rasters for new data **/
     double *area_m2, *cell_width, cell_height, **hierarchy, **vel, **RT, *Qmax, **C, **L0;

     printf("Loading data...\n");
     /** Parameters from file **/
     double *parameters=calloc(12, sizeof(double));// = {7.2, 0.5, 0.27, 0.39, 0.044};
     int args;
     strcpy(name, dir);
     strcat(name, "data/input_parameters.txt");
     read_parameters(name, parameters, &args);
     if(args<10){
          printf("Too few parameters\n");
          printf("Closing program...\n");
          return 0;
     }
     save_all = parameters[7];
     double e1=1, e2=1;
     parameters[10]=e1;
     parameters[11]=e2;

     /** loading raster's names **/
     char **names=(char**)malloc(11*sizeof(char*));
     for(i=0; i<11; i++) names[i]=(char*)malloc(200*sizeof(char));
     if(load_names(names, dir)==0)
          return 0;

     #pragma omp parallel sections
     {
     #pragma omp section
          area = read_raster(names[0], ref_area);
     #pragma omp section
          direction = read_raster(names[1], ref_raster);
     }
     /** Parameters from raster's header **/
     int ncols = ref_raster[0];         //number of columns available in ascii grid header
     int nrows = ref_raster[1];         //number of rows available in ascii grid header
     double yllcorner = ref_raster[3];  //in decimal degrees available in ascii grid header
     double resolution=ref_raster[4];   //in decimal degrees available in ascii grid header
     //nan_unif(nrows, ncols, area, direction, slope, consum, population, wwtp_connection);

     hierarchy=hierarchy_area(area, nrows, ncols, &nl);
     for(i=0; i<nrows; i++) free(area[i]);//area no longer needed

     /** GEOGRAPHICAL RELATED PROCESSES **/
     if(parameters[8]==1){
     printf("Running geographical related processes\n");
     #pragma omp parallel sections
     {
     #pragma omp section
          runoff = read_raster(names[2], ref_runoff);
     #pragma omp section
          slope = read_raster(names[3], ref_slope);
     #pragma omp section
          lake_label = read_raster(names[4], ref_lake_label);
     #pragma omp section
          V = read_raster(names[5], ref_V);
     }

     /*if(check_rasters(ref_raster, ref_direction, ref_runoff, ref_slope, ref_lake_label, ref_V, ref_consum,
          ref_population, ref_connection)==0)
          return 0;*/

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
               strcpy(name, dir);
               strcat(name, "results/flow_acc.txt");
               //printf("flow acc name %s\n", name);
               if(parameters[8]==0) runoff = read_raster(name, ref_runoff);
          }
     #pragma omp section
     {
          strcpy(name, dir);
          strcat(name, "results/RT.txt");
          //printf("RT name %s\n", name);
          if(parameters[8]==0) RT = read_raster(name, ref_raster);
     }
     #pragma omp section
          consum = read_raster(names[6], ref_consum);
     #pragma omp section
          population = read_raster(names[7], ref_population);
     #pragma omp section
          treated = read_raster(names[8], ref_treated);
     #pragma omp section
          nosewage = read_raster(names[9], ref_nosewage);
     }
     free(names);

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
     L0 = initial_contaminant_load(nrows, ncols, parameters[5], consum, population, treated, nosewage, parameters);
     C = Contaminant_accumulation(L0, direction, hierarchy, nl, runoff, RT, parameters, nrows, ncols);

     /** Save result **/
     strcpy(name, dir);
     if(parameters[9]==1)//concentration
          strcat(name, "results/contaminant_C");
     if(parameters[9]==0)//load
          strcat(name, "results/contaminant_L");
     /*if(argc>1) {
          strcat(name, "_");
          snprintf(P, sizeof(P), "%.2f", P1);
          strcat(name, P);
     }
     if(argc>2){
          strcat(name, "_");
          snprintf(P, sizeof(P), "%.2f", P2);
          strcat(name, P);
     }*/
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
          free(wwtp_connection[i]);
     }
     free(RT); free(C); free(hierarchy); free(direction); free(runoff);
     free(consum); free(population); free(wwtp_connection);

     time_e=time(NULL);
     printf("\nTotal elapsed time %ld s\n\n", time_e-time_i);
     if(argc==1) {printf("\nPress Enter to close\n\n");
     getchar();}
}
