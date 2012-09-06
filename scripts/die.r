source("RcppInterface.r")
library("multicore")
library("rgl")
library("e1071")

iteration<-0

parallel(createGNGServer())
Sys.sleep(2.0)
sv<-new("GNGClient")

print(sv$getNumberNodesOnline())

mat <- matrix(rnorm(20,mean=1), 10000,3)

for(j in 1:10000){
	t<-rnorm(1,mean=0,sd=1)
	u<-rnorm(1,mean=0,sd=1)
	val<-3*sigmoid(t^2+u^2);
	mat[j,1] = t
	mat[j,2] = u
	mat[j,3] = val	

}

for(j in 1:10000){
	for(i in 1:3){
		if(abs(mat[j,i])>4) mat[j,i]=0;
	} 	

}

sv$addExamples(mat)


sv$runServer()
