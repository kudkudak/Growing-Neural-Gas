
mat <- matrix(rnorm(20,mean=1), 10000,3)

for(j in 1:10000){
	t<-rnorm(1,mean=0,sd=1)
	u<-rnorm(1,mean=0,sd=1)
	val<-3*sigmoid(t^2+u^2);
	mat[j,1] = 1.0
	mat[j,2] = 1.0
	mat[j,3] = 1.0 	

}

for(j in 1:10000){
	for(i in 1:3){
		if(abs(mat[j,i])>4) mat[j,i]=0;
	} 	

}

sv$addExamples(mat)



