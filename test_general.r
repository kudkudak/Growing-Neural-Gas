devtools::clean_dll(".")
devtools::install(".")

x = library("GrowingNeuralGas")
mo<-Module("gng_module", PACKAGE=x)



spherePoint<-function(){
  phi<-runif(1)*2*pi
  theta<-runif(1)*pi
  c(cos(theta)*cos(phi), cos(theta)*sin(phi), sin(theta))
}  

mat<-matrix(0,180000,4)

for(i in 1:180000){
  mat[i,1:3] = spherePoint()
  mat[i,4]=mat[i,1]
}

mo
g = mo$GNGConfiguration
s = mo$GNGServer
gngConfig = new(g)
gngConfig$beta
gngConfig$alpha
gngConfig$eps_v
gngConfig$eps_n
gngConfig$dataset_type = 3 # DatasetBaggingProb
gngServer = new(s, gngConfig)
gngServer$set_debug_level(10)
gngServer$set_memory_move_examples(mat)


gngServer$run()
gngServer$get_number_nodes()
gngServer$pause()

gngServer$get_node(1)
gngServer$get_node(2)
gngServer$get_node(3)
gngServer$get_node(234)
gngServer$get_mean_error()

gngServer$terminate()
source("R/gng-visualize.r")
source("R/igraph-utils.r")
gng.plot3d(gngServer)
#gng.plot2d(gngServer)
