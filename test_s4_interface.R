devtools::clean_dll(".")
devtools::install(".")
devtools::test(".")
library("GrowingNeuralGas")


server <- GNG(dim=3, dataset_type=gng.dataset.sequential, max_nodes=1000,
              uniformgrid_optimization=TRUE, 
              uniformgrid_boundingbox_sides=c(4,4,4), uniformgrid_boundingbox_origin=c(-2,-2,-2))




insert_examples(server, preset=gng.preset.sphere, N=10000)
insert_examples(server, preset=gng.preset.plane, N=10000)
run(server)
Sys.sleep(40)
pause(server)


number_nodes(server)

server$set_debug_level(10)
ig = convert_igraph(server)
V(ig)

.visualizeIGraphRGL(ig)

summary(server)

library(igraph)

??plot.gng

predict(server, c(0.3, 0.3, 0.3))




print(server)
summary(server)
plot(server, mode=gng.plot.rgl3d)
plot(server, mode=gng.plot.2d, cluster=TRUE, layout_2d=FALSE)


x = .sphere.point(r=0.5, center=c(0.5,0.5,0.5))
x
node(server, predict(server, x))

# Performance test
# end_goal <- 40000
# goal<-2
# delta<-1000
# i<-1
# results=matrix(0,10000,2)
# start = proc.time()
# while(gngServer$get_number_nodes()<end_goal){
#   if(gngServer$get_number_nodes()>goal){
#     time<-(proc.time()-start)["elapsed"]
#     results[i,1]=goal
#     results[i,2]=time
#     i=i+1
#     print(paste("Achieved ",goal, "in ",time))
#     goal=goal+delta
#     #delta=max(10,10^(log(gngServer$get_number_nodes(),10)-1))
#     
#   }
# }
# 
# 
# 
# plot(results[1:100,1],results[1:100,2], type="l", lty=2, lwd=2, xlab="Number of nodes", ylab="Time [s]",
#      ylim=c(0,80),xlim=c(0,end_goal), frame.plot=F)
