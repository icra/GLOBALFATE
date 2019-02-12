GLOBALFATE.run : read_write_rasters.o initial_contaminant_load.o area_m2_fun.o other_functions.o Flow_accumulation_m2.o Contaminant_accumulation.o RT_lakes_incorporation.o RT_rivers_calculator.o runoff_input.o main.o
	gcc -fopenmp read_write_rasters.o initial_contaminant_load.o area_m2_fun.o other_functions.o Flow_accumulation_m2.o Contaminant_accumulation.o RT_lakes_incorporation.o RT_rivers_calculator.o runoff_input.o main.o -o GLOBALFATE.run -lm 

main.o : main.c lib.h
	gcc -lm -fopenmp -c main.c
read_write_rasters.o : read_write_rasters.c lib.h
	gcc -lm -fopenmp -c read_write_rasters.c
area_m2_fun.o : area_m2_fun.c lib.h
	gcc -lm -fopenmp -c area_m2_fun.c 
other_functions.o : other_functions.c lib.h
	gcc -lm -fopenmp -c other_functions.c
Flow_accumulation_m2.o : Flow_accumulation_m2.c lib.h
	gcc -lm -fopenmp -c Flow_accumulation_m2.c
Contaminant_accumulation.o : Contaminant_accumulation.c lib.h
	gcc -lm -fopenmp -c Contaminant_accumulation.c
RT_lakes_incorporation.o : RT_lakes_incorporation.c lib.h
	gcc -lm -fopenmp -c RT_lakes_incorporation.c
RT_rivers_calculator.o : RT_rivers_calculator.c lib.h
	gcc -lm -fopenmp -c RT_rivers_calculator.c
runoff_input.o : runoff_input.c lib.h
	gcc -lm -fopenmp -c runoff_input.c
initial_contaminant_load.o : initial_contaminant_load.c lib.h
	gcc -lm -c -fopenmp initial_contaminant_load.c  	

clean:
	rm read_write_rasters.o initial_contaminant_load.o area_m2_fun.o other_functions.o Flow_accumulation_m2.o Contaminant_accumulation.o RT_lakes_incorporation.o RT_rivers_calculator.o runoff_input.o main.o
