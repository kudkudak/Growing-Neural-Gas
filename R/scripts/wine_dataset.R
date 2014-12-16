library(GrowingNeuralGas)
require(c("igraph", "rattle"))
data(wine, package="rattle")
scaled_wine <- scale(wine[-1])

# Train in an offline manner
gng <- GNG(scaled_wine, labels=wine$Type, max_nodes=20)

# Print number of nodes
nodes <- numberNodes(gng)

# Print mean degree of the network
ig = convertToGraph(gng)
mean(degree(ig))

# Plot using igraph layout
plot(gng, mode = gng.plot.2d, 
     vertex.color=gng.plot.color.label, layout=igraph::layout.fruchterman.reingold)
