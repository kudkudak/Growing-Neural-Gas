source("gng.r")

GNGSetParams(max_nodes=25000, dim=3, orig=c(-1,-1,-1), axis=c(2,2,2), uniformgrid=FALSE, lazyheap=FALSE)
parallel(createGNGServer())

Sys.sleep(2.0)
sv<-new("GNGClient")


set.seed(3)
M<-matrix(0,100000,3)
for(i in 1:100000){
	M[i,1:3] = SpherePoint()
}

sv$addExamples(M)

sv$runServer()

iteration<-0
goal<-2
delta<-10
end_goal=24800

results=matrix(0,10000,2)
i<-1
start = proc.time()
while(sv$getNumberNodesOnline()<end_goal){
 if(sv$getNumberNodesOnline()>goal){
 	time<-(proc.time()-start)["elapsed"]
	results[i,1]=goal	
	results[i,2]=time
	i=i+1
	print(paste("Achieved ",goal, "in ",time))
	goal=goal+delta
	delta=max(10,10^(floor(log(sv$getNumberNodesOnline(),10))-1))
 
  }
}
