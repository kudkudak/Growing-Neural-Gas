# This file tests basic integration with RcppModule
# Do not use it as reference for interaction with GrowingNeuralGas

library('testthat')

x = library("GrowingNeuralGas")
mo<-Module("gng_module", PACKAGE=x)


spherePoint<-function(){
  phi<-runif(1)*2*pi
  theta<-runif(1)*pi
  c(cos(theta)*cos(phi), cos(theta)*sin(phi), sin(theta))
}  

mat<-matrix(0,400000,3)

for(i in 1:400000){
  mat[i,1:3] = spherePoint()
  mat[i,3]=mat[i,1]
}


mo
g = mo$GNGConfiguration
s = mo$GNGServer
s
gngConfig = new(g)
gngConfig$beta
gngConfig$alpha
gngConfig$eps_v
gngConfig$max_nodes = 60000
gngConfig$set_uniform_grid_axis(c(2,2,2))
gngConfig$set_uniform_grid_origin(c(-1,-1,-1))
gngConfig$uniformgrid_optimization = TRUE
gngConfig$get_uniform_grid_axis( )
gngConfig$lazyheap_optimization = TRUE
gngConfig$eps_n
gngConfig$dataset_type = 1 # DatasetBaggingProb
gngServer = new(s, gngConfig)
gngServer$set_debug_level(9)
gngServer$set_memory_move_examples(mat)


gngServer$run()
gngServer$get_number_nodes()

Sys.sleep(2)

gngServer$pause()

gngServer$get_node(1)
gngServer$get_node(2)
gngServer$get_node(3)

gngServer$get_mean_error()

gngServer$get_error_statistics()

gngServer$terminate()

rm(gngServer)
rm(gngConfig)
gc()


print("Passed test_basic_module")