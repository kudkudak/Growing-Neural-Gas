source("gng.r")
#GNGSetParams(max_nodes=1000)
parallel(createGNGServer())
Sys.sleep(2.0)
sv<-new("GNGClient")

iteration<-0

mat <- matrix(rnorm(20,mean=1), 100000,3)
print("adding examples")
for(j in 1:100000){
	t<-rnorm(1,mean=0,sd=1)
	u<-rnorm(1,mean=0,sd=1)
	val<-3;
	mat[j,1] = t
	mat[j,2] = u
	mat[j,3] = val	

}

for(j in 1:100000){
	for(i in 1:3){
		if(abs(mat[j,i])>4) mat[j,i]=0;
	} 	

}

sv$addExamples(mat)

source("VisualisePoints.r")
