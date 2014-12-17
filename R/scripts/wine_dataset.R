library("GrowingNeuralGas")
data(wine, package="rattle")
scaled.wine <- scale(wine[-1])

#TODO: not 200 but 126

# Train in an offline manner
gng <- GNG(scaled.wine, labels=wine$Type, max.nodes=200, 
           training=gng.train.offline(max.iter=1000, min.improvement=0))
devtools::install(".")
devtools::load_all(".")

# Print number of nodes
numberNodes(gng)

# Print mean degree of the network
ig = convertToGraph(gng)
mean(degree(ig))

# Plot using igraph layout
plot(gng, mode = gng.plot.2d, 
     vertex.color=gng.plot.color.label, layout=igraph::layout.fruchterman.reingold)
