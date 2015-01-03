library("GrowingNeuralGas")
data(wine, package="rattle")
scaled.wine <- scale(wine[-1])

# Train in an offline manner
gng <- GNG(scaled.wine, labels=wine$Type, max.nodes=70, 
           training=gng.train.offline(max.iter=10000, min.improvement=1e-1))


# Print number of nodes
numberNodes(gng)

# Print mean degree of the network
ig = convertToGraph(gng)
mean(degree(ig))

# Plot using igraph layout
plot(gng, mode = gng.plot.2d, 
     vertex.color=gng.plot.color.label, layout=igraph::layout.fruchterman.reingold)

# Print summary of trained object
print(summary(gng))