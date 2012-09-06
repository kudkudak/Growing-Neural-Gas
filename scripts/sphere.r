source("gng.r")

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

while(1){
 iteration<-2
 ++iteration
 Sys.sleep(1)
 GNGVisualisePoints()
}
