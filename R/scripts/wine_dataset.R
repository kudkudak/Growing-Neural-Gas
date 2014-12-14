
devtools::install(".")
devtools::load_all(".")


library("Growing-Neural-Gas")
data(wine, package="rattle")
head(wine)
df <- scale(wine[-1])
df <- apply(as.matrix(df), MARGIN = 2, FUN = function(X) (X - min(X))/diff(range(X)))

g <- GNG(as.matrix(df), max_nodes=20)

number_nodes(g)
error_statistics(g)


plot(g, mode = gng.plot.2d, vertex.color=gng.plot.color.cluster, layout=igraph::layout.fruchterman.reingold)
