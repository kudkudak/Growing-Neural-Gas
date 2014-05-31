# Simple sphere dataset

library("GrowingNeuralGas")
library(igraph)

max_nodes <- 600

# Construct gng object
gng <- GNG(dataset_type=gng.dataset.bagging.prob, max_nodes=max_nodes, dim=3,
           uniformgrid_optimization=TRUE,  lazyheap_optimization=FALSE,
           uniformgrid_boundingbox_sides=c(3,3,3), uniformgrid_boundingbox_origin=c(-0.5,-0.5,-0.5))


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

pause(gng)

plot(gng, mode=gng.plot.2d.errors, layout_2d=TRUE, cluster=TRUE)

# Test memory
terminate(gng)
rm(gng)
gc()

