library("GrowingNeuralGas")
library(igraph)
library(testthat)

max_nodes <- 600

# Construct gng object
gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3,
           experimental_utility_option = gng.experimental.utility.option.basic
           )





# Construct examples, here we will use a sphere
ex <- gng.preset.sphere(N=10000)
print(dim(ex))
ex_binded <- cbind(ex, rep(0.6, nrow(ex) ))


gng$set_debug_level(11)

gng$insert_examples(ex_binded)




# Run algorithm in parallel
run(gng)

mean_error(gng)

# Wait for it to converge
Sys.sleep(20.0)
print("Adding jumped distribution")
pause(gng)
plot(gng, start_s=4,  mode=gng.plot.2d.errors) #0.003 without utility



insert_examples(gng, preset=gng.preset.box, N=90000, prob=0.6, r=1.0, center=c(3.0,3.0,3.0))
run(gng)
print("Test::Jumped distribution added")
Sys.sleep(35.0)
pause(gng)
plot(gng, start_s=10, mode=gng.plot.2d.errors)
plot(gng, mode=gng.plot.rgl3d)



print("Test::Graph after jumped distribution")

ig <- GrowingNeuralGas::convert_igraph(gng)

# Running unit tests (almost)
test_that("GNG has not isolated vertexes", {
  expect_that(any(degree(ig)==0), is_false()) 
})

print("Test::No isolated vertexes")


test_that("GNG has converged", {
  error_before = mean_error(gng)
  expect_that(error_before, is_less_than(50.0/max_nodes) )
})

print("Test::Convergence test")



