library(raster)
setwd("~/projectes/GLOBAQUA - Model/GIS/contaminant_results")

raster_layer<-raster("mask_C2_nodesert_new.tif")
raster_layer[which(raster_layer[]<0)] <- -9999
writeRaster(raster_layer, "mask_C2_nodesert_new.asc", format="ascii", overwrite=TRUE)

raster_layer<-raster("BAU_swiss/contaminant_C_swiss_mask.tif")
raster_layer[which(raster_layer[]<0)] <- -9999
writeRaster(raster_layer, "BAU_swiss/contaminant_C_swiss_BAU_mask.asc", format="ascii", overwrite=TRUE)

raster_layer<-raster("BAU_consum/contaminant_C_consum_BAU_mask.tif")
raster_layer[which(raster_layer[]<0)] <- -9999
writeRaster(raster_layer, "BAU_consum/contaminant_C_consum_BAU_mask.asc", format="ascii", overwrite=TRUE)

raster_layer<-raster("SDG6_BAU_consum_swiss/contaminant_C_consum_swiss_SDG6_BAU_mask.tif")
raster_layer[which(raster_layer[]<0)] <- -9999
writeRaster(raster_layer, "SDG6_BAU_consum_swiss/contaminant_C_consum_swiss_SDG6_BAU_mask.asc", format="ascii", overwrite=TRUE)

raster_layer<-raster("mask_SDG.tif")
raster_layer[which(raster_layer[]<0)] <- -9999
writeRaster(raster_layer, "mask_SDG.asc", format="ascii", overwrite=TRUE)
