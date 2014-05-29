devtools::clean_dll(".")
devtools::install(".")
devtools::load_all(".")

library("GrowingNeuralGas")


server <- GNG(dim=3, dataset_type=gng.dataset.sequential, 
              uniformgrid_optimization=TRUE, 
              uniformgrid_boundingbox_sides=c(4,4,4), uniformgrid_boundingbox_origin=c(-2,-2,-2))





insert_examples(server, preset=gng.preset.sphere, N=10000)
insert_examples(server, preset=gng.preset.plane, N=10000)
run(server)
pause(server)


number_nodes(server)

server$set_debug_level(0)
ig = convert_igraph(server)
V(ig)

.visualizeIGraphRGL(ig)

summary(server)

library(igraph)

predict(server, c(0.3, 0.3, 0.3))




print(server)
summary(server)
plot(server, mode=gng.plot.rgl3d)
plot(server, mode=gng.plot.2d, cluster=TRUE, layout_2d=FALSE)


x = .sphere.point(r=0.5, center=c(0.5,0.5,0.5))
x
node(server, predict(server, x))
