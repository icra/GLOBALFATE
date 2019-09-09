/**
 GLOBAL-FATE Model
 Authors: Carme Font, Francesco Bregoli and Rafa Marcé
 Contact: cfont@icra.cat; rmarce@icra.cat
          Catalan Institute for Water Research (ICRA),
          Carrer Emili Grahit 101, 17003, Girona (Spain), Tel (+34) 972 18 33 80
          www.icra.cat
 Description: // The description will be supplied upon publication of the model in a peer-reviewed journal
 Last modification: sep 2019
**/

#include "lib.h"
/*
void make_directory(const char* name) 
   {
   #ifdef __linux__
       mkdir(name, 777); 
   #else
       _mkdir(name);
   #endif
   }
*/
int main(int argc, char *argv[]){
     /** Declaration of variables **/
     time_t time0, time1, time_i=time(NULL), time_e;
     double ref_raster[6]; //rasters' header
     double R = 6371007; // The authalic earth radius at equator
     char name[200]; // auxiliary string
     char dir[100]={'\0'}; // Change only if executable file is in a different directory than 'data' and 'results' folders
     int i, j, nl, save_all=1, num_lakes; //indexes
     
     /** Rasters' pointers **/
     double **area, **direction, **runoff, **slope, **consum, **population, **lake_label, **V, **treated;
     /** Rasters for new data **/
     double *area_m2, *cell_width, cell_height, **hierarchy, **vel, **RT, *Qmax, **C, **L0;

     /** Parameters from file **/
     printf("Loading data...\n");
     double *parameters=calloc(12, sizeof(double));// vector with parameters' values
     int args; 
     strcpy(name, dir);
     strcat(name, "data/input_parameters.txt");
     read_parameters(name, parameters, &args); //load parameters
     if(args<10){ //check number of parameters loaded
          printf("Too few parameters\n");
          printf("Closing program...\n");
          return 0;
     }
     save_all = parameters[8]; //getting saving options

     /** loading raster's names **/
     char **names=(char**)malloc(11*sizeof(char*));
     for(i=0; i<11; i++) names[i]=(char*)malloc(200*sizeof(char));
     if(load_names(names, dir, "input_rasters.txt")==0)
          return 0;


     /* loading 'basic' rasters */
     #pragma omp parallel sections
     {
     #pragma omp section
          area = read_raster(names[0], ref_raster);
     #pragma omp section
          direction = read_raster(names[1], ref_raster);
     }
     /** Parameters from raster's header **/
     int ncols = ref_raster[0];         //number of columns available in ascii grid raster
     int nrows = ref_raster[1];         //number of rows available in ascii grid raster
     double yllcorner = ref_raster[3];  //wetern coordinate in ascii grid raster
     double resolution=ref_raster[4];   //in decimal degrees available in ascii grid raster

     //function to transform the matrix 'area' to a two column matrix to save memory
     hierarchy=hierarchy_area(area, nrows, ncols, &nl);
     for(i=0; i<nrows; i++) free(area[i]);//area no longer needed

     /** GEOGRAPHICAL RELATED PROCESSES **/
     if(parameters[9]==1){
     printf("Running geographical related processes\n");
     /* Loading raster for the geographical related processes */
     #pragma omp parallel sections
     {
     #pragma omp section
          runoff = read_raster(names[2], ref_raster);
     #pragma omp section
          slope = read_raster(names[3], ref_raster);
     #pragma omp section
          lake_label = read_raster(names[4], ref_raster);
     #pragma omp section
          V = read_raster(names[5], ref_raster);
     }

     /** 1. running the area_m2 function **/
     printf("Running area m2...\n");
     area_m2=(double*)calloc(nrows,sizeof(double));
     cell_width=(double*)calloc(nrows,sizeof(double));
     area_m2_fun(resolution, nrows, yllcorner, R, area_m2, cell_width, &cell_height);

     /** 2. running the flow accumulation function; result in m3/years **/
     printf("Running flow accumulation...\n");
     Flow_accumulation_m2(direction,hierarchy,area_m2,runoff, nrows, ncols, nl);

     /** 3. running the residence time function **/
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
     //make_directory("results");
     #pragma omp parallel sections
     {
          #pragma omp section
          write_vector("results/width.txt", cell_width, nrows); //vector of raster's cells width
          #pragma omp section
          write_vector("results/area_m2.txt", area_m2, nrows); //vector raster's cells area
          #pragma omp section
          write_vector("results/Qmax.txt", Qmax, num_lakes); //vector of lakes' outlet
          #pragma omp section
          write_raster("results/flow_acc.txt", ref_raster, runoff); //raster of flow accumulation
          #pragma omp section
          write_raster("results/vel.txt", ref_raster, vel); //raster of flow velocity
          #pragma omp section
          write_raster("results/RT.txt", ref_raster, RT); //raster of residence time
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
     if(parameters[9]==0){ // If geographical processes are not run, then 'flow_acc' and 'RT' are taken from the results' folder
        strcpy(names[9], dir);
        strcat(names[9], "results/flow_acc.txt");
        strcpy(names[10], dir);
        strcat(names[10], "results/RT.txt");
     }
     
     #pragma omp parallel sections private(name)
     {
     #pragma omp section
          if(parameters[9]==0) runoff = read_raster(names[9], ref_raster);
     #pragma omp section
          if(parameters[9]==0) RT = read_raster(names[10], ref_raster);
     #pragma omp section
          consum = read_raster(names[6], ref_raster);
     #pragma omp section
          population = read_raster(names[7], ref_raster);
     #pragma omp section
          treated = read_raster(names[8], ref_raster);
     }
     free(names);


     /** 5. running the Contaminant accumulation function **/
     printf("Running contaminant...\n");
     L0 = initial_contaminant_load(nrows, ncols, consum, population, treated, parameters); //initial contaminant load
     C = Contaminant_accumulation(L0, direction, hierarchy, nl, runoff, RT, parameters, nrows, ncols); //concentration after river attenuation

     /** Save result **/
     strcpy(name, dir);
     if(parameters[10]==1)//concentration
          strcat(name, "results/contaminant_C");
     if(parameters[10]==0)//load
          strcat(name, "results/contaminant_L");
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
