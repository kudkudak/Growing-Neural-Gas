devtools::clean_dll(".")
devtools::install(".")


x = library("GrowingNeuralGas")
mo<-Module("gng_module", PACKAGE=x)



spherePoint<-function(){
  phi<-runif(1)*2*pi
  theta<-runif(1)*pi
  c(cos(theta)*cos(phi), cos(theta)*sin(phi), sin(theta))
}  

mat<-matrix(0,40000,4)

for(i in 1:40000){
  mat[i,1:3] = spherePoint()
  mat[i,4]=mat[i,1]
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
gngConfig$max_nodes = 600
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


end_goal <- 40000
goal<-2
delta<-1000
i<-1
results=matrix(0,10000,2)
start = proc.time()
while(gngServer$get_number_nodes()<end_goal){
  if(gngServer$get_number_nodes()>goal){
    time<-(proc.time()-start)["elapsed"]
    results[i,1]=goal
    results[i,2]=time
    i=i+1
    print(paste("Achieved ",goal, "in ",time))
    goal=goal+delta
    #delta=max(10,10^(log(gngServer$get_number_nodes(),10)-1))
    
  }
}



plot(results[1:100,1],results[1:100,2], type="l", lty=2, lwd=2, xlab="Number of nodes", ylab="Time [s]", 
     ylim=c(0,80),xlim=c(0,end_goal), frame.plot=F)



gngServer$pause()

gngServer$get_node(1)
gngServer$get_node(2)
gngServer$get_node(3)
gngServer$get_node(234)
gngServer$get_mean_error()

gngServer$get_error_statistics()

gngServer$terminate()
source("R/gng-visualize.r")
source("R/igraph-utils.r")
gng.plot3d(gngServer)
gng.plot2d(gngServer)


rm(gngServer)
rm(gngConfig)
gc()




