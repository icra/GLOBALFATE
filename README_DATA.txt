All rasters must be in the folder 'data', as well as the files 'input_rasters.txt' and 'input_parameters.txt' described below.

/***********************************************************/

The file 'input_rasters.txt' contains the names of the input rasters.
The order of the rasters' names must be exactly as indicated here:
1) raster of hierarchical cells numbers (acc_area.txt)
2) raster of flow direction calculated with D8 method (fdr.txt)
3) raster of annual runoff in mm/year (runoff.txt)
4) raster of slope in m/m (slope.txt)
5) raster of lakes index (lake_index.txt) 
6) raster of lake Volume, raster of lake volume in m3 (lake_vol.txt)
7) raster of consumption, annual contaminant consumption per capita in g/inhab/year (consumption.txt)
8) raster of population (population.txt)
9) raster of the ratio of water treated over the total (wwtp.txt)

Rasters need to have EXACTLY the same extent (including nodata cells) and resolution. Take care of geo projection!

/***********************************************************/

The parameters' values must be in the file 'input_paramenters.txt'
following this order:
1) a: coeff parameter of channel WIDTH formula in Leopold and Maddock (1953)
2) b: coeff exponent of channel WIDTH formula in Leopold and Maddock (1953)
3) c: coeff parameter of channel DEPTH formula in Leopold and Maddock (1953)
4) d: coeff exponent of channel DEPTH formula in Leopold and Maddock (1953)
5) n: Manning coefficient in SI (s*m^(-1/3))
6) excretion rate: rate of excretion of the compound
7) k: DECAY RATE (hours^-1)
8) save partial: save partial results? 0 (no); 1 (yes)
9) geo: geographical processes needed? 0 (no); 1 (yes). In case of 0 (no) the files corresponding to Residence Time (RT.txt)  and flow accumulation (flow_acc.txt) are taken from the folder "results", where all the results are placed.
10) concentration: If you want to get contaminant concentration set 1; if you prefer contaminant load set 0
11) decay rate in wwtp's (fraction of load)
