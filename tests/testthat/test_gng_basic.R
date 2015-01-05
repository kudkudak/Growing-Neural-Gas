library(testthat)
library(igraph)

for(k in 1:3){
  print(k)
  max_nodes <- 600
  ex <- gng.preset.sphere(N=90000)
  if(k == 1){
    # Construct gng object
    gng <- GNG(max.nodes=max_nodes, training=gng.train.online(dim=3), verbosity=10)
    insertExamples(gng, ex)
    # Run algorithm in parallel
    run(gng)
  }
  else if(k == 2){
    # Construct gng object
    gng <- GNG(ex, max.nodes=max_nodes, training=gng.train.offline(max.iter=1000), verbosity=10)
  }
  else{
    # Construct gng object
    gng <- OptimizedGNG(max.nodes=max_nodes, training=gng.train.online(dim=3), verbosity=10, value.range=c(-2,2))    
    insertExamples(gng, ex)
    
    # Run algorithm in parallel
    run(gng)
  }
  # Construct examples, here we will use a sphere
  

  if(k!=2){
    # Wait for the graph to converge
    n <- 0
    print("Waiting to converge")
    while(numberNodes(gng) != gng$getConfiguration()$max_nodes && n < 100) {
      Sys.sleep(1.0)
      n <- n + 1
    }
    test_that("GNG has reached expected number of vertexes", {
      expect_that(n < 100, is_true() )
    })
  }
  

  print("Test::Completness test")
  
  # Find closest node
  predict(gng, c(1,1,1))
  
  # # Get igraph
  ig <- convertToGraph(gng)
  
  # # Running testthat unit tests (almost)
  test_that("GNG has not isolated vertexes", {
    expect_that(any(degree(ig)==0), is_false()) 
  })
  print("Test::No isolated vertexes")
  
  test_that("GNG has converged", {
    error_before = meanError(gng)
    expect_that(error_before  < 50.0/max_nodes, is_true() )
  })
  print("Test::Convergence test")
  
  # Test memory
  terminate(gng)
  
  save.gng(gng, "graph.bin")
  
  print(paste("Finished ",k))
}
