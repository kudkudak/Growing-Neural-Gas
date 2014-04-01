source("gng.r")

parallel(createGNGServer())
Sys.sleep(2.0)
sv<-new("GNGClient")

iteration<-0
mat <- matrix(rnorm(20,mean=1), 1000000,3)

for(j in 1:1000000){
	mat[j,1]<-runif(1)
	mat[j,2]<-runif(1)
	mat[j,3]<-(3*mat[j,1])
}

for(j in 1:1000000){
	for(i in 1:3){
		if(abs(mat[j,i])>4) mat[j,i]=0;
	} 	

}

sv$addExamples(mat)

sv$runServer()

