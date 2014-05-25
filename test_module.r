  # Always clear workspace before installing new version of module
  # Because Module function is caching 
  rm(list=ls(all=TRUE))
  
  library(Rcpp)
  library(devtools)
  library(devtools)
  library(BH)
  library(inline)
  
  # Clean and install (load_all could have caused some problems - not sure here)
  devtools::clean_dll(".")
  devtools::install(".")
  
  # Install module
  x = library("GrowingNeuralGas")
  mo<-Module("gng_module", PACKAGE=x)
  
  
  # Test loading
  mo
  g = mo$GNGConfiguration
  s = mo$GNGServer

  s
  g
  mo$GNGConfiguration
  
  
  
  gngConfig = new(g)
  gngConfig$message_bufor_size
  gngConfig$max_nodes
  gngConfig$get_uniform_grid_axis()
  gngConfig$set_uniform_grid_axis(c(2,2,2))
  gngConfig$get_uniform_grid_axis()
  gngConfig$uniformgrid_optimization
  gngConfig$lazyheap_optimization
  
  
  gngServer = new(s, gngConfig)
  gngServer$get_configuration()$get_uniform_grid_axis()
  
  gngServer$get_number_nodes()
  gngServer$get_mean_error()

  m = matrix(nrow=10, ncol=3)
  for(i in 1:10){
    m[i,] = c(i,3,4)
  }
  k = matrix(nrow=10, ncol=3)
  for(i in 1:10){
    k[i,] = c(i,3,4)
  }  
  gngServer$insert_examples(m)
  gngServer$insert_examples(k)
  gngServer$dump_memory()
  
f<-function(){
    h = matrix(nrow=10, ncol=3)
    for(i in 1:10){
      h[i,] = c(i,3,4)
    }  
    gngServer$insert_examples(h)
}
f_set_mem<-function(){
  h = matrix(nrow=10, ncol=3)
  for(i in 1:10){
    h[i,] = c(i,9,9)
  }  
  gngServer$set_memory_move_examples(h)
}
  
h  
f()
gngServer$dump_memory()
gc()
f_set_mem()  
gngServer$dump_memory()  
gc()
gngServer  
  
  