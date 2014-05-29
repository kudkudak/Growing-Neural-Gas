library(testthat)


for(k in 1:2){
  
  library("GrowingNeuralGas")
  library(igraph)
  
  max_nodes <- 600
  
  # Construct gng object
  gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3,
                uniformgrid_optimization=FALSE,  lazyheap_optimization=FALSE)
  
  
  # Construct examples, here we will use a sphere
  ex <- gng.preset.sphere(N=90000)
  print(dim(ex))
  ex_binded <- cbind(ex, rep(0.6, nrow(ex) ))
  
  
  gng$set_debug_level(11)
  
  # Set examples pointer, note: this is the most
  # efficient way to pass examples, however you can use it
  # only once (it sets hard link to memory inside)
  gng$set_memory_move_examples(ex_binded)
  
  # Run algorithm in parallel
  run(gng)
  
  # Wait for the graph to converge
  n <- 0
  print("Waiting to converge")
  while(number_nodes(gng) != gng$get_configuration()$max_nodes && n < 100) {
      Sys.sleep(1.0)
      n <- n + 1
  }
  # print("Pausing GNG")
  # pause(gng)
  
  test_that("GNG has reached expected number of vertexes", {
    expect_that(n, is_less_than(100) )
  })
  print("Test::Completness test")
  
  # 
  # # Get igraph
  ig <- convert_igraph(gng)
  # 
  # 
  # # Running testthat unit tests (almost)
  test_that("GNG has not isolated vertexes", {
    expect_that(any(degree(ig)==0), is_false()) 
  })
  
  print("Test::No isolated vertexes")
  
  
  test_that("GNG has converged", {
    error_before = mean_error(gng)
    expect_that(error_before, is_less_than(50.0/max_nodes) )
  })
  print("Test::Convergence test")
  
  # Test memory
  terminate(gng)
  # Sys.sleep(2.0)
  rm(gng)
  gc()
  
  print(paste("Finished ",k))

}
