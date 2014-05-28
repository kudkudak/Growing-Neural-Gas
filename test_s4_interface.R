devtools::install(".")

library("GrowingNeuralGas")



source("R/gng.r")
source("R/igraph-utils.r")
source("R/gng-visualize.r")
source("R/gng-presets.R")

server <- GNG(dim=3, uniformgrid_optimization=FALSE)

server$set_debug_level(10)

terminate(server)

insert_examples(server, preset=gng.preset.sphere, N=100000)

run(server)
pause(server)

ig = convert_igraph(server)



print(server)
summary(server)

plot(server, mode=gng.plot.rgl3d)

predict(server, c(1.0))
