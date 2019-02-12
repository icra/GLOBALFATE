All results are placed in the folder 'results'. This folder must be created before running GLOBALFATE

The partial results of the model are saved if 'the 'save partial' flag in the file input_parameters is set to one:

width.txt - raster of wodht (m) of the river channels
area_m2.txt - raster of pixel areas (m2)
Qmax.txt - raster identifying the oulets of the lakes
flow_acc.txt - raster of streamflow (m3/s)
vel.txt - raster of water velocity (m/s)
RT.txt - Raster of water residence time (hours)

The final result of the model is saved as:
 - contaminant_C.txt, as ng/L, if concentration flag in the file input_parameters is set to one
 - contaminant_L.txt, as ng/year, if concentration flag in the file input_parameters is set to zero
