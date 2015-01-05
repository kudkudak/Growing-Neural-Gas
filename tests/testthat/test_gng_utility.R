library(igraph)
library(testthat)

max_nodes <- 600

# Construct gng object
gng <- GNG(max.nodes=max_nodes, training = gng.train.online(dim=3), verbosity=10, k=1.3)


# Construct examples, here we will use a sphere
ex <- gng.preset.sphere(N=10000, prob=-1)
labels <- round(runif(10000)*3)
insertExamples(gng, ex, labels)



# Run algorithm in parallel
run(gng)
meanError(gng)


# Wait for it to converge
Sys.sleep(20.0)
print("Adding jumped distribution")
pause(gng)
plot(gng, mode=gng.plot.2d.errors) #0.068 without utility , 10 times less with

ex2 <- gng.preset.box(N=90000, r=1.0, center=c(3.0,3.0,3.0), prob=-1)
insertExamples(gng, ex2, labels)


run(gng)
print("Test::Jumped distribution added")
Sys.sleep(35.0)
pause(gng)
plot(gng, mode=gng.plot.2d.errors)

g <- convertToGraph(gng)
length(V(g))

if("rgl" %in% rownames(installed.packages()) == TRUE) {
  plot(gng, mode=gng.plot.rgl3d)
}



print("Test::Graph after jumped distribution")

ig <- convertToGraph(gng)

# Running unit tests (almost)
test_that("GNG has not isolated vertexes", {
  expect_that(any(degree(ig)==0), is_false()) 
})

print("Test::No isolated vertexes")


test_that("GNG has converged", {
  error_before = meanError(gng)
  expect_that(error_before < 50/max_nodes, is_true() )
})

print("Test::Convergence test")



terminate(gng)
