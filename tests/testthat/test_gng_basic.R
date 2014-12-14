devtools::install(".")

library(testthat)
library("GrowingNeuralGas")
library(igraph)


for(k in 1:2){
  max_nodes <- 600
  
  # Construct gng object
  gng <- GNG(max_nodes=max_nodes, training=gng.train.online(dim=3), verbosity=10)
  
  # Construct examples, here we will use a sphere
  ex <- gng.preset.sphere(N=90000)
  insert_examples(gng, ex)
  
  # Run algorithm in parallel
  run(gng)
  
  # Wait for the graph to converge
  n <- 0
  print("Waiting to converge")
  while(number_nodes(gng) != gng$get_configuration()$max_nodes && n < 100) {
    Sys.sleep(1.0)
    n <- n + 1
  }
  
  test_that("GNG has reached expected number of vertexes", {
    expect_that(n < 100, is_true() )
  })
  print("Test::Completness test")
  
  # Find closest node
  predict(gng, c(1,1,1))
  
  # # Get igraph
  ig <- convert_igraph(gng)
  
  # # Running testthat unit tests (almost)
  test_that("GNG has not isolated vertexes", {
    expect_that(any(degree(ig)==0), is_false()) 
  })
  print("Test::No isolated vertexes")
  
  test_that("GNG has converged", {
    error_before = mean_error(gng)
    expect_that(error_before  < 50.0/max_nodes, is_true() )
  })
  print("Test::Convergence test")
  
  # Test memory
  terminate(gng)
  
  save.gng(gng, "graph.bin")
  
  print(paste("Finished ",k))
}
