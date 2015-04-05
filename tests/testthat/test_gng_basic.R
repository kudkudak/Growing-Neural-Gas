library(testthat)


test_that("GNG converges on simple cases", {

    online_converged <- function(gng){
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
    
    sanity_check <- function(gng){
      # Find closest node
      predict(gng, c(1,1,1))
      
      # # Get igraph
      ig <- convertToGraph(gng)
      
      # # Running testthat unit tests (almost)
      test_that("GNG has not isolated vertexes", {
        expect_that(any(degree(ig)==0), is_false()) 
      })
      
      test_that("GNG has converged", {
        error_before = meanError(gng)
        expect_that(error_before  < 50.0/max_nodes, is_true() )
      })
      
      # Test memory
      terminate(gng)
      save.gng(gng, "graph.bin")
    }


    max_nodes <- 600
    ex <- gng.preset.sphere(N=90000)
    gng <- GNG(max.nodes=max_nodes, training=gng.train.online(dim=3), verbosity=3)
    insertExamples(gng, ex)
    run(gng)
    online_converged(gng)
    sanity_check(gng) 

    gng <- GNG(ex, max.nodes=max_nodes, training=gng.train.offline(max.iter=1000), verbosity=3)
    sanity_check(gng) 


    gng <- OptimizedGNG(max.nodes=max_nodes, training=gng.train.online(dim=3), verbosity=3, value.range=c(-2,2))    
    insertExamples(gng, ex)
    run(gng)
    online_converged(gng)
    sanity_check(gng) 
})

test_that("GNG is working on mouse dataset", {
    data(cec_mouse_1_spherical)
    dataset = input
    gng <- GNG(dataset)
  expect_that(gng$getMeanError() < 0.1, is_true())
  expect_that(all(gng$clustering() == predict(gng,dataset)), is_true())
})

test_that("GNG clustering and predict are returning the same", {
  X <- replicate(10, rnorm(20))
  gng <- GNG(X)
  expect_that(all(gng$clustering() == predict(gng,X)), is_true())
})

test_that("GNG synchronization looks ok", {
    synchronization_test <- function(){

        data(cec_mouse_1_spherical)
        dataset = input
        gng <- GNG(dataset, verbosity=3, max.nodes=20)
        gng$.updateClustering()
        sum_1 = (sum( gng$clustering() != predict(gng, dataset)))

        gng <- GNG(train=gng.train.online(dim=2), verbosity=3, max.nodes=20)
        gng$insertExamples(dataset) 
        gng$pause()
        gng$.updateClustering()

        sum_2 = (sum( gng$clustering() != predict(gng, dataset)))
        
        expect_that(sum_1 == 0 && sum_2 == 0, is_true())
    }
    for(i in 1:3){
        synchronization_test()
    }
})
