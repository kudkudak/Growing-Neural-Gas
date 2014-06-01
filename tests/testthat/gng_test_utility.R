library("GrowingNeuralGas")
library(igraph)
library(testthat)

max_nodes <- 600

# Construct gng object
gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3,
           experimental_utility_option = gng.experimental.utility.option.basic,
           experimental_vertex_extra_data=TRUE
)

gng$get_configuration()$vertex_extra_data_dim



# Construct examples, here we will use a sphere
ex <- gng.preset.sphere(N=10000)
print(dim(ex))
ex_binded <- cbind(ex, rep(2.0, nrow(ex) ))
ex_binded <- cbind(ex_binded, rep(0.6, nrow(ex) ))
print(dim(ex_binded))

gng$set_debug_level(10)

gng$insert_examples(ex_binded)



# Run algorithm in parallel
run(gng)


mean_error(gng)


# Wait for it to converge
Sys.sleep(20.0)
print("Adding jumped distribution")
pause(gng)
plot(gng, start_s=4,  mode=gng.plot.2d.errors) #0.003 without utility


gng$set_debug_level(-10)
ex_2 <- gng.preset.box(N=90000, r=1.0, center=c(3.0,3.0,3.0))
print(dim(ex_2))
ex_binded_2 <- cbind(ex_2, rep(1.0, nrow(ex_2) ))
ex_binded_2 <- cbind(ex_binded_2, rep(0.6, nrow(ex_2) ))
insert_examples(gng, ex_binded_2)
gng$insert_examples(ex_binded_2)
ex_2[1,]
ex[1, ]
gng$set_debug_level(10)

run(gng)
print("Test::Jumped distribution added")
Sys.sleep(35.0)
pause(gng)
plot(gng, start_s=10, mode=gng.plot.2d.errors)
plot(gng, mode=gng.plot.rgl3d)


test_that("[EXPERIMENTAL] GNG extra data is working correctly", {
  expect_that(round(node(gng, predict(gng, ex_2[1,1:3]))$extra_data), equals(1))
  expect_that(round(node(gng, predict(gng, ex[1,1:3]))$extra_data), equals(2))
})


print("Test::Graph after jumped distribution")

ig <- GrowingNeuralGas::convert_igraph(gng)

# Running unit tests (almost)
test_that("GNG has not isolated vertexes", {
  expect_that(any(degree(ig)==0), is_false()) 
})

print("Test::No isolated vertexes")


test_that("[EXPERIMENTAL] GNG has converged", {
  error_before = mean_error(gng)
  expect_that(error_before, is_less_than(50.0/max_nodes) )
})

print("Test::Convergence test")



terminate(gng)
